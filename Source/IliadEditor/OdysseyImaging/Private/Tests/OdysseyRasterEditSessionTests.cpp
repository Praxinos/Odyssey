// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#include "OdysseyRasterEditSession.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "Editor.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Math/TranslationMatrix.h"
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

/** Renders an area of a content replacement into a new float target of its destination size. */
TArray<FLinearColor> ReadReplacement(const IOdysseyTextureRenderingAbility::FRenderFunction& Draw, const FIntRect& SourceRect,
    const FIntRect& DestinationRect, const FMatrix& Transform)
{
    TStrongObjectPtr<UTextureRenderTarget2D> Target(NewObject<UTextureRenderTarget2D>());
    Target->InitCustomFormat(DestinationRect.Max.X, DestinationRect.Max.Y, PF_A32B32G32R32F, true);
    Target->UpdateResourceImmediate(true);
    ENQUEUE_RENDER_COMMAND(OdysseyRasterEditSessionTestRenderArea)(
        [Target = Target.Get(), Draw, SourceRect, DestinationRect, Transform](FRHICommandListImmediate& RHICmdList)
        {
            FRDGBuilder Graph(RHICmdList);
            FRDGTextureRef Destination = Target->GetRenderTargetResource()->GetRenderTargetTexture(Graph);
            Draw(Graph, GMaxRHIFeatureLevel, Destination, SourceRect, DestinationRect, Transform);
            Graph.Execute();
        });
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

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOdysseyRasterEditSessionPreserveAlphaTest, "Odyssey.Imaging.RasterEditSession.PreserveAlpha", Odyssey::RasterEditSessionTests::TestFlags)
/** A session preserving alpha shows and commits the edited colors with the alpha the tiles had at its beginning. */
bool FOdysseyRasterEditSessionPreserveAlphaTest::RunTest(const FString& Parameters)
{
    using namespace Odyssey::RasterEditSessionTests;
    TStrongObjectPtr<UOdysseyTiledImage> Image = MakeImage();
    // A transparent area, which an edit preserving alpha cannot paint.
    const FIntRect TransparentRect(50, 50, 60, 60);
    ::ULIS::FBlock Transparent(TestSize, TestSize, ::ULIS::Format_RGBAF);
    FMemory::Memzero(Transparent.Bits(), Transparent.BytesTotal());
    Odyssey::TiledImage::CopyFromBlock(*Image, Transparent, TransparentRect, TransparentRect.Min);
    const TArray<FLinearColor> Original = ReadImage(*Image);

    const FGuid Id = FGuid::NewGuid();
    TSharedPtr<FOdysseyRasterEditSession> Session = FOdysseyRasterEditSession::Begin(Id, *Image, FIntPoint(TestSize, TestSize), true);
    if (!TestTrue(TEXT("Begin a session"), Session.IsValid()))
    {
        return false;
    }
    TestTrue(TEXT("The session preserves alpha"), Session->PreservesAlpha());

    // Erase an opaque area, and paint an opaque red over the transparent one.
    const FIntRect Erased(10, 10, 20, 20);
    ClearTarget(*Session, Erased);
    ::ULIS::FBlock Red(TestSize, TestSize, ::ULIS::Format_RGBAF);
    for (int32 Index = 0; Index < TestSize * TestSize; ++Index)
    {
        float* Pixel = reinterpret_cast<float*>(Red.Bits()) + Index * 4;
        Pixel[0] = 1.0f;
        Pixel[1] = 0.0f;
        Pixel[2] = 0.0f;
        Pixel[3] = 1.0f;
    }
    Odyssey::TiledImage::CopyBlockToRenderTarget(*Session->GetTarget(), EOdysseyTiledImageFormat::RGBA32F, Red, { TransparentRect });
    Session->Invalidate({ TransparentRect });

    FOdysseyRenderContext Context;
    FOdysseyRasterEditSession::AddOverrides(Context);
    const FOdysseyRenderOverride* Override = Context.Overrides.Find(Id);
    if (!TestTrue(TEXT("The session replaces the content"), Override && Override->ContentReplacement))
    {
        return false;
    }
    TArray<FLinearColor> Expected = Original;
    for (int32 Y = Erased.Min.Y; Y < Erased.Max.Y; ++Y)
    {
        for (int32 X = Erased.Min.X; X < Erased.Max.X; ++X)
        {
            // The erased colors with the opaque alpha of the tiles.
            Expected[Y * TestSize + X] = FLinearColor(0.0f, 0.0f, 0.0f, 1.0f);
        }
    }
    for (int32 Y = TransparentRect.Min.Y; Y < TransparentRect.Max.Y; ++Y)
    {
        for (int32 X = TransparentRect.Min.X; X < TransparentRect.Max.X; ++X)
        {
            // The painted colors with the transparent alpha of the tiles.
            Expected[Y * TestSize + X] = FLinearColor(1.0f, 0.0f, 0.0f, 0.0f);
        }
    }
    // The views blend the replacement: transparent pixels show no color.
    const TArray<FLinearColor> Shown = ReadReplacement(Override->ContentReplacement);
    if (TestEqual(TEXT("Shown pixel count"), Shown.Num(), TestSize * TestSize))
    {
        TestTrue(TEXT("The views show the erased colors with the original alpha"), Shown[15 * TestSize + 15].Equals(FLinearColor(0.0f, 0.0f, 0.0f, 1.0f), 0.0001f));
        TestEqual(TEXT("The views show no painted pixel where the tiles were transparent"), Shown[55 * TestSize + 55].A, 0.0f);
        TestTrue(TEXT("The views show the other pixels unchanged"), Shown[40 * TestSize + 40].Equals(Original[40 * TestSize + 40], 0.0001f));
    }

    {
        FScopedTransaction Transaction(NSLOCTEXT("OdysseyRasterEditSessionTests", "CommitPreservingAlpha", "Commit edit session preserving alpha"));
        TestTrue(TEXT("Commit the session"), Session->Commit({ Erased, TransparentRect }));
    }
    ComparePixels(*this, TEXT("The tiles hold the colors with the original alpha"), Expected, ReadImage(*Image));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOdysseyRasterEditSessionPreserveAlphaAreaTest, "Odyssey.Imaging.RasterEditSession.PreserveAlphaPartialRender", Odyssey::RasterEditSessionTests::TestFlags)
/**
 * A session preserving alpha composes the colors of its target with the original alpha over the area a render samples only.
 * Renders of a part of the content, and through a transform, e.g. a cell out of pegs, show the composed pixels at their place.
 */
bool FOdysseyRasterEditSessionPreserveAlphaAreaTest::RunTest(const FString& Parameters)
{
    using namespace Odyssey::RasterEditSessionTests;
    TStrongObjectPtr<UOdysseyTiledImage> Image = MakeImage();
    const TArray<FLinearColor> Original = ReadImage(*Image);
    const FGuid Id = FGuid::NewGuid();
    TSharedPtr<FOdysseyRasterEditSession> Session = FOdysseyRasterEditSession::Begin(Id, *Image, FIntPoint(TestSize, TestSize), true);
    if (!TestTrue(TEXT("Begin a session preserving alpha"), Session.IsValid() && Session->PreservesAlpha()))
    {
        return false;
    }
    // Erased pixels keep the opaque alpha of the tiles: they are shown black.
    const FIntRect Erased(10, 10, 20, 20);
    ClearTarget(*Session, Erased);
    FOdysseyRenderContext Context;
    FOdysseyRasterEditSession::AddOverrides(Context);
    const FOdysseyRenderOverride* Override = Context.Overrides.Find(Id);
    if (!TestTrue(TEXT("The session replaces the content"), Override && Override->ContentReplacement))
    {
        return false;
    }

    struct FCase
    {
        const TCHAR* Name;
        FIntRect SourceRect;
        FMatrix Transform;
        /** The content pixel shown at a destination pixel. */
        FIntPoint Offset;
    };
    const FIntRect DestinationRect(0, 0, 40, 40);
    const FCase Cases[] = {
        { TEXT("part of the content"), FIntRect(5, 5, 45, 45), FMatrix::Identity, FIntPoint(5, 5) },
        { TEXT("translated content"), FIntRect(0, 0, 40, 40), FTranslationMatrix(FVector(-3.0, -4.0, 0.0)), FIntPoint(3, 4) },
    };
    for (const FCase& Case : Cases)
    {
        const TArray<FLinearColor> Shown = ReadReplacement(Override->ContentReplacement, Case.SourceRect, DestinationRect, Case.Transform);
        if (!TestEqual(FString::Printf(TEXT("Render of a %s: pixel count"), Case.Name), Shown.Num(), 40 * 40))
        {
            continue;
        }
        for (const FIntPoint& Pixel : { FIntPoint(1, 1), FIntPoint(10, 10), FIntPoint(30, 30), FIntPoint(38, 38) })
        {
            const FIntPoint ContentPixel = Pixel + Case.Offset;
            const FLinearColor Expected = Erased.Contains(ContentPixel) ? FLinearColor(0.0f, 0.0f, 0.0f, 1.0f)
                : Original[ContentPixel.Y * TestSize + ContentPixel.X];
            const FLinearColor& Actual = Shown[Pixel.Y * 40 + Pixel.X];
            TestTrue(FString::Printf(TEXT("Render of a %s shows content pixel (%d, %d): expected %s, got %s"), Case.Name,
                ContentPixel.X, ContentPixel.Y, *Expected.ToString(), *Actual.ToString()), Actual.Equals(Expected, 0.0001f));
        }
    }
    Session->End();
    return true;
}

#endif
