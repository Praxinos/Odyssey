// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#include "OdysseyPaintEngine.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "Editor.h"
#include "Engine/Texture2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Misc/AutomationTest.h"
#include "RenderGraphBuilder.h"
#include "RenderGraphUtils.h"
#include "RenderingThread.h"
#include "ScopedTransaction.h"
#include "TextureCompiler.h"
#include "TextureResource.h"
#include "UObject/StrongObjectPtr.h"

#include "OdysseyRasterBlockEdit.h"
#include "OdysseyRasterContent.h"
#include "OdysseyRasterEditSession.h"
#include "OdysseyRenderContext.h"
#include "OdysseyTextureLayerImageRaster.h"
#include "OdysseyTextureLayerStack.h"
#include "OdysseyTextureLayerStackUserData.h"
#include "OdysseyTiledImage.h"
#include "OdysseyRasterContentBlocks.h"

namespace Odyssey::PaintEngineTests
{
constexpr EAutomationTestFlags TestFlags = EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter;
constexpr int32 Width = 90;
constexpr int32 Height = 70;
const FLinearColor Background(0.25f, 0.5f, 0.75f, 1.0f);
const FIntRect StrokeRect(10, 12, 30, 25);

/** Returns a raster layer of a new BGRA8 texture filled with the background color. */
UOdysseyTextureLayerImageRaster* MakeLayer(TStrongObjectPtr<UTexture2D>& OutTexture)
{
    OutTexture.Reset(NewObject<UTexture2D>(GetTransientPackage(), NAME_None, RF_Public | RF_Transactional));
    TArray<FColor> Pixels;
    Pixels.Init(Background.QuantizeRound(), Width * Height);
    OutTexture->Source.Init(Width, Height, 1, 1, TSF_BGRA8, reinterpret_cast<const uint8*>(Pixels.GetData()));
    OutTexture->SRGB = false;
    OutTexture->MipGenSettings = TMGS_NoMipmaps;
    OutTexture->CompressionSettings = TC_VectorDisplacementmap;
    OutTexture->UpdateResource();
    FTextureCompilingManager::Get().FinishCompilation({ OutTexture.Get() });

    UOdysseyTextureLayerStackUserData* UserData = NewObject<UOdysseyTextureLayerStackUserData>(OutTexture.Get());
    OutTexture->AddAssetUserData(UserData);
    UserData->InitWithDefaultLayerStack();
    UserData->LayerStack->SetTextureUpdateMode(EOdysseyTextureLayerStackTextureUpdateMode::Manually);
    return CastChecked<UOdysseyTextureLayerImageRaster>(UserData->LayerStack->GetCurrentLayer());
}

/** Draws an opaque red stroke into the paint block, as a brush does. */
void PaintStroke(FOdysseyPaintEngine& Engine)
{
    TSharedPtr<::ULIS::FBlock> PaintBlock = Engine.PaintBlock();
    for (int32 Y = StrokeRect.Min.Y; Y < StrokeRect.Max.Y; ++Y)
    {
        for (int32 X = StrokeRect.Min.X; X < StrokeRect.Max.X; ++X)
        {
            uint8* Pixel = PaintBlock->PixelBits(static_cast<uint16>(X), static_cast<uint16>(Y));
            Pixel[0] = 0;
            Pixel[1] = 0;
            Pixel[2] = 255;
            Pixel[3] = 255;
        }
    }
    PaintBlock->Dirty(::ULIS::FRectI::FromMinMax(StrokeRect.Min.X, StrokeRect.Min.Y, StrokeRect.Max.X, StrokeRect.Max.Y));
}

TArray<FLinearColor> ReadTarget(UTextureRenderTarget2D* Target)
{
    FlushRenderingCommands();
    TArray<FLinearColor> Pixels;
    FReadSurfaceDataFlags Flags(RCM_MinMax);
    Flags.SetLinearToGamma(false);
    Target->GameThread_GetRenderTargetResource()->ReadLinearColorPixels(Pixels, Flags);
    return Pixels;
}

/** Reads the pixel shown by the views, i.e. through the active edit sessions. */
FLinearColor ReadShown(const UOdysseyTextureLayerImageRaster* Layer, FIntPoint Position)
{
    FOdysseyRenderContext Context;
    FOdysseyRasterEditSession::AddOverrides(Context);
    IOdysseyTextureRenderingAbility::FRenderFunction Draw;
    if (!Layer->BuildRenderPipeline(Draw, {}, &Context))
    {
        return FLinearColor(-1.0f, -1.0f, -1.0f, -1.0f);
    }
    TStrongObjectPtr<UTextureRenderTarget2D> Target(NewObject<UTextureRenderTarget2D>());
    Target->InitCustomFormat(Width, Height, PF_A32B32G32R32F, true);
    Target->UpdateResourceImmediate(true);
    ENQUEUE_RENDER_COMMAND(OdysseyPaintEngineTestRender)(
        [Target = Target.Get(), Draw = MoveTemp(Draw)](FRHICommandListImmediate& RHICmdList)
        {
            FRDGBuilder Graph(RHICmdList);
            FRDGTextureRef Destination = Target->GetRenderTargetResource()->GetRenderTargetTexture(Graph);
            AddClearRenderTargetPass(Graph, Destination, FLinearColor::Transparent);
            Draw(Graph, GMaxRHIFeatureLevel, Destination, FIntRect(0, 0, Width, Height), FIntRect(0, 0, Width, Height), FMatrix::Identity);
            Graph.Execute();
        });
    const TArray<FLinearColor> Pixels = ReadTarget(Target.Get());
    return Pixels.IsValidIndex(Position.Y * Width + Position.X) ? Pixels[Position.Y * Width + Position.X] : FLinearColor(-1.0f, -1.0f, -1.0f, -1.0f);
}

/** Reads a pixel of the tiles of the layer, i.e. its committed content. */
FLinearColor ReadCommitted(const UOdysseyTextureLayerImageRaster* Layer, FIntPoint Position)
{
    TStrongObjectPtr<UTextureRenderTarget2D> Target(NewObject<UTextureRenderTarget2D>());
    Target->InitCustomFormat(Width, Height, PF_A32B32G32R32F, true);
    Target->UpdateResourceImmediate(true);
    Layer->GetTiledImage()->Render(Target.Get(), FIntRect(0, 0, Width, Height), FIntPoint::ZeroValue);
    const TArray<FLinearColor> Pixels = ReadTarget(Target.Get());
    return Pixels.IsValidIndex(Position.Y * Width + Position.X) ? Pixels[Position.Y * Width + Position.X] : FLinearColor(-1.0f, -1.0f, -1.0f, -1.0f);
}

bool IsShown(const UOdysseyTextureLayerImageRaster* Layer)
{
    FOdysseyRenderContext Context;
    FOdysseyRasterEditSession::AddOverrides(Context);
    return Context.Overrides.Contains(Layer->GetRenderingId());
}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOdysseyPaintEngineTest, "Odyssey.PainterEditor.PaintEngine", Odyssey::PaintEngineTests::TestFlags)
/**
 * The paint engine shows a stroke through an edit of the layer without changing its tiles,
 * discards it on abort, draws it again from the original pixels on reset, and writes it as one undoable change on commit.
 */
bool FOdysseyPaintEngineTest::RunTest(const FString& Parameters)
{
    using namespace Odyssey::PaintEngineTests;
    TStrongObjectPtr<UTexture2D> Texture;
    UOdysseyTextureLayerImageRaster* Layer = MakeLayer(Texture);
    const FOdysseyBlendParameters Blend(EOdysseyBlendMode::Normal, 100.f);
    const FIntPoint Inside(15, 15);
    const FIntPoint Outside(60, 50);
    const float Tolerance = 0.5f / 255.0f + 0.0001f;
    const FLinearColor Red(1.0f, 0.0f, 0.0f, 1.0f);

    // Abort: the views show the layer again, the tiles are unchanged.
    FOdysseyPaintEngine Engine;
    Engine.Edit(Odyssey::RasterContentBlocks::BeginBlockEdit(Layer->GetRasterContent()));
    if (!TestTrue(TEXT("Paint into an edit of the layer"), Engine.GetEdit().IsValid() && Engine.PaintBlock().IsValid()))
    {
        return false;
    }
    PaintStroke(Engine);
    Engine.Update(Blend);
    TestTrue(TEXT("The views show the stroke"), ReadShown(Layer, Inside).Equals(Red, Tolerance));
    TestTrue(TEXT("The views show the pixels outside the stroke"), ReadShown(Layer, Outside).Equals(Background, Tolerance));
    TestTrue(TEXT("The tiles are not painted before the commit"), ReadCommitted(Layer, Inside).Equals(Background, Tolerance));
    Engine.Abort();
    TestFalse(TEXT("An aborted stroke ends its edit"), Engine.GetEdit().IsValid() || IsShown(Layer));
    TestTrue(TEXT("An aborted stroke leaves the tiles"), ReadCommitted(Layer, Inside).Equals(Background, Tolerance));

    // Reset: the edit goes on from the original pixels.
    Engine.Edit(Odyssey::RasterContentBlocks::BeginBlockEdit(Layer->GetRasterContent()));
    PaintStroke(Engine);
    Engine.Update(Blend);
    Engine.Reset();
    Engine.Update(Blend);
    TestTrue(TEXT("A reset keeps the edit"), Engine.GetEdit().IsValid() && IsShown(Layer));
    TestTrue(TEXT("A reset shows the original pixels"), ReadShown(Layer, Inside).Equals(Background, Tolerance));

    // Commit: one undoable change of the tiles.
    PaintStroke(Engine);
    {
        FScopedTransaction Transaction(NSLOCTEXT("OdysseyPaintEngineTests", "Commit", "Paint stroke"));
        TestTrue(TEXT("Commit the stroke"), Engine.Commit(Blend));
    }
    TestFalse(TEXT("A commit ends the edit"), Engine.GetEdit().IsValid() || IsShown(Layer));
    TestTrue(TEXT("The tiles hold the stroke"), ReadCommitted(Layer, Inside).Equals(Red, Tolerance));
    TestTrue(TEXT("The tiles keep the pixels outside the stroke"), ReadCommitted(Layer, Outside).Equals(Background, Tolerance));
    TestTrue(TEXT("The views show the committed stroke"), ReadShown(Layer, Inside).Equals(Red, Tolerance));
    TestTrue(TEXT("Undo the stroke"), GEditor->UndoTransaction());
    TestTrue(TEXT("Undo restores the tiles"), ReadCommitted(Layer, Inside).Equals(Background, Tolerance));

    // An edit ended by another edit of the layer discards the stroke.
    Engine.Edit(Odyssey::RasterContentBlocks::BeginBlockEdit(Layer->GetRasterContent()));
    PaintStroke(Engine);
    TSharedPtr<FOdysseyRasterBlockEdit> Other = Odyssey::RasterContentBlocks::BeginBlockEdit(Layer->GetRasterContent());
    Engine.Update(Blend);
    TestFalse(TEXT("The paint engine drops an edit ended elsewhere"), Engine.GetEdit().IsValid());
    TestFalse(TEXT("Nothing to commit once the edit ended"), Engine.Commit(Blend));
    if (Other)
    {
        Other->End();
    }
    TestFalse(TEXT("No edit remains"), IsShown(Layer));
    TestTrue(TEXT("The tiles are unchanged"), ReadCommitted(Layer, Inside).Equals(Background, Tolerance));

    // Painting into the edit of another layer, e.g. after the current layer changed mid-shape, discards the pending stroke.
    UOdysseyTextureLayerImageRaster* OtherLayer = CastChecked<UOdysseyTextureLayerImageRaster>(Layer->GetLayerStack()->AddLayer(UOdysseyTextureLayerImageRaster::StaticClass()));
    Engine.Edit(Odyssey::RasterContentBlocks::BeginBlockEdit(Layer->GetRasterContent()));
    PaintStroke(Engine);
    Engine.Update(Blend);
    {
        // A commit would be recorded by this transaction.
        FScopedTransaction Transaction(NSLOCTEXT("OdysseyPaintEngineTests", "SwitchLayer", "Switch layer"));
        Engine.Edit(Odyssey::RasterContentBlocks::BeginBlockEdit(OtherLayer->GetRasterContent()));
    }
    TestTrue(TEXT("The paint engine paints into the edit of the other layer"), Engine.GetEdit().IsValid() && IsShown(OtherLayer));
    TestFalse(TEXT("The edit of the previous layer ended"), IsShown(Layer));
    TestTrue(TEXT("The pending stroke is not committed into the previous layer"), ReadCommitted(Layer, Inside).Equals(Background, Tolerance));
    Engine.Abort();
    TestFalse(TEXT("No edit remains after switching layers"), IsShown(Layer) || IsShown(OtherLayer));
    return true;
}

#endif
