// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#include "OdysseyRasterEditSession.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "Editor.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Misc/AutomationTest.h"
#include "RenderGraphBuilder.h"
#include "RenderGraphUtils.h"
#include "RenderingThread.h"
#include "ScopedTransaction.h"
#include "TextureResource.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/UnrealType.h"

#include "OdysseyRenderContext.h"
#include "OdysseyTiledImage.h"
#include "OdysseyTiledImageBlock.h"

namespace Odyssey::RasterEditSessionTests
{
constexpr EAutomationTestFlags TestFlags = EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter;
constexpr int32 TestSize = 80;

/** An image of opaque pixels whose color depends on their position, spanning 2x2 tiles of 64 pixels. */
TStrongObjectPtr<UOdysseyTiledImage> MakeImage()
{
    TStrongObjectPtr<UOdysseyTiledImage> Image(NewObject<UOdysseyTiledImage>(GetTransientPackage(), NAME_None, RF_Transactional));
    Image->Initialize(64, EOdysseyTiledImageFormat::RGBA32F);
    ::ULIS::FBlock Block(TestSize, TestSize, ::ULIS::Format_RGBAF);
    for (int32 Y = 0; Y < TestSize; ++Y)
    {
        for (int32 X = 0; X < TestSize; ++X)
        {
            float* Pixel = reinterpret_cast<float*>(Block.PixelBits(static_cast<uint16>(X), static_cast<uint16>(Y)));
            Pixel[0] = X / float(TestSize);
            Pixel[1] = Y / float(TestSize);
            Pixel[2] = 0.5f;
            Pixel[3] = 1.0f;
        }
    }
    Odyssey::TiledImage::CopyFromBlock(*Image, Block, FIntRect(0, 0, TestSize, TestSize), FIntPoint::ZeroValue);
    return Image;
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

TArray<FLinearColor> ReadImage(const UOdysseyTiledImage& Image)
{
    TStrongObjectPtr<UTextureRenderTarget2D> Target(Image.CreateRenderTarget(FIntPoint(TestSize, TestSize)));
    Image.Render(Target.Get(), FIntRect(0, 0, TestSize, TestSize), FIntPoint::ZeroValue);
    return ReadTarget(Target.Get());
}

/** Renders a content replacement into a new float target. */
TArray<FLinearColor> ReadReplacement(const IOdysseyTextureRenderingAbility::FRenderFunction& Draw)
{
    TStrongObjectPtr<UTextureRenderTarget2D> Target(NewObject<UTextureRenderTarget2D>());
    Target->InitCustomFormat(TestSize, TestSize, PF_A32B32G32R32F, true);
    Target->UpdateResourceImmediate(true);
    ENQUEUE_RENDER_COMMAND(OdysseyRasterEditSessionTestRender)(
        [Target = Target.Get(), Draw](FRHICommandListImmediate& RHICmdList)
        {
            FRDGBuilder Graph(RHICmdList);
            FRDGTextureRef Destination = Target->GetRenderTargetResource()->GetRenderTargetTexture(Graph);
            Draw(Graph, GMaxRHIFeatureLevel, Destination, FIntRect(0, 0, TestSize, TestSize), FIntRect(0, 0, TestSize, TestSize), FMatrix::Identity);
            Graph.Execute();
        });
    return ReadTarget(Target.Get());
}

bool ComparePixels(FAutomationTestBase& Test, const TCHAR* What, const TArray<FLinearColor>& Expected, const TArray<FLinearColor>& Actual)
{
    if (!Test.TestEqual(FString::Printf(TEXT("%s: pixel count"), What), Actual.Num(), Expected.Num()) || Actual.IsEmpty())
    {
        return false;
    }
    for (int32 Index = 0; Index < Expected.Num(); ++Index)
    {
        if (!Actual[Index].Equals(Expected[Index], 0.0001f))
        {
            Test.AddError(FString::Printf(TEXT("%s: pixel (%d, %d) expected %s, got %s"), What, Index % TestSize, Index / TestSize,
                *Expected[Index].ToString(), *Actual[Index].ToString()));
            return false;
        }
    }
    return true;
}

TMap<FIntPoint, FOdysseyTileId> ReadTileIds(const UOdysseyTiledImage& Image)
{
    const FMapProperty* Property = FindFProperty<FMapProperty>(Image.GetClass(), TEXT("Tiles"));
    return *Property->ContainerPtrToValuePtr<TMap<FIntPoint, FOdysseyTileId>>(&Image);
}

/** Clears an area of the session target, as a tool would draw into it. */
void ClearTarget(FOdysseyRasterEditSession& Session, const FIntRect& Rect)
{
    ::ULIS::FBlock Transparent(TestSize, TestSize, ::ULIS::Format_RGBAF);
    FMemory::Memzero(Transparent.Bits(), Transparent.BytesTotal());
    Odyssey::TiledImage::CopyBlockToRenderTarget(*Session.GetTarget(), EOdysseyTiledImageFormat::RGBA32F, Transparent, { Rect });
    Session.Invalidate({ Rect });
}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOdysseyRasterEditSessionPreviewTest, "Odyssey.Imaging.RasterEditSession.Preview", Odyssey::RasterEditSessionTests::TestFlags)
/** A session starts from the tiles, is shown by the views through their context, and discards its edits when it ends. */
bool FOdysseyRasterEditSessionPreviewTest::RunTest(const FString& Parameters)
{
    using namespace Odyssey::RasterEditSessionTests;
    TStrongObjectPtr<UOdysseyTiledImage> Image = MakeImage();
    const TArray<FLinearColor> Original = ReadImage(*Image);
    const TMap<FIntPoint, FOdysseyTileId> OriginalTiles = ReadTileIds(*Image);
    const FGuid Id = FGuid::NewGuid();

    TSharedRef<FOdysseyRenderPreview> RenderPreview = MakeShared<FOdysseyRenderPreview>();
    TArray<FIntRect> PreviewInvalidations;
    RenderPreview->OnInvalidated().AddLambda([&PreviewInvalidations](const TArray<FIntRect>& Rects) { PreviewInvalidations.Append(Rects); });
    FOdysseyRasterEditPreview Preview(RenderPreview);

    TestFalse(TEXT("No session without size"), FOdysseyRasterEditSession::Begin(Id, *Image, FIntPoint::ZeroValue).IsValid());
    TSharedPtr<FOdysseyRasterEditSession> Session = FOdysseyRasterEditSession::Begin(Id, *Image, FIntPoint(TestSize, TestSize));
    if (!TestTrue(TEXT("Begin a session"), Session.IsValid() && Session->IsActive()))
    {
        return false;
    }
    ComparePixels(*this, TEXT("The target starts with the tiles"), Original, ReadTarget(Session->GetTarget()));
    const FOdysseyRenderOverride* Override = RenderPreview->GetContext().Overrides.Find(Id);
    if (!TestTrue(TEXT("The preview replaces the edited content"), Override && Override->ContentReplacement))
    {
        return false;
    }
    TestTrue(TEXT("Beginning a session renders nothing again"), PreviewInvalidations.IsEmpty());

    const FIntRect Erased(10, 10, 20, 20);
    ClearTarget(*Session, Erased);
    TestTrue(TEXT("Edits invalidate the view"), PreviewInvalidations.Contains(Erased));
    const TArray<FLinearColor> Edited = ReadReplacement(RenderPreview->GetContext().Overrides.FindChecked(Id).ContentReplacement);
    TestTrue(TEXT("The replacement shows the edit"), Edited.IsValidIndex(15 * TestSize + 15) && Edited[15 * TestSize + 15].Equals(FLinearColor::Transparent));
    ComparePixels(*this, TEXT("The tiles are not edited"), Original, ReadImage(*Image));

    // Beginning another session of the same content ends the first one.
    TSharedPtr<FOdysseyRasterEditSession> Second = FOdysseyRasterEditSession::Begin(Id, *Image, FIntPoint(TestSize, TestSize));
    TestFalse(TEXT("A content has a single session"), Session->IsActive());
    PreviewInvalidations.Reset();
    ClearTarget(*Second, Erased);
    Second->End();
    TestTrue(TEXT("Ending a session removes its replacement"), RenderPreview->GetContext().Overrides.IsEmpty());
    TestTrue(TEXT("Ending a session renders its edits again"), PreviewInvalidations.Contains(Erased));
    TestTrue(TEXT("Ending a session leaves the tiles"), ReadTileIds(*Image).OrderIndependentCompareEqual(OriginalTiles));
    TestFalse(TEXT("An ended session cannot be committed"), Second->Commit({ Erased }));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOdysseyRasterEditSessionCommitTest, "Odyssey.Imaging.RasterEditSession.Commit", Odyssey::RasterEditSessionTests::TestFlags)
/** A commit writes the touched tiles only, as one undoable change. */
bool FOdysseyRasterEditSessionCommitTest::RunTest(const FString& Parameters)
{
    using namespace Odyssey::RasterEditSessionTests;
    TStrongObjectPtr<UOdysseyTiledImage> Image = MakeImage();
    const TArray<FLinearColor> Original = ReadImage(*Image);
    const TMap<FIntPoint, FOdysseyTileId> Before = ReadTileIds(*Image);
    TSharedPtr<FOdysseyRasterEditSession> Session = FOdysseyRasterEditSession::Begin(FGuid::NewGuid(), *Image, FIntPoint(TestSize, TestSize));
    if (!TestTrue(TEXT("Begin a session"), Session.IsValid()))
    {
        return false;
    }
    const FIntRect Erased(10, 10, 20, 20);
    ClearTarget(*Session, Erased);
    const TArray<FLinearColor> Edited = ReadTarget(Session->GetTarget());
    {
        FScopedTransaction Transaction(NSLOCTEXT("OdysseyRasterEditSessionTests", "Commit", "Commit edit session"));
        TestTrue(TEXT("Commit the session"), Session->Commit({ Erased }));
    }
    TestFalse(TEXT("A commit ends the session"), Session->IsActive());
    ComparePixels(*this, TEXT("The tiles hold the edit"), Edited, ReadImage(*Image));

    const TMap<FIntPoint, FOdysseyTileId> After = ReadTileIds(*Image);
    int32 Changed = 0;
    for (const TPair<FIntPoint, FOdysseyTileId>& Pair : Before)
    {
        Changed += !(After.FindChecked(Pair.Key) == Pair.Value);
    }
    TestEqual(TEXT("The commit replaces the touched tile only"), Changed, 1);

    TestTrue(TEXT("Undo the commit"), GEditor->UndoTransaction());
    TestTrue(TEXT("Undo restores the tiles"), ReadTileIds(*Image).OrderIndependentCompareEqual(Before));
    ComparePixels(*this, TEXT("Undone commit"), Original, ReadImage(*Image));
    return true;
}

#endif
