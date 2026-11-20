// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#include "OdysseyPainterEditorRasterSelection.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "Editor.h"
#include "Engine/Texture2D.h"
#include "Misc/AutomationTest.h"
#include "TextureCompiler.h"
#include "UObject/StrongObjectPtr.h"

#include "OdysseyAnimation.h"
#include "OdysseyAnimationCellImageRaster.h"
#include "OdysseyAnimationEditorData.h"
#include "OdysseyAnimationLayerImageRaster.h"
#include "OdysseyAnimationLayerStack.h"
#include "OdysseyAnimationPlayer.h"
#include "OdysseyMediaRaster.h"
#include "OdysseyPainterEditorAnimationSource.h"
#include "OdysseyPainterEditorTextureSource.h"
#include "OdysseyRasterContent.h"
#include "OdysseyRasterContentBlocks.h"
#include "OdysseyTextureLayerImageRaster.h"
#include "OdysseyTextureLayerStack.h"
#include "OdysseyTextureLayerStackUserData.h"

namespace Odyssey::RasterCutPasteTests
{
constexpr EAutomationTestFlags TestFlags = EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter;
constexpr int32 Width = 90;
constexpr int32 Height = 70;
const FColor Background(64, 128, 191, 255);
/** Selected pixels, away from the canvas origin so that the area offsets matter. */
const FIntPoint Inside(20, 20);
/** Unselected pixels, on both sides of the selection. */
const FIntPoint Before(5, 5);
const FIntPoint After(70, 50);

/** Returns a raster layer of a new BGRA8 texture filled with the background color. */
UOdysseyTextureLayerImageRaster* MakeLayer(TStrongObjectPtr<UTexture2D>& OutTexture)
{
    OutTexture.Reset(NewObject<UTexture2D>(GetTransientPackage(), NAME_None, RF_Public | RF_Transactional));
    TArray<FColor> Pixels;
    Pixels.Init(Background, Width * Height);
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

/** Returns the BGRA8 pixel of a block. */
FColor PixelAt(const ::ULIS::FBlock& Block, FIntPoint Position)
{
    const uint8* Pixel = Block.PixelBits(static_cast<uint16>(Position.X), static_cast<uint16>(Position.Y));
    return FColor(Pixel[2], Pixel[1], Pixel[0], Pixel[3]);
}

/** Returns the committed pixel of the content. */
FColor ContentAt(FOdysseyRasterContent& Content, FIntPoint Position)
{
    TSharedPtr<::ULIS::FBlock> Block = Odyssey::RasterContentBlocks::ReadBlock(Content);
    return Block ? PixelAt(*Block, Position) : FColor(0, 0, 0, 0);
}

/** Returns the bounds of the pixels of a BGRA8 block whose alpha is not zero, scanned pixel by pixel. */
FIntRect VisibleBounds(const ::ULIS::FBlock& Block)
{
    FIntRect Bounds(MAX_int32, MAX_int32, MIN_int32, MIN_int32);
    for (int32 Y = 0; Y < Block.Height(); ++Y)
    {
        for (int32 X = 0; X < Block.Width(); ++X)
        {
            if (PixelAt(Block, FIntPoint(X, Y)).A > 0)
            {
                Bounds.Include(FIntPoint(X, Y));
                Bounds.Include(FIntPoint(X + 1, Y + 1));
            }
        }
    }
    return Bounds.Min.X <= Bounds.Max.X ? Bounds : FIntRect();
}

/** Returns the bounds of the selected pixels of the mask, scanned pixel by pixel. */
FIntRect MaskBounds(UOdysseyPainterEditorRasterSelection& Selection)
{
    FIntRect Bounds(MAX_int32, MAX_int32, MIN_int32, MIN_int32);
    TSharedPtr<::ULIS::FBlock> Mask = Selection.GetBlock();
    for (int32 Y = 0; Mask && Y < Mask->Height(); ++Y)
    {
        const float* Line = reinterpret_cast<const float*>(Mask->ScanlineBits(static_cast<uint16>(Y)));
        for (int32 X = 0; X < Mask->Width(); ++X)
        {
            if (Line[X] > 0.0f)
            {
                Bounds.Include(FIntPoint(X, Y));
                Bounds.Include(FIntPoint(X + 1, Y + 1));
            }
        }
    }
    return Bounds.Min.X <= Bounds.Max.X ? Bounds : FIntRect();
}

bool NearlyEqual(const FColor& A, const FColor& B)
{
    return FMath::Abs(A.R - B.R) <= 1 && FMath::Abs(A.G - B.G) <= 1 && FMath::Abs(A.B - B.B) <= 1 && FMath::Abs(A.A - B.A) <= 1;
}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOdysseyRasterCutPasteTest, "Odyssey.PainterEditor.RasterCutPaste", Odyssey::RasterCutPasteTests::TestFlags)
/**
 * Copy, cut and paste of a raster selection read and write the area of the selection only:
 * the copied block covers the canvas with the selected pixels alone, the cut clears them in an undoable way
 * and leaves the other pixels unchanged, the paste restores them, and a transparent paste changes nothing.
 */
bool FOdysseyRasterCutPasteTest::RunTest(const FString& Parameters)
{
    using namespace Odyssey::RasterCutPasteTests;
    TStrongObjectPtr<UTexture2D> Texture;
    UOdysseyTextureLayerImageRaster* Layer = MakeLayer(Texture);
    FOdysseyRasterContent& Content = Layer->GetRasterContent();
    FOdysseyPainterEditorTextureSource Source(Texture.Get());

    TStrongObjectPtr<UOdysseyPainterEditorRasterSelection> Selection(NewObject<UOdysseyPainterEditorRasterSelection>(GetTransientPackage(), NAME_None, RF_Transactional));
    Selection->Init(Width, Height);
    Selection->Add({ FVector2D(10, 10), FVector2D(40, 10), FVector2D(40, 30), FVector2D(10, 30) });

    // Copy
    TSharedPtr<::ULIS::FBlock> Copy = Selection->CopySelectedPixels(Content);
    if (!TestTrue(TEXT("The selected pixels are copied"), Copy.IsValid()))
    {
        return false;
    }
    TestEqual(TEXT("The copy covers the canvas width"), int32(Copy->Width()), Width);
    TestEqual(TEXT("The copy covers the canvas height"), int32(Copy->Height()), Height);
    TestTrue(TEXT("The copy holds the selected pixels"), NearlyEqual(PixelAt(*Copy, Inside), Background));
    TestEqual(TEXT("The copy is transparent before the selection"), int32(PixelAt(*Copy, Before).A), 0);
    TestEqual(TEXT("The copy is transparent after the selection"), int32(PixelAt(*Copy, After).A), 0);
    TestEqual(TEXT("The visible area of the copy holds its visible pixels"), Odyssey::RasterContentBlocks::FindVisibleRect(*Copy).ToString(), VisibleBounds(*Copy).ToString());

    // Over an opaque content, the copy holds every selected pixel, also along the canvas border and after an inversion.
    TestEqual(TEXT("The copy holds every selected pixel"), VisibleBounds(*Copy).ToString(), MaskBounds(*Selection).ToString());
    {
        TStrongObjectPtr<UOdysseyPainterEditorRasterSelection> Border(NewObject<UOdysseyPainterEditorRasterSelection>(GetTransientPackage(), NAME_None, RF_Transactional));
        Border->Init(Width, Height);
        Border->Add({ FVector2D(60, 40), FVector2D(Width + 5, 40), FVector2D(Width + 5, Height + 5), FVector2D(60, Height + 5) });
        TSharedPtr<::ULIS::FBlock> BorderCopy = Border->CopySelectedPixels(Content);
        TestTrue(TEXT("A selection along the canvas border is copied whole"), BorderCopy.IsValid() && VisibleBounds(*BorderCopy) == MaskBounds(*Border));

        Border->Invert();
        TSharedPtr<::ULIS::FBlock> InvertedCopy = Border->CopySelectedPixels(Content);
        TestTrue(TEXT("An inverted selection is copied whole"), InvertedCopy.IsValid() && VisibleBounds(*InvertedCopy) == MaskBounds(*Border));
        TestTrue(TEXT("An inverted selection copies the canvas origin"), InvertedCopy.IsValid() && NearlyEqual(PixelAt(*InvertedCopy, FIntPoint::ZeroValue), Background));
    }

    // Cut
    Source.ClearFromCopyBlock(Copy);
    TestEqual(TEXT("The cut clears the selected pixels"), int32(ContentAt(Content, Inside).A), 0);
    TestTrue(TEXT("The cut keeps the pixels before the selection"), NearlyEqual(ContentAt(Content, Before), Background));
    TestTrue(TEXT("The cut keeps the pixels after the selection"), NearlyEqual(ContentAt(Content, After), Background));
    TestTrue(TEXT("Undo the cut"), GEditor->UndoTransaction());
    TestTrue(TEXT("Undo restores the selected pixels"), NearlyEqual(ContentAt(Content, Inside), Background));
    TestTrue(TEXT("Redo the cut"), GEditor->RedoTransaction());
    TestEqual(TEXT("Redo clears the selected pixels again"), int32(ContentAt(Content, Inside).A), 0);

    // Paste
    Source.PasteBlockToCurrentLayer(Copy);
    TestTrue(TEXT("The paste restores the selected pixels"), NearlyEqual(ContentAt(Content, Inside), Background));
    TestTrue(TEXT("The paste keeps the pixels after the selection"), NearlyEqual(ContentAt(Content, After), Background));

    // A transparent paste has no area to read nor write
    TSharedPtr<::ULIS::FBlock> Transparent = MakeShared<::ULIS::FBlock>(Width, Height, ::ULIS::Format_BGRA8);
    FMemory::Memzero(Transparent->Bits(), Transparent->BytesTotal());
    TestTrue(TEXT("A transparent block has no visible area"), Odyssey::RasterContentBlocks::FindVisibleRect(*Transparent).Area() <= 0);
    Source.PasteBlockToCurrentLayer(Transparent);
    TestTrue(TEXT("A transparent paste changes nothing"), NearlyEqual(ContentAt(Content, Inside), Background));

    // An area partly outside the canvas is clipped
    FIntRect Edited;
    TestTrue(TEXT("An area across the canvas border is edited"), Odyssey::RasterContentBlocks::EditBlock(Content, FIntRect(Width - 10, Height - 10, Width + 10, Height + 10),
        [&Edited](::ULIS::FBlock& Block, const FIntRect& Area)
        {
            Edited = Area;
            FMemory::Memzero(Block.Bits(), Block.BytesTotal());
        }));
    TestEqual(TEXT("The edited area is clipped to the canvas"), Edited.ToString(), FIntRect(Width - 10, Height - 10, Width, Height).ToString());
    TestEqual(TEXT("The clipped area is written"), int32(ContentAt(Content, FIntPoint(Width - 1, Height - 1)).A), 0);
    TestTrue(TEXT("Pixels outside the area are kept"), NearlyEqual(ContentAt(Content, FIntPoint(Width - 11, Height - 11)), Background));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOdysseyAnimationCutPasteTest, "Odyssey.PainterEditor.AnimationCutPaste", Odyssey::RasterCutPasteTests::TestFlags)
/**
 * The animation source pastes a block into a new layer by writing its visible area into the new cell,
 * cuts the pixels of a copied block from the current cell, and pastes them back over it.
 */
bool FOdysseyAnimationCutPasteTest::RunTest(const FString& Parameters)
{
    using namespace Odyssey::RasterCutPasteTests;
    TStrongObjectPtr<UOdysseyAnimation> Animation(NewObject<UOdysseyAnimation>(GetTransientPackage(), NAME_None, RF_Public | RF_Transactional));
    Animation->Init(Width, Height, EOdysseyAnimationFormat::BGRA8, 24.0f);
    UOdysseyAnimationLayerStack* LayerStack = OdysseyAnimationEditor::GetLayerStack(Animation.Get());
    if (!TestNotNull(TEXT("The animation has a layer stack"), LayerStack))
    {
        return false;
    }
    // Pasting into a new layer needs a current raster layer.
    UOdysseyAnimationLayer* FirstLayer = CastChecked<UOdysseyAnimationLayer>(LayerStack->AddLayer(UOdysseyAnimationLayerImageRaster::StaticClass()));
    LayerStack->SetCurrentLayer(FirstLayer);
    FirstLayer->AddCell(UOdysseyAnimationCellImageRaster::StaticClass());
    FOdysseyPainterEditorAnimationSource Source(Animation.Get());
    Source.GetAnimationPlayer()->SetAnimation(Animation.Get());

    // A block holding an opaque red square, away from the canvas origin.
    const FColor Red(255, 0, 0, 255);
    const FIntRect Square(20, 10, 40, 30);
    TSharedPtr<::ULIS::FBlock> Block = MakeShared<::ULIS::FBlock>(Width, Height, ::ULIS::Format_BGRA8);
    FMemory::Memzero(Block->Bits(), Block->BytesTotal());
    for (int32 Y = Square.Min.Y; Y < Square.Max.Y; ++Y)
    {
        for (int32 X = Square.Min.X; X < Square.Max.X; ++X)
        {
            uint8* Pixel = Block->PixelBits(static_cast<uint16>(X), static_cast<uint16>(Y));
            Pixel[0] = Red.B;
            Pixel[1] = Red.G;
            Pixel[2] = Red.R;
            Pixel[3] = Red.A;
        }
    }

    Source.PasteBlockToNewLayer(Block);
    FOdysseyMediaProvider MediaProvider = Source.GetCurrentMediaProvider();
    TArray<TSharedPtr<FOdysseyMediaRaster>> Medias;
    if (MediaProvider.HasMedia<FOdysseyMediaRaster>())
    {
        Medias = MediaProvider.GetOrCreateMedias<FOdysseyMediaRaster>();
    }
    TSharedPtr<FOdysseyRasterContent> Content = Medias.Num() > 0 ? Medias[0]->GetContent() : nullptr;
    if (!TestTrue(TEXT("The paste creates a raster cell in a new layer"), Content.IsValid()))
    {
        return false;
    }
    TestTrue(TEXT("The paste makes the new layer current"), LayerStack->GetCurrentLayer() != FirstLayer);
    TestTrue(TEXT("The new cell holds the pasted pixels"), NearlyEqual(ContentAt(*Content, FIntPoint(30, 20)), Red));
    TestEqual(TEXT("The new cell is transparent elsewhere"), int32(ContentAt(*Content, FIntPoint(5, 5)).A), 0);

    Source.ClearFromCopyBlock(Block);
    TestEqual(TEXT("The cut clears the copied pixels"), int32(ContentAt(*Content, FIntPoint(30, 20)).A), 0);

    Source.PasteBlockToCurrentLayer(Block);
    TestTrue(TEXT("The paste puts the pixels back"), NearlyEqual(ContentAt(*Content, FIntPoint(30, 20)), Red));
    TestEqual(TEXT("The paste leaves the other pixels transparent"), int32(ContentAt(*Content, FIntPoint(70, 50)).A), 0);
    return true;
}

#endif
