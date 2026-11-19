// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#if WITH_DEV_AUTOMATION_TESTS
#include "Editor.h"
#include "EditorValidatorSubsystem.h"
#include "Engine/Texture2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "HAL/FileManager.h"
#include "Misc/AutomationTest.h"
#include "Misc/CommandLine.h"
#include "Misc/PackageName.h"
#include "Misc/Parse.h"
#include "Misc/Paths.h"
#include "Misc/ScopeExit.h"
#include "RenderingThread.h"
#include "ScopedTransaction.h"
#include "TextureResource.h"
#include "UObject/Package.h"
#include "UObject/SavePackage.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/UnrealType.h"

#include "OdysseyAnimation.h"
#include "OdysseyAnimationCellImageRaster.h"
#include "OdysseyAnimationLayerImageRaster.h"
#include "OdysseyAnimationRenderContext.h"
#include "OdysseyLayerStack.h"
#include "OdysseyLighttable.h"
#include "OdysseyRasterBlock.h"
#include "OdysseyRasterBlockMutator.h"
#include "OdysseyPixelFormat.h"
#include "OdysseySurfaceTexture2DEditable.h"
#include "OdysseyTiledImage.h"
#include "OdysseyAnimationLayerStackUserData.h"
#include "OdysseyAnimationEditorData.h"

namespace Odyssey::AnimationTiledTests
{
constexpr int32 Width = 73;
constexpr int32 Height = 67;
constexpr EAutomationTestFlags Flags = EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter;

UOdysseyAnimationCellImageRaster* Cell(UOdysseyAnimation* Animation, int32 Index)
{
    return CastChecked<UOdysseyAnimationCellImageRaster>(CastChecked<UOdysseyAnimationLayer>(OdysseyAnimationEditor::GetLayerStack(Animation)->GetRootLayers()[0])->GetCells()[Index]);
}

void Fill(UOdysseyAnimationCellImageRaster* Target, uint8 Seed)
{
    auto Raster = Target->GetRasterBlock();
    FOdysseyRasterBlockMutator Mutator(Raster, false);
    Mutator.EditTilesFromRects({Raster->GetRect()}, [Seed](TSharedPtr<ULIS::FBlock> Block, const FOdysseyInvalidTileMap&) -> TArray<ULIS::FEvent>
    {
        // Both animation formats have four channels; write through ULIS for the float variant too.
        for (int32 Y = 0; Y < Height; ++Y)
        {
            for (int32 X = 0; X < Width; ++X)
            {
                ULIS::FColor Color(ULIS::Format_RGBA8, {Seed, uint8((X * 3 + Y * 5) % 251), uint8(255 - Seed), uint8(255)});
                Block->SetPixel(X, Y, Color);
            }
        }
        return {};
    });
    Mutator.Commit();
}

UOdysseyAnimation* MakeAnimation(UObject* Outer, EOdysseyAnimationFormat Format)
{
    UOdysseyAnimation* Animation = NewObject<UOdysseyAnimation>(Outer, NAME_None, RF_Public | RF_Transactional);
    Animation->Init(Width, Height, Format, 24.0f);
    UOdysseyAnimationLayerStack* LayerStack = OdysseyAnimationEditor::GetLayerStack(Animation);
    UOdysseyAnimationLayer* Layer = CastChecked<UOdysseyAnimationLayer>(LayerStack->AddLayer(UOdysseyAnimationLayerImageRaster::StaticClass()));
    LayerStack->SetCurrentLayer(Layer);
    UOdysseyAnimationCellImageRaster* First = CastChecked<UOdysseyAnimationCellImageRaster>(Layer->AddCell(UOdysseyAnimationCellImageRaster::StaticClass()));
    UOdysseyAnimationCellImageRaster* Second = CastChecked<UOdysseyAnimationCellImageRaster>(Layer->AddCell(UOdysseyAnimationCellImageRaster::StaticClass()));
    First->SetExposure(2);
    Second->SetExposure(3);
    Fill(First, 47);
    Fill(Second, 191);
    return Animation;
}

TArray<FLinearColor> Render(UOdysseyAnimation* Animation, int32 Frame, uint64 Type)
{
    TStrongObjectPtr<UTextureRenderTarget2D> Target(NewObject<UTextureRenderTarget2D>());
    Target->InitCustomFormat(Width, Height, PF_A32B32G32R32F, true);
    Target->UpdateResourceImmediate(true);
    FOdysseyRenderContext RenderContext;
    RenderContext.FindOrAddExtension<FOdysseyAnimationRenderContext>().Frame = FFrameNumber(Frame);
    Animation->Render_GameThread(Target.Get(), Type, &RenderContext);
    FlushRenderingCommands();
    TArray<FLinearColor> Pixels;
    FReadSurfaceDataFlags ReadFlags(RCM_MinMax);
    ReadFlags.SetLinearToGamma(false);
    Target->GameThread_GetRenderTargetResource()->ReadLinearColorPixels(Pixels, ReadFlags);
    return Pixels;
}

bool ComparePixels(FAutomationTestBase& Test, const FString& What, const TArray<FLinearColor>& Expected, const TArray<FLinearColor>& Actual, float Tolerance)
{
    if (!Test.TestEqual(What + TEXT(": rendered pixel count"), Actual.Num(), Width * Height)
        || !Test.TestEqual(What + TEXT(": reference pixel count"), Expected.Num(), Actual.Num()))
    {
        return false;
    }
    float MaxDifference = 0.0f;
    for (int32 Index = 0; Index < Actual.Num(); ++Index)
    {
        const FLinearColor Difference = Actual[Index] - Expected[Index];
        MaxDifference = FMath::Max(MaxDifference, FMath::Max(
            FMath::Max(FMath::Abs(Difference.R), FMath::Abs(Difference.G)),
            FMath::Max(FMath::Abs(Difference.B), FMath::Abs(Difference.A))));
        if (!Actual[Index].Equals(Expected[Index], Tolerance))
        {
            Test.AddError(FString::Printf(TEXT("%s: pixel %d differs: %s vs %s"), *What, Index,
                *Expected[Index].ToString(), *Actual[Index].ToString()));
            return false;
        }
    }
    Test.AddInfo(FString::Printf(TEXT("%s: maximum component difference %.7f"), *What, MaxDifference));
    return true;
}

/**
 * Compares colors as displayed by an 8-bit sRGB output, allowing one level of difference.
 * The former render texture was decoded by the GPU, up to 0.002 away from the exact decoding of the tiles:
 * filtered dark tones may differ by a few more levels within this linear difference.
 */
bool CompareDisplayedPixels(FAutomationTestBase& Test, const FString& What, const TArray<FLinearColor>& Expected, const TArray<FLinearColor>& Actual)
{
    if (!Test.TestEqual(What + TEXT(": rendered pixel count"), Actual.Num(), Width * Height)
        || !Test.TestEqual(What + TEXT(": reference pixel count"), Expected.Num(), Actual.Num()))
    {
        return false;
    }
    int32 MaxDifference = 0;
    for (int32 Index = 0; Index < Actual.Num(); ++Index)
    {
        const FColor ExpectedColor = Expected[Index].ToFColorSRGB();
        const FColor ActualColor = Actual[Index].ToFColorSRGB();
        const int32 Difference = FMath::Max(FMath::Max(FMath::Abs(ExpectedColor.R - ActualColor.R), FMath::Abs(ExpectedColor.G - ActualColor.G)),
            FMath::Max(FMath::Abs(ExpectedColor.B - ActualColor.B), FMath::Abs(ExpectedColor.A - ActualColor.A)));
        MaxDifference = FMath::Max(MaxDifference, Difference);
        if (Difference > 1 && !Actual[Index].Equals(Expected[Index], 0.002f))
        {
            Test.AddError(FString::Printf(TEXT("%s: pixel %d differs: %s vs %s"), *What, Index,
                *ExpectedColor.ToString(), *ActualColor.ToString()));
            return false;
        }
    }
    Test.AddInfo(FString::Printf(TEXT("%s: maximum 8-bit sRGB difference %d"), *What, MaxDifference));
    return true;
}

TArray<UOdysseyAnimationCellImageRaster*> RasterCells(UOdysseyAnimation* Animation)
{
    TArray<UOdysseyAnimationCellImageRaster*> Cells;
    for (UOdysseyLayer* Layer : OdysseyAnimationEditor::GetLayerStack(Animation)->GetRootLayers())
    {
        for (UOdysseyAnimationCell* AnimationCell : CastChecked<UOdysseyAnimationLayer>(Layer)->GetCells())
        {
            if (UOdysseyAnimationCellImageRaster* RasterCell = Cast<UOdysseyAnimationCellImageRaster>(AnimationCell))
            {
                Cells.Add(RasterCell);
            }
        }
    }
    return Cells;
}

/**
 * Renders a frame while every cell displays uncommitted edits changing no pixel, as the former render texture did,
 * then once these edits are committed into the tiles. Both renders must match:
 * float cells exactly, 8-bit cells as displayed.
 */
bool Compare(FAutomationTestBase& Test, UOdysseyAnimation* Animation, int32 Frame, uint64 Type = EOdysseyRenderingType::Render)
{
    TArray<TUniquePtr<FOdysseyRasterBlockMutator>> Mutators;
    for (UOdysseyAnimationCellImageRaster* RasterCell : RasterCells(Animation))
    {
        TSharedPtr<FOdysseyRasterBlock> Raster = RasterCell->GetRasterBlock();
        TUniquePtr<FOdysseyRasterBlockMutator>& Mutator = Mutators.Add_GetRef(MakeUnique<FOdysseyRasterBlockMutator>(Raster, false));
        Mutator->EditTilesFromRects({Raster->GetRect()},
            [](TSharedPtr<ULIS::FBlock>, const FOdysseyInvalidTileMap&) -> TArray<ULIS::FEvent>
            {
                return {};
            });
    }
    const TArray<FLinearColor> Preview = Render(Animation, Frame, Type);
    for (TUniquePtr<FOdysseyRasterBlockMutator>& Mutator : Mutators)
    {
        Mutator->Commit();
    }
    const TArray<FLinearColor> Tiles = Render(Animation, Frame, Type);
    const FString What = FString::Printf(TEXT("Frame %d render type %llu"), Frame, Type);
    if (Animation->GetFormat() == EOdysseyAnimationFormat::BGRA8)
    {
        return CompareDisplayedPixels(Test, What, Preview, Tiles);
    }
    return ComparePixels(Test, What, Preview, Tiles, 0.0001f);
}

TMap<FIntPoint, FOdysseyTileId> TileIds(const UOdysseyTiledImage* Image)
{
    return *FindFProperty<FMapProperty>(Image->GetClass(), TEXT("Tiles"))
        ->ContainerPtrToValuePtr<TMap<FIntPoint, FOdysseyTileId>>(Image);
}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOdysseyAnimationTiledIntegrationTest, "Odyssey.Animation.TiledImage.Integration", Odyssey::AnimationTiledTests::Flags)
bool FOdysseyAnimationTiledIntegrationTest::RunTest(const FString& Parameters)
{
    using namespace Odyssey::AnimationTiledTests;
    TStrongObjectPtr<UOdysseyAnimationCellImageRaster> Detached(NewObject<UOdysseyAnimationCellImageRaster>());
    TestNull(TEXT("Detached cell has no image"), Detached->GetTiledImage());
    TestFalse(TEXT("Detached cell has no raster block"), Detached->GetRasterBlock().IsValid());
    for (auto Format : {EOdysseyAnimationFormat::BGRA8, EOdysseyAnimationFormat::RGBAF})
    {
        TStrongObjectPtr<UOdysseyAnimation> Animation(MakeAnimation(GetTransientPackage(), Format));
        auto First = Cell(Animation.Get(), 0);
        auto Second = Cell(Animation.Get(), 1);
        if (!TestNotNull(TEXT("Cell stores its pixels in a tiled image"), First->GetTiledImage()))
        {
            return false;
        }
        TestEqual(TEXT("Image belongs to the cell"), First->GetTiledImage()->GetOuter(), static_cast<UObject*>(First));
        TestNull(TEXT("Raster cell has no render texture"), First->GetRenderTexture());
        TestEqual(TEXT("Tiles hold linear colors with enough precision"), First->GetTiledImage()->GetFormat(),
            Format == EOdysseyAnimationFormat::BGRA8 ? EOdysseyTiledImageFormat::RGBA16F : EOdysseyTiledImageFormat::RGBA32F);
        for (int32 Frame = 0; Frame < 5; ++Frame) { Compare(*this, Animation.Get(), Frame); }
        TestEqual(TEXT("Cell exposures form five frames"), Animation->GetFrameCount(), 5);
        UOdysseyAnimationLayer* Layer = CastChecked<UOdysseyAnimationLayer>(OdysseyAnimationEditor::GetLayerStack(Animation.Get())->GetRootLayers()[0]);
        FOdysseyLighttable Lighttable;
        Lighttable.bIsActivated = true;
        Lighttable.PreviousKeys[0].bIsActivated = true;
        Lighttable.PreviousKeys[0].Opacity = 40.0f;
        Lighttable.NextKeys[0].bIsActivated = true;
        Lighttable.NextKeys[0].Opacity = 30.0f;
        Layer->SetLighttable(Lighttable);
        FOdysseyAnimationCellOutOfPegs Pegs;
        Pegs.Pan = FVector2D(3, -2);
        Pegs.Rotation = 17;
        Pegs.Zoom = 125;
        First->SetOutOfPegs(Pegs);
        Compare(*this, Animation.Get(), 0, EOdysseyRenderingType::Editor | EOdysseyRenderingType::OutOfPegs);
        Compare(*this, Animation.Get(), 2, EOdysseyRenderingType::Editor | EOdysseyRenderingType::OutOfPegs);
        const TMap<FIntPoint, FOdysseyTileId> OtherTiles = TileIds(Second->GetTiledImage());
        const TMap<FIntPoint, FOdysseyTileId> FirstTiles = TileIds(First->GetTiledImage());
        const TArray<FLinearColor> BeforeClear = Render(Animation.Get(), 0, EOdysseyRenderingType::Render);
        First->Clear();
        TestTrue(TEXT("Other cell stays unchanged"), TileIds(Second->GetTiledImage()).OrderIndependentCompareEqual(OtherTiles));
        Compare(*this, Animation.Get(), 0);
        TestTrue(TEXT("Undo clear"), GEditor->UndoTransaction());
        TestTrue(TEXT("Undo restores the tiles"), TileIds(First->GetTiledImage()).OrderIndependentCompareEqual(FirstTiles));
        ComparePixels(*this, TEXT("Undone clear"), BeforeClear, Render(Animation.Get(), 0, EOdysseyRenderingType::Render), 0.0001f);
        Compare(*this, Animation.Get(), 0);
        TestTrue(TEXT("Redo clear"), GEditor->RedoTransaction());
        Compare(*this, Animation.Get(), 0);
        TStrongObjectPtr<UOdysseyAnimation> Duplicate(DuplicateObject<UOdysseyAnimation>(Animation.Get(), GetTransientPackage()));
        Compare(*this, Duplicate.Get(), 2);
        // The duplicate has no raster block yet: it reads it from its tiles.
        TSharedPtr<ULIS::FBlock> OriginalBlock = Second->GetRasterBlock()->GetBlock();
        TSharedPtr<ULIS::FBlock> ReadBlock = Cell(Duplicate.Get(), 1)->GetRasterBlock()->GetBlock();
        TestTrue(TEXT("Raster block read from the tiles keeps every value"), OriginalBlock->BytesTotal() == ReadBlock->BytesTotal()
            && FMemory::Memcmp(OriginalBlock->Bits(), ReadBlock->Bits(), OriginalBlock->BytesTotal()) == 0);
        TestTrue(TEXT("Duplicated cell owns its image"), Cell(Duplicate.Get(), 1)->GetTiledImage() != Second->GetTiledImage());
    }
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOdysseyAnimationTiledPartialTest, "Odyssey.Animation.TiledImage.PartialUpdates", Odyssey::AnimationTiledTests::Flags)
/** Uncommitted edits are rendered without touching the tiles; a commit replaces only the touched tiles. */
bool FOdysseyAnimationTiledPartialTest::RunTest(const FString& Parameters)
{
    using namespace Odyssey::AnimationTiledTests;
    TStrongObjectPtr<UOdysseyAnimation> Animation(MakeAnimation(GetTransientPackage(), EOdysseyAnimationFormat::BGRA8));
    UOdysseyAnimationCellImageRaster* First = Cell(Animation.Get(), 0);
    const TMap<FIntPoint, FOdysseyTileId> Before = TileIds(First->GetTiledImage());
    const TArray<FLinearColor> BeforePixels = Render(Animation.Get(), 0, EOdysseyRenderingType::Render);
    FOdysseyRasterBlockMutator Mutator(First->GetRasterBlock(), false);
    Mutator.EditTilesFromRects({ULIS::FRectI::FromXYWH(2, 3, 4, 5)},
        [](TSharedPtr<ULIS::FBlock> Block, const FOdysseyInvalidTileMap&) -> TArray<ULIS::FEvent>
        {
            for (int32 Y = 3; Y < 8; ++Y) { FMemory::Memzero(Block->Bits() + (Y * Width + 2) * 4, 16); }
            return {};
        });
    TestTrue(TEXT("Uncommitted edits leave the tiles unchanged"), TileIds(First->GetTiledImage()).OrderIndependentCompareEqual(Before));
    const TArray<FLinearColor> Interactive = Render(Animation.Get(), 0, EOdysseyRenderingType::Render);
    if (TestEqual(TEXT("Interactive pixel count"), Interactive.Num(), Width * Height)
        && TestEqual(TEXT("Reference pixel count"), BeforePixels.Num(), Width * Height))
    {
        TestTrue(TEXT("Uncommitted edits are rendered"), Interactive[4 * Width + 3].Equals(FLinearColor::Transparent));
        const FColor Kept = Interactive[40 * Width + 40].ToFColorSRGB();
        const FColor Original = BeforePixels[40 * Width + 40].ToFColorSRGB();
        TestTrue(TEXT("Pixels outside the edit are kept"), FMath::Abs(Kept.R - Original.R) <= 1 && FMath::Abs(Kept.G - Original.G) <= 1
            && FMath::Abs(Kept.B - Original.B) <= 1 && Kept.A == Original.A);
    }
    Mutator.Commit();
    const TMap<FIntPoint, FOdysseyTileId> After = TileIds(First->GetTiledImage());
    int32 Changed = 0;
    for (const TPair<FIntPoint, FOdysseyTileId>& Pair : Before) { Changed += !(After.FindChecked(Pair.Key) == Pair.Value); }
    TestEqual(TEXT("Commit replaces only the touched tile"), Changed, 1);
    // The edit is rendered from the raster block before the commit, and from the tiles after.
    CompareDisplayedPixels(*this, TEXT("Committed tiles render the edit"), Interactive, Render(Animation.Get(), 0, EOdysseyRenderingType::Render));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOdysseyAnimationLegacyMigrationTest, "Odyssey.Animation.TiledImage.LegacyMigration", Odyssey::AnimationTiledTests::Flags)
/** Cells saved before the tiled image storage hold their pixels in a render texture, moved into the tiled image after loading. */
bool FOdysseyAnimationLegacyMigrationTest::RunTest(const FString& Parameters)
{
    using namespace Odyssey::AnimationTiledTests;
    FObjectProperty* TextureProperty = FindFProperty<FObjectProperty>(UOdysseyAnimationCell::StaticClass(), TEXT("Texture"));
    if (!TestNotNull(TEXT("Render texture property"), TextureProperty))
    {
        return false;
    }
    for (EOdysseyAnimationFormat Format : {EOdysseyAnimationFormat::BGRA8, EOdysseyAnimationFormat::RGBAF})
    {
        TStrongObjectPtr<UOdysseyAnimation> Animation(MakeAnimation(GetTransientPackage(), Format));
        UOdysseyAnimationCellImageRaster* First = Cell(Animation.Get(), 0);
        const TArray<FLinearColor> Reference = Render(Animation.Get(), 0, EOdysseyRenderingType::Render);

        // Recreate the state of a loaded legacy cell: its pixels are only in its render texture, set up as the former one.
        TSharedPtr<FOdysseyRasterBlock> Raster = First->GetRasterBlock();
        UTexture2D* LegacyTexture = NewObject<UTexture2D>(First);
        LegacyTexture->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
        LegacyTexture->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
        InitTextureWithBlockData(Raster->GetBlock().Get(), LegacyTexture, TextureSourceFormatForULISFormat(Raster->GetFormat()));
        First->Clear();
        TextureProperty->SetObjectPropertyValue_InContainer(First, LegacyTexture);
        First->PostLoad();

        TestNull(TEXT("Migration discards the render texture"), First->GetRenderTexture());
        ComparePixels(*this, TEXT("Migrated cell"), Reference, Render(Animation.Get(), 0, EOdysseyRenderingType::Render), 0.0001f);
        Compare(*this, Animation.Get(), 0);
    }
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOdysseyAnimationBoundsTest, "Odyssey.Animation.Bounds.Editor", Odyssey::AnimationTiledTests::Flags)
bool FOdysseyAnimationBoundsTest::RunTest(const FString& Parameters)
{
    using namespace Odyssey::AnimationTiledTests;
    FScopedDisableValidateOnSave DisableValidation;
    const FString Id = FGuid::NewGuid().ToString(EGuidFormats::Digits);
    const FString Mount = TEXT("/OdysseyAnimationBoundsTest/");
    const FString Directory = FPaths::ConvertRelativePathToFull(FPaths::ProjectPluginsDir() / TEXT("Odyssey-Plugin/Intermediate/OdysseyAnimationTests/"));
    FPackageName::RegisterMountPoint(Mount, Directory + TEXT("/"));
    ON_SCOPE_EXIT { FPackageName::UnRegisterMountPoint(Mount, Directory + TEXT("/")); };
    UPackage* Package = CreatePackage(*(Mount + Id));
    TStrongObjectPtr<UOdysseyAnimation> Animation(MakeAnimation(Package, EOdysseyAnimationFormat::BGRA8));
    TestTrue(TEXT("Automatic bounds follow cell exposures"), Animation->GetFrameRange() == FInt32Range::Inclusive(0, 4));
    for (const FName PropertyName : {FName(TEXT("LeftBoundMode")), FName(TEXT("RightBoundMode")), FName(TEXT("LeftBound")), FName(TEXT("RightBound"))})
    {
        const FProperty* Property = FindFProperty<FProperty>(Animation->GetClass(), PropertyName);
        if (TestNotNull(TEXT("Editor bounds property exists"), Property))
        {
            TestTrue(TEXT("Bounds property is stripped when cooking"), Property->HasAnyPropertyFlags(CPF_EditorOnly));
        }
    }
    Animation->SetLeftBoundMode(EOdysseyAnimationBoundMode::Manual);
    Animation->SetRightBoundMode(EOdysseyAnimationBoundMode::Manual);
    Animation->SetLeftBoundValue(99);
    TestEqual(TEXT("Left bound is clamped to the right bound"), Animation->GetLeftBoundValue(), 4);
    Animation->SetLeftBoundValue(-1);
    TestEqual(TEXT("Left bound is clamped to zero"), Animation->GetLeftBoundValue(), 0);
    Animation->SetLeftBoundValue(1);
    Animation->SetRightBoundValue(3);
    TestTrue(TEXT("Manual bounds trim the animation"), Animation->GetFrameRange() == FInt32Range::Inclusive(1, 3));
    const FString Filename = Directory / (Id + TEXT(".uasset"));
    IFileManager::Get().MakeDirectory(*Directory, true);
    ON_SCOPE_EXIT { IFileManager::Get().Delete(*Filename); };
    FSavePackageArgs Args;
    Args.TopLevelFlags = RF_Public;
    Args.SaveFlags = SAVE_NoError;
    if (!TestTrue(TEXT("Save animation with editor bounds"), UPackage::SavePackage(Package, Animation.Get(), *Filename, Args))) { return false; }
    const FArrayProperty* FramesProperty = FindFProperty<FArrayProperty>(Animation->GetClass(), TEXT("Frames"));
    if (!TestNotNull(TEXT("Baked frames property"), FramesProperty)) { return false; }
    const TArray<FOdysseyAnimationFrame>& BakedFrames = *FramesProperty->ContainerPtrToValuePtr<TArray<FOdysseyAnimationFrame>>(Animation.Get());
    if (!TestEqual(TEXT("Trimmed animation bakes both images"), BakedFrames.Num(), 2)) { return false; }
    TestEqual(TEXT("First exposure is trimmed by the left bound"), BakedFrames[0].Exposure, 1);
    TestEqual(TEXT("Second exposure is trimmed by the right bound"), BakedFrames[1].Exposure, 2);
    TestNotNull(TEXT("First trimmed image is baked"), BakedFrames[0].Texture);
    TestNotNull(TEXT("Second trimmed image is baked"), BakedFrames[1].Texture);
    Animation->SetLeftBoundMode(EOdysseyAnimationBoundMode::Automatic);
    Animation->SetRightBoundMode(EOdysseyAnimationBoundMode::Automatic);
    TestTrue(TEXT("Automatic bounds restore the complete animation"), Animation->GetFrameRange() == FInt32Range::Inclusive(0, 4));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOdysseyAnimationTiledPersistenceTest, "Odyssey.Animation.TiledImage.Persistence", Odyssey::AnimationTiledTests::Flags)
bool FOdysseyAnimationTiledPersistenceTest::RunTest(const FString& Parameters)
{
    using namespace Odyssey::AnimationTiledTests;
    FScopedDisableValidateOnSave DisableValidation;
    FString FixtureId;
    const bool bWriteCookFixtures = FParse::Value(FCommandLine::Get(), TEXT("OdysseyCookFixtureId="), FixtureId);
    const FString Id = FGuid::NewGuid().ToString(EGuidFormats::Digits);
    const FString Mount = TEXT("/OdysseyAnimationTest/");
    const FString Directory = FPaths::ConvertRelativePathToFull(FPaths::ProjectPluginsDir() / TEXT("Odyssey-Plugin/Intermediate/OdysseyAnimationTests/"));
    FPackageName::RegisterMountPoint(Mount, Directory + TEXT("/"));
    ON_SCOPE_EXIT { FPackageName::UnRegisterMountPoint(Mount, Directory + TEXT("/")); };
    const FString Suffix = TEXT("Baked");
    const FString PackageName = Mount + Id + Suffix;
    UPackage* Package = CreatePackage(*PackageName);
    TStrongObjectPtr<UOdysseyAnimation> Animation(MakeAnimation(Package, EOdysseyAnimationFormat::BGRA8));
    const FString AssetName = Animation->GetName();
    const FString Filename = Directory / (Id + Suffix + TEXT(".uasset"));
    IFileManager::Get().MakeDirectory(*Directory, true);
    ON_SCOPE_EXIT { IFileManager::Get().Delete(*Filename); };
    FSavePackageArgs Args;
    Args.TopLevelFlags = RF_Public;
    Args.SaveFlags = SAVE_NoError;
    const TArray<FLinearColor> FirstImage = Render(Animation.Get(), 0, EOdysseyRenderingType::Render);
    const TArray<FLinearColor> SecondImage = Render(Animation.Get(), 2, EOdysseyRenderingType::Render);
    if (!TestTrue(TEXT("Save animation package"), UPackage::SavePackage(Package, Animation.Get(), *Filename, Args))) { return false; }
    Package->Rename(*(PackageName + TEXT("_Original")), nullptr, REN_DontCreateRedirectors | REN_NonTransactional);
    UPackage* LoadedPackage = LoadPackage(nullptr, *PackageName, LOAD_None);
    if (!TestNotNull(TEXT("Reload animation package"), LoadedPackage)) { return false; }
    UOdysseyAnimation* Loaded = FindObject<UOdysseyAnimation>(LoadedPackage, *AssetName);
    if (!TestNotNull(TEXT("Reload animation"), Loaded)) { return false; }
    const UOdysseyAnimationLayerStackUserData* LoadedEditorData = Loaded->GetAssetUserData<UOdysseyAnimationLayerStackUserData>();
    if (!TestNotNull(TEXT("Editor data is saved with the animation"), LoadedEditorData)) { return false; }
    TestTrue(TEXT("Saved layer stack overrides the animation rendering"), Loaded->GetRenderingOverride() == LoadedEditorData->LayerStack.Get());
    if (!TestNotNull(TEXT("Editor animation retains its layer stack"), LoadedEditorData->LayerStack.Get())) { return false; }
    TestEqual(TEXT("Saved layer stack keeps its layers"), LoadedEditorData->LayerStack->GetRootLayers().Num(), 1);
    const FArrayProperty* FramesProperty = FindFProperty<FArrayProperty>(Loaded->GetClass(), TEXT("Frames"));
    if (!TestNotNull(TEXT("Baked animation frames property"), FramesProperty)) { return false; }
    const TArray<FOdysseyAnimationFrame>& BakedFrames = *FramesProperty->ContainerPtrToValuePtr<TArray<FOdysseyAnimationFrame>>(Loaded);
    if (!TestEqual(TEXT("Saving bakes both distinct images"), BakedFrames.Num(), 2)) { return false; }
    TestEqual(TEXT("First baked exposure"), BakedFrames[0].Exposure, 2);
    TestEqual(TEXT("Second baked exposure"), BakedFrames[1].Exposure, 3);
    TestNotNull(TEXT("First baked texture persists"), BakedFrames[0].Texture);
    TestNotNull(TEXT("Second baked texture persists"), BakedFrames[1].Texture);
    for (int32 Index = 0; Index < 2; ++Index)
    {
        TestNotNull(TEXT("Image is saved with the cell"), Cell(Loaded, Index)->GetTiledImage());
        TestNull(TEXT("No render texture is saved with the cell"), Cell(Loaded, Index)->GetRenderTexture());
        FObjectPropertyBase* Property = FindFProperty<FObjectPropertyBase>(Cell(Loaded, Index)->GetClass(), TEXT("TiledImage"));
        TestTrue(TEXT("Image property is stripped when cooking"), Property->HasAnyPropertyFlags(CPF_EditorOnly));
    }
    ComparePixels(*this, TEXT("Reloaded first image"), FirstImage, Render(Loaded, 0, EOdysseyRenderingType::Render), 0.0001f);
    ComparePixels(*this, TEXT("Reloaded second image"), SecondImage, Render(Loaded, 2, EOdysseyRenderingType::Render), 0.0001f);
    Compare(*this, Loaded, 0);
    Compare(*this, Loaded, 2);
    if (bWriteCookFixtures)
    {
        const FString FixturePackage = TEXT("/Odyssey/Automation/Generated/") + FixtureId + TEXT("/") + Suffix;
        UPackage* CookPackage = CreatePackage(*FixturePackage);
        UOdysseyAnimation* CookAnimation = DuplicateObject<UOdysseyAnimation>(Loaded, CookPackage, *Suffix);
        const FString CookFilename = FPackageName::LongPackageNameToFilename(FixturePackage, TEXT(".uasset"));
        if (IFileManager::Get().FileExists(*CookFilename)) { AddError(TEXT("Cook fixture already exists")); return false; }
        IFileManager::Get().MakeDirectory(*FPaths::GetPath(CookFilename), true);
        TestTrue(TEXT("Write cook fixture"), UPackage::SavePackage(CookPackage, CookAnimation, *CookFilename, Args));
        AddInfo(TEXT("Cook fixture: ") + FixturePackage + TEXT(".") + Suffix);
    }
    return true;
}
#endif
