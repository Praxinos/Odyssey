// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#if WITH_DEV_AUTOMATION_TESTS
#include "Editor.h"
#include "EditorValidatorSubsystem.h"
#include "Engine/Texture2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "HAL/FileManager.h"
#include "HAL/IConsoleManager.h"
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
#include "OdysseyTiledImage.h"
#include "OdysseyAnimationLayerStackUserData.h"

namespace Odyssey::AnimationTiledTests
{
constexpr int32 Width = 73;
constexpr int32 Height = 67;
constexpr EAutomationTestFlags Flags = EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter;

UOdysseyAnimationCellImageRaster* Cell(UOdysseyAnimation* Animation, int32 Index)
{
    return CastChecked<UOdysseyAnimationCellImageRaster>(CastChecked<UOdysseyAnimationLayer>(UOdysseyAnimationLayerStackUserData::GetLayerStack(Animation)->GetRootLayers()[0])->GetCells()[Index]);
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
    UOdysseyAnimationLayer* Layer = CastChecked<UOdysseyAnimationLayer>(UOdysseyAnimationLayerStackUserData::GetLayerStack(Animation)->AddLayer(UOdysseyAnimationLayerImageRaster::StaticClass()));
    UOdysseyAnimationLayerStackUserData::GetLayerStack(Animation)->SetCurrentLayer(Layer);
    UOdysseyAnimationCellImageRaster* First = CastChecked<UOdysseyAnimationCellImageRaster>(Layer->AddCell(UOdysseyAnimationCellImageRaster::StaticClass()));
    UOdysseyAnimationCellImageRaster* Second = CastChecked<UOdysseyAnimationCellImageRaster>(Layer->AddCell(UOdysseyAnimationCellImageRaster::StaticClass()));
    First->SetExposure(2);
    Second->SetExposure(3);
    Fill(First, 47);
    Fill(Second, 191);
    return Animation;
}

TArray<FLinearColor> Render(UOdysseyAnimation* Animation, int32 Frame, uint64 Type, bool bTiled)
{
    IConsoleManager::Get().FindConsoleVariable(TEXT("Odyssey.Animation.UseTiledRendering"))->Set(bTiled ? 1 : 0);
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

bool Compare(FAutomationTestBase& Test, UOdysseyAnimation* Animation, int32 Frame, uint64 Type = EOdysseyRenderingType::Render)
{
    auto Expected = Render(Animation, Frame, Type, false);
    auto Actual = Render(Animation, Frame, Type, true);
    if (!Test.TestEqual(TEXT("Rendered pixel count"), Actual.Num(), Width * Height)
        || !Test.TestEqual(TEXT("Reference pixel count"), Expected.Num(), Actual.Num()))
    {
        return false;
    }
    const float Tolerance = 0.0001f;
    float MaxDifference = 0.0f;
    for (int32 Index = 0; Index < Actual.Num(); ++Index)
    {
        const FLinearColor Difference = Actual[Index] - Expected[Index];
        MaxDifference = FMath::Max(MaxDifference, FMath::Max(
            FMath::Max(FMath::Abs(Difference.R), FMath::Abs(Difference.G)),
            FMath::Max(FMath::Abs(Difference.B), FMath::Abs(Difference.A))));
        if (!Actual[Index].Equals(Expected[Index], Tolerance))
        {
            Test.AddError(FString::Printf(TEXT("Frame %d pixel %d differs: %s vs %s"), Frame, Index,
                *Expected[Index].ToString(), *Actual[Index].ToString()));
            return false;
        }
    }
    Test.AddInfo(FString::Printf(TEXT("Frame %d render type %llu: maximum component difference %.7f"), Frame, Type, MaxDifference));
    return true;
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
    auto Switch = IConsoleManager::Get().FindConsoleVariable(TEXT("Odyssey.Animation.UseTiledRendering"));
    const int32 Previous = Switch->GetInt();
    ON_SCOPE_EXIT { Switch->Set(Previous); };
    TStrongObjectPtr<UOdysseyAnimationCellImageRaster> Detached(NewObject<UOdysseyAnimationCellImageRaster>());
    TestNull(TEXT("Pure getter on a detached cell"), Detached->GetTiledImage());
    TestFalse(TEXT("Detached update fails safely"), Detached->UpdateTiledImage());
    for (auto Format : {EOdysseyAnimationFormat::BGRA8, EOdysseyAnimationFormat::RGBAF})
    {
        TStrongObjectPtr<UOdysseyAnimation> Animation(MakeAnimation(GetTransientPackage(), Format));
        auto First = Cell(Animation.Get(), 0);
        auto Second = Cell(Animation.Get(), 1);
        TestNull(TEXT("Getter does not create the snapshot"), First->GetTiledImage());
        for (int32 Frame = 0; Frame < 5; ++Frame) { Compare(*this, Animation.Get(), Frame); }
        TestEqual(TEXT("Cell exposures form five frames"), Animation->GetFrameCount(), 5);
        UOdysseyAnimationLayer* Layer = CastChecked<UOdysseyAnimationLayer>(UOdysseyAnimationLayerStackUserData::GetLayerStack(Animation.Get())->GetRootLayers()[0]);
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
        Second->UpdateTiledImage();
        const auto OtherTiles = TileIds(Second->GetTiledImage());
        First->Clear();
        Compare(*this, Animation.Get(), 0);
        TestTrue(TEXT("Undo clear"), GEditor->UndoTransaction());
        Compare(*this, Animation.Get(), 0);
        TestTrue(TEXT("Redo clear"), GEditor->RedoTransaction());
        Compare(*this, Animation.Get(), 0);
        TestTrue(TEXT("Other cell stays unchanged"), TileIds(Second->GetTiledImage()).OrderIndependentCompareEqual(OtherTiles));
        TStrongObjectPtr<UOdysseyAnimation> Duplicate(DuplicateObject<UOdysseyAnimation>(Animation.Get(), GetTransientPackage()));
        Compare(*this, Duplicate.Get(), 2);
        TestTrue(TEXT("Duplicated cell owns its snapshot"), Cell(Duplicate.Get(), 1)->GetTiledImage() != Second->GetTiledImage());
    }
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOdysseyAnimationTiledPartialTest, "Odyssey.Animation.TiledImage.PartialUpdates", Odyssey::AnimationTiledTests::Flags)
bool FOdysseyAnimationTiledPartialTest::RunTest(const FString& Parameters)
{
    using namespace Odyssey::AnimationTiledTests;
    auto Switch = IConsoleManager::Get().FindConsoleVariable(TEXT("Odyssey.Animation.UseTiledRendering"));
    const int32 Previous = Switch->GetInt();
    ON_SCOPE_EXIT { Switch->Set(Previous); };
    TStrongObjectPtr<UOdysseyAnimation> Animation(MakeAnimation(GetTransientPackage(), EOdysseyAnimationFormat::BGRA8));
    auto First = Cell(Animation.Get(), 0);
    TestTrue(TEXT("Initial explicit update"), First->UpdateTiledImage());
    const auto Before = TileIds(First->GetTiledImage());
    FOdysseyRasterBlockMutator Mutator(First->GetRasterBlock(), false);
    Mutator.EditTilesFromRects({ULIS::FRectI::FromXYWH(2, 3, 4, 5)},
        [](TSharedPtr<ULIS::FBlock> Block, const FOdysseyInvalidTileMap&) -> TArray<ULIS::FEvent>
        {
            for (int32 Y = 3; Y < 8; ++Y) { FMemory::Memzero(Block->Bits() + (Y * Width + 2) * 4, 16); }
            return {};
        });
    TestTrue(TEXT("Getter does not synchronize dirty tiles"), TileIds(First->GetTiledImage()).OrderIndependentCompareEqual(Before));
    TestTrue(TEXT("Update interactive edits before commit"), First->UpdateTiledImage());
    const auto After = TileIds(First->GetTiledImage());
    int32 Changed = 0;
    for (const auto& Pair : Before) { Changed += !(After.FindChecked(Pair.Key) == Pair.Value); }
    TestEqual(TEXT("Only one tile changes"), Changed, 1);
    Compare(*this, Animation.Get(), 0);
    Mutator.Abort();
    Compare(*this, Animation.Get(), 0);
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
    IConsoleVariable* Switch = IConsoleManager::Get().FindConsoleVariable(TEXT("Odyssey.Animation.UseTiledRendering"));
    const int32 Previous = Switch->GetInt();
    ON_SCOPE_EXIT { Switch->Set(Previous); };
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
    Switch->Set(1);
    if (!TestTrue(TEXT("Save animation package"), UPackage::SavePackage(Package, Animation.Get(), *Filename, Args))) { return false; }
    Package->Rename(*(PackageName + TEXT("_Original")), nullptr, REN_DontCreateRedirectors | REN_NonTransactional);
    UPackage* LoadedPackage = LoadPackage(nullptr, *PackageName, LOAD_None);
    if (!TestNotNull(TEXT("Reload animation package"), LoadedPackage)) { return false; }
    UOdysseyAnimation* Loaded = FindObject<UOdysseyAnimation>(LoadedPackage, *AssetName);
    if (!TestNotNull(TEXT("Reload animation"), Loaded)) { return false; }
    const UOdysseyAnimationLayerStackUserData* LoadedEditorData = Loaded->GetAssetUserData<UOdysseyAnimationLayerStackUserData>();
    if (!TestNotNull(TEXT("Editor data is saved with the animation"), LoadedEditorData)) { return false; }
    TestTrue(TEXT("Editor data is the animation editor data"), Loaded->GetEditorData() == LoadedEditorData);
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
        const UOdysseyTiledImage* Persisted = Cell(Loaded, Index)->GetTiledImage();
        TestNotNull(TEXT("Snapshot persisted before explicit update"), Persisted);
        if (Persisted)
        {
            const TArray<FLinearColor> Reference = Render(Loaded, Index == 0 ? 0 : 2, EOdysseyRenderingType::Render, false);
            TStrongObjectPtr<UTextureRenderTarget2D> Target(NewObject<UTextureRenderTarget2D>());
            Target->InitCustomFormat(Width, Height, PF_A32B32G32R32F, true);
            Target->UpdateResourceImmediate(true);
            Persisted->Render(Target.Get(), FIntRect(0, 0, Width, Height), FIntPoint::ZeroValue);
            FlushRenderingCommands();
            TArray<FLinearColor> Pixels;
            FReadSurfaceDataFlags ReadFlags(RCM_MinMax);
            ReadFlags.SetLinearToGamma(false);
            Target->GameThread_GetRenderTargetResource()->ReadLinearColorPixels(Pixels, ReadFlags);
            TestEqual(TEXT("Persisted image pixel count"), Pixels.Num(), Reference.Num());
            for (int32 Pixel = 0; Pixel < FMath::Min(Pixels.Num(), Reference.Num()); ++Pixel)
            {
                if (!Pixels[Pixel].Equals(Reference[Pixel], 0.0001f))
                {
                    AddError(TEXT("Persisted tile pixels differ before reconstruction"));
                    break;
                }
            }
        }
        FObjectPropertyBase* Property = FindFProperty<FObjectPropertyBase>(Cell(Loaded, Index)->GetClass(), TEXT("TiledImage"));
        TestTrue(TEXT("Snapshot property is stripped when cooking"), Property->HasAnyPropertyFlags(CPF_EditorOnly));
    }
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
