// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#include "OdysseyTextureLayerImageRaster.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "Editor.h"
#include "EditorValidatorSubsystem.h"
#include "Engine/Texture2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "HAL/FileManager.h"
#include "HAL/IConsoleManager.h"
#include "HAL/PlatformTime.h"
#include "ImageCore.h"
#include "ImageCoreUtils.h"
#include "Misc/AutomationTest.h"
#include "Misc/PackageName.h"
#include "Misc/Paths.h"
#include "Misc/ScopeExit.h"
#include "RenderingThread.h"
#include "RenderGraphUtils.h"
#include "Math/ScaleMatrix.h"
#include "Math/TranslationMatrix.h"
#include "Math/RotationMatrix.h"
#include "ScopedTransaction.h"
#include "TextureCompiler.h"
#include "TextureResource.h"
#include "UObject/Package.h"
#include "UObject/SavePackage.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/UnrealType.h"

#include "OdysseyRasterBlockMutator.h"
#include "OdysseySurfaceTexture2DEditable.h"
#include "OdysseyTextureLayerStack.h"
#include "OdysseyTextureLayerStackUserData.h"
#include "OdysseyTiledImage.h"

namespace Odyssey::TextureLayerTiledImageTests
{
constexpr int32 Width = 73;
constexpr int32 Height = 67;

UTexture2D* MakeTexture(UObject* Outer, ETextureSourceFormat Format)
{
    UTexture2D* Texture = NewObject<UTexture2D>(Outer, NAME_None, RF_Public | RF_Transactional);
    FImage Image(Width, Height, ERawImageFormat::RGBA32F, EGammaSpace::Linear);
    for (int32 Y = 0; Y < Height; ++Y)
    {
        for (int32 X = 0; X < Width; ++X)
        {
            Image.AsRGBA32F()[Y * Width + X] = FLinearColor(
                float(X % 17) / 16.0f, float(Y % 13) / 12.0f, 0.25f, (X + Y) % 3 ? 1.0f : 0.5f);
        }
    }
    FImage Converted;
    Image.CopyTo(Converted, FImageCoreUtils::ConvertToRawImageFormat(Format), EGammaSpace::Linear);
    Texture->Source.Init(Converted);
    Texture->SRGB = false;
    Texture->MipGenSettings = TMGS_NoMipmaps;
    Texture->CompressionSettings = TC_VectorDisplacementmap;
    Texture->UpdateResource();
    FTextureCompilingManager::Get().FinishCompilation({Texture});
    return Texture;
}

UOdysseyTextureLayerImageRaster* MakeLayer(UTexture2D* Texture)
{
    UOdysseyTextureLayerStackUserData* UserData = NewObject<UOdysseyTextureLayerStackUserData>(Texture);
    Texture->AddAssetUserData(UserData);
    UserData->InitWithDefaultLayerStack();
    UserData->LayerStack->SetTextureUpdateMode(EOdysseyTextureLayerStackTextureUpdateMode::Manually);
    return CastChecked<UOdysseyTextureLayerImageRaster>(UserData->LayerStack->GetCurrentLayer());
}

bool CheckRender(FAutomationTestBase& Test, UOdysseyTextureLayerImageRaster* Layer, const UOdysseyTiledImage* Image)
{
    if (!Test.TestNotNull(TEXT("Layer owns a tiled image"), Image))
    {
        return false;
    }
    TStrongObjectPtr<UTextureRenderTarget2D> Expected(NewObject<UTextureRenderTarget2D>());
    TStrongObjectPtr<UTextureRenderTarget2D> Actual(NewObject<UTextureRenderTarget2D>());
    for (UTextureRenderTarget2D* Target : {Expected.Get(), Actual.Get()})
    {
        Target->InitCustomFormat(Width, Height, PF_A32B32G32R32F, true);
        Target->UpdateResourceImmediate(true);
    }
    Layer->Render_GameThread(Expected.Get(), FFrameNumber(0), EOdysseyRenderingType::Render);
    Image->Render(Actual.Get(), FIntRect(0, 0, Width, Height), FIntPoint::ZeroValue);
    FlushRenderingCommands();
    TArray<FLinearColor> ExpectedPixels;
    TArray<FLinearColor> ActualPixels;
    FReadSurfaceDataFlags Flags(RCM_MinMax);
    Flags.SetLinearToGamma(false);
    if (!Test.TestTrue(TEXT("Read reference pixels"), Expected->GameThread_GetRenderTargetResource()->ReadLinearColorPixels(ExpectedPixels, Flags))
        || !Test.TestTrue(TEXT("Read tiled pixels"), Actual->GameThread_GetRenderTargetResource()->ReadLinearColorPixels(ActualPixels, Flags)))
    {
        return false;
    }
    for (int32 Index = 0; Index < ExpectedPixels.Num(); ++Index)
    {
        if (!ActualPixels[Index].Equals(ExpectedPixels[Index], 0.0001f))
        {
            Test.AddError(FString::Printf(TEXT("Pixel (%d, %d): expected %s, got %s"), Index % Width, Index / Width,
                *ExpectedPixels[Index].ToString(), *ActualPixels[Index].ToString()));
            return false;
        }
    }
    return true;
}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOdysseyTextureLayerTiledImageTest, "Odyssey.Texture.TiledImage.Integration",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FOdysseyTextureLayerTiledImageTest::RunTest(const FString& Parameters)
{
    using namespace Odyssey::TextureLayerTiledImageTests;
    TStrongObjectPtr<UOdysseyTextureLayerImageRaster> Detached(NewObject<UOdysseyTextureLayerImageRaster>());
    TestNull(TEXT("Detached layer has no image"), Detached->GetTiledImage());
    TestFalse(TEXT("Detached layer cannot update an image"), Detached->UpdateTiledImage());

    const ETextureSourceFormat Formats[] = {TSF_BGRA8, TSF_RGBA16, TSF_RGBA16F, TSF_RGBA32F};
    for (ETextureSourceFormat Format : Formats)
    {
        TStrongObjectPtr<UTexture2D> Texture(MakeTexture(GetTransientPackage(), Format));
        UOdysseyTextureLayerImageRaster* Layer = MakeLayer(Texture.Get());
        const UOdysseyTextureLayerImageRaster* ConstLayer = Layer;
        TestNull(TEXT("Getter does not create an image"), ConstLayer->GetTiledImage());
        TestTrue(TEXT("Update tiled snapshot"), Layer->UpdateTiledImage());
        CheckRender(*this, Layer, Layer->GetTiledImage());
        TestEqual(TEXT("Snapshot belongs to layer"), Layer->GetTiledImage()->GetOuter(), static_cast<UObject*>(Layer));

        // Exercise the unchanged paint/undo path with a clear spanning tile boundaries.
        {
            FScopedTransaction Transaction(NSLOCTEXT("OdysseyTextureTests", "Clear", "Clear raster test"));
            FOdysseyRasterBlockMutator Mutator(Layer->GetRasterBlock());
            TSharedPtr<ULIS::FBlock> Empty = MakeShared<ULIS::FBlock>(Width, Height, Layer->GetRasterBlock()->GetFormat());
            FMemory::Memzero(Empty->Bits(), Empty->BytesTotal());
            Mutator.Copy(Empty, {ULIS::FRectI::FromXYWH(0, 0, Width, Height)});
            Mutator.Commit();
        }
        TestTrue(TEXT("Update tiled snapshot"), Layer->UpdateTiledImage());
        CheckRender(*this, Layer, Layer->GetTiledImage());
        TestTrue(TEXT("Undo raster edit"), GEditor->UndoTransaction());
        TestTrue(TEXT("Update tiled snapshot"), Layer->UpdateTiledImage());
        CheckRender(*this, Layer, Layer->GetTiledImage());
        TestTrue(TEXT("Redo raster edit"), GEditor->RedoTransaction());
        TestTrue(TEXT("Update tiled snapshot"), Layer->UpdateTiledImage());
        CheckRender(*this, Layer, Layer->GetTiledImage());

        TStrongObjectPtr<UTexture2D> Duplicate(DuplicateObject<UTexture2D>(Texture.Get(), GetTransientPackage()));
        UOdysseyTextureLayerStackUserData* DuplicateData = CastChecked<UOdysseyTextureLayerStackUserData>(
            Duplicate->GetAssetUserDataOfClass(UOdysseyTextureLayerStackUserData::StaticClass()));
        UOdysseyTextureLayerImageRaster* DuplicateLayer = CastChecked<UOdysseyTextureLayerImageRaster>(DuplicateData->LayerStack->GetCurrentLayer());
        TestTrue(TEXT("Update duplicated snapshot"), DuplicateLayer->UpdateTiledImage());
        CheckRender(*this, DuplicateLayer, DuplicateLayer->GetTiledImage());
        TestTrue(TEXT("Duplicate owns a separate snapshot"), DuplicateLayer->GetTiledImage() != Layer->GetTiledImage());
    }
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOdysseyTextureLayerTiledImagePackageTest, "Odyssey.Texture.TiledImage.PackagePersistence",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FOdysseyTextureLayerTiledImagePackageTest::RunTest(const FString& Parameters)
{
    using namespace Odyssey::TextureLayerTiledImageTests;
    // This temporary fixture tests persistence, not project asset validation.
    FScopedDisableValidateOnSave DisableValidation;
    const FString Id = FGuid::NewGuid().ToString(EGuidFormats::Digits);
    const FString MountPoint = TEXT("/OdysseyTiledLayerTests/");
    const FString Directory = FPaths::ConvertRelativePathToFull(FPaths::ProjectPluginsDir()
        / TEXT("Odyssey-Plugin/Intermediate/OdysseyTextureTests")) + TEXT("/");
    FPackageName::RegisterMountPoint(MountPoint, Directory);
    ON_SCOPE_EXIT { FPackageName::UnRegisterMountPoint(MountPoint, Directory); };
    const FString PackageName = MountPoint + Id;
    const FString Filename = Directory / (Id + TEXT(".uasset"));
    IFileManager::Get().MakeDirectory(*FPaths::GetPath(Filename), true);
    ON_SCOPE_EXIT { IFileManager::Get().Delete(*Filename); };
    UPackage* Package = CreatePackage(*PackageName);
    TStrongObjectPtr<UTexture2D> Texture(MakeTexture(Package, TSF_BGRA8));
    UOdysseyTextureLayerImageRaster* Layer = MakeLayer(Texture.Get());
    const FString AssetName = Texture->GetName();
    FSavePackageArgs Args;
    Args.TopLevelFlags = RF_Public;
    Args.SaveFlags = SAVE_NoError;
    // Do not request a snapshot: PreSave must create and persist it on an existing layer.
    if (!TestTrue(TEXT("Save real texture package"), UPackage::SavePackage(Package, Texture.Get(), *Filename, Args)))
    {
        return false;
    }
    CheckRender(*this, Layer, Layer->GetTiledImage());
    // Keep the original alive under another name to force a real disk load.
    Package->Rename(*(PackageName + TEXT("_Original")), nullptr, REN_DontCreateRedirectors | REN_NonTransactional);
    UPackage* LoadedPackage = LoadPackage(nullptr, *PackageName, LOAD_None);
    if (!TestNotNull(TEXT("Reload texture package"), LoadedPackage))
    {
        return false;
    }
    UTexture2D* LoadedTexture = FindObject<UTexture2D>(LoadedPackage, *AssetName);
    if (!TestNotNull(TEXT("Reload texture asset"), LoadedTexture))
    {
        return false;
    }
    UOdysseyTextureLayerStackUserData* Data = CastChecked<UOdysseyTextureLayerStackUserData>(
        LoadedTexture->GetAssetUserDataOfClass(UOdysseyTextureLayerStackUserData::StaticClass()));
    UOdysseyTextureLayerImageRaster* LoadedLayer = CastChecked<UOdysseyTextureLayerImageRaster>(Data->LayerStack->GetCurrentLayer());
    // The getter must expose the persisted image without reconstructing it.
    const UOdysseyTiledImage* Persisted = LoadedLayer->GetTiledImage();
    CheckRender(*this, LoadedLayer, Persisted);
    return true;
}


namespace Odyssey::TextureLayerTiledImageTests
{
TArray<FLinearColor> ReadPipeline(IOdysseyTextureRenderingAbility* Renderer, bool bTiled,
    FIntRect SourceRect, FIntRect DestinationRect, FMatrix Transform)
{
    IConsoleManager::Get().FindConsoleVariable(TEXT("Odyssey.Texture.UseTiledRendering"))->Set(bTiled ? 1 : 0);
    IOdysseyTextureRenderingAbility::FRenderFunction Draw;
    if (!Renderer->BuildRenderPipeline(FFrameNumber(0), EOdysseyRenderingType::Render, Draw))
    {
        return {};
    }
    TStrongObjectPtr<UTextureRenderTarget2D> Target(NewObject<UTextureRenderTarget2D>());
    Target->InitCustomFormat(Width, Height, PF_A32B32G32R32F, true);
    Target->UpdateResourceImmediate(true);
    ENQUEUE_RENDER_COMMAND(OdysseyTextureTestRender)(
        [Target, Draw = MoveTemp(Draw), SourceRect, DestinationRect, Transform](FRHICommandListImmediate& RHICmdList)
        {
            FRDGBuilder Graph(RHICmdList);
            FRDGTextureRef Destination = Target->GetRenderTargetResource()->GetRenderTargetTexture(Graph);
            AddClearRenderTargetPass(Graph, Destination, FLinearColor::Transparent);
            Draw(Graph, GMaxRHIFeatureLevel, Destination, SourceRect, DestinationRect, Transform);
            Graph.Execute();
        });
    FlushRenderingCommands();
    TArray<FLinearColor> Pixels;
    FReadSurfaceDataFlags Flags(RCM_MinMax);
    Flags.SetLinearToGamma(false);
    Target->GameThread_GetRenderTargetResource()->ReadLinearColorPixels(Pixels, Flags);
    return Pixels;
}

bool ComparePipelines(FAutomationTestBase& Test, IOdysseyTextureRenderingAbility* Renderer,
    FIntRect SourceRect = FIntRect(0, 0, Width, Height), FIntRect DestinationRect = FIntRect(0, 0, Width, Height),
    FMatrix Transform = FMatrix::Identity)
{
    const auto Legacy = ReadPipeline(Renderer, false, SourceRect, DestinationRect, Transform);
    const auto Tiled = ReadPipeline(Renderer, true, SourceRect, DestinationRect, Transform);
    if (!Test.TestEqual(TEXT("Both pipelines rendered every pixel"), Legacy.Num(), Width * Height)
        || !Test.TestEqual(TEXT("Tiled render size"), Tiled.Num(), Legacy.Num()))
    {
        return false;
    }
    for (int32 Index = 0; Index < Legacy.Num(); ++Index)
    {
        if (!Legacy[Index].Equals(Tiled[Index], 0.0001f))
        {
            Test.AddError(FString::Printf(TEXT("Pipeline mismatch at (%d,%d): %s vs %s"), Index % Width,
                Index / Width, *Legacy[Index].ToString(), *Tiled[Index].ToString()));
            return false;
        }
    }
    return true;
}

TMap<FIntPoint, FOdysseyTileId> ReadTileIds(const UOdysseyTiledImage* Image)
{
    const FMapProperty* Property = FindFProperty<FMapProperty>(Image->GetClass(), TEXT("Tiles"));
    return *Property->ContainerPtrToValuePtr<TMap<FIntPoint, FOdysseyTileId>>(Image);
}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOdysseyTextureLayerTiledPipelineTest, "Odyssey.Texture.TiledImage.RDGPipeline",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FOdysseyTextureLayerTiledPipelineTest::RunTest(const FString& Parameters)
{
    using namespace Odyssey::TextureLayerTiledImageTests;
    IConsoleVariable* Switch = IConsoleManager::Get().FindConsoleVariable(TEXT("Odyssey.Texture.UseTiledRendering"));
    const int32 PreviousValue = Switch->GetInt();
    ON_SCOPE_EXIT { Switch->Set(PreviousValue); };
    for (ETextureSourceFormat Format : {TSF_BGRA8, TSF_RGBA16, TSF_RGBA16F, TSF_RGBA32F})
    {
        TStrongObjectPtr<UTexture2D> Texture(MakeTexture(GetTransientPackage(), Format));
        UOdysseyTextureLayerImageRaster* Layer = MakeLayer(Texture.Get());
        ComparePipelines(*this, Layer);
        ComparePipelines(*this, Layer, FIntRect(1, 3, 70, 65), FIntRect(4, 1, 64, 60));
        ComparePipelines(*this, Layer, FIntRect(0, 0, Width, Height), FIntRect(0, 0, Width, Height),
            FScaleMatrix(FVector(1.25, 0.8, 1)) * FRotationMatrix(FRotator(0, 17, 0))
                * FTranslationMatrix(FVector(3, 2, 0)));

        UOdysseyLayerStack* Stack = Layer->GetLayerStack();
        UOdysseyTextureLayerImageRaster* Top = CastChecked<UOdysseyTextureLayerImageRaster>(Stack->AddLayer(
            UOdysseyTextureLayerImageRaster::StaticClass()));
        FOdysseyRasterBlockMutator Copy(Top->GetRasterBlock(), false);
        Copy.Copy(Layer->GetRasterBlock()->GetBlock(), {ULIS::FRectI::FromXYWH(0, 0, Width, Height)});
        Copy.Commit();
        Top->SetOpacity(0.63f);
        for (EOdysseyBlendMode Mode : {EOdysseyBlendMode::Normal, EOdysseyBlendMode::Multiply, EOdysseyBlendMode::Screen})
        {
            Top->SetBlendMode(Mode);
            Top->SetInheritsAlpha(false);
            ComparePipelines(*this, Stack->GetLayerRoot());
            Top->SetInheritsAlpha(true);
            ComparePipelines(*this, Stack->GetLayerRoot());
        }
    }
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOdysseyTextureLayerTiledPartialTest, "Odyssey.Texture.TiledImage.PartialInteractiveUpdates",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FOdysseyTextureLayerTiledPartialTest::RunTest(const FString& Parameters)
{
    using namespace Odyssey::TextureLayerTiledImageTests;
    IConsoleVariable* Switch = IConsoleManager::Get().FindConsoleVariable(TEXT("Odyssey.Texture.UseTiledRendering"));
    const int32 PreviousValue = Switch->GetInt();
    ON_SCOPE_EXIT { Switch->Set(PreviousValue); };
    TStrongObjectPtr<UTexture2D> Texture(MakeTexture(GetTransientPackage(), TSF_BGRA8));
    UOdysseyTextureLayerImageRaster* Layer = MakeLayer(Texture.Get());
    TestTrue(TEXT("Create initial tiled snapshot"), Layer->UpdateTiledImage());
    const auto Before = ReadTileIds(Layer->GetTiledImage());
    TestEqual(TEXT("Fixture spans four tiles"), Before.Num(), 4);
    // Keep a render snapshot alive while changing the image, as queued rendering does.
    auto Snapshot = Layer->GetTiledImage()->BuildRenderFunction(FIntRect(0, 0, Width, Height));
    const auto BeforePixels = ReadPipeline(Layer, false, FIntRect(0, 0, Width, Height),
        FIntRect(0, 0, Width, Height), FMatrix::Identity);
    const double Start = FPlatformTime::Seconds();
    FOdysseyRasterBlockMutator Mutator(Layer->GetRasterBlock(), false);
    Mutator.EditTilesFromRects({ULIS::FRectI::FromXYWH(2, 3, 4, 5)},
        [](TSharedPtr<ULIS::FBlock> Block, const FOdysseyInvalidTileMap&) -> TArray<ULIS::FEvent>
        {
            for (int32 Y = 3; Y < 8; ++Y)
            {
                FMemory::Memzero(Block->Bits() + (Y * Width + 2) * 4, 4 * 4);
            }
            return {};
        });
    TestTrue(TEXT("Getter leaves pending edits unsynchronized"), ReadTileIds(Layer->GetTiledImage()).OrderIndependentCompareEqual(Before));
    TestTrue(TEXT("Explicitly update interactive edits"), Layer->UpdateTiledImage());
    const auto After = ReadTileIds(Layer->GetTiledImage());
    int32 ChangedTiles = 0;
    for (const auto& Pair : Before)
    {
        ChangedTiles += !(After.FindChecked(Pair.Key) == Pair.Value);
    }
    TestEqual(TEXT("Interactive update replaces only one tile"), ChangedTiles, 1);
    AddInfo(FString::Printf(TEXT("Small edit + tile sync CPU submission: %.3f ms; replaced tile payload: %d bytes of %d bytes"),
        (FPlatformTime::Seconds() - Start) * 1000.0, 64 * 64 * 4, Before.Num() * 64 * 64 * 4));
    ComparePipelines(*this, Layer); // Before commit: the interactive drawing must already be visible.

    TStrongObjectPtr<UTextureRenderTarget2D> SnapshotTarget(NewObject<UTextureRenderTarget2D>());
    SnapshotTarget->InitCustomFormat(Width, Height, PF_A32B32G32R32F, true);
    SnapshotTarget->UpdateResourceImmediate(true);
    ENQUEUE_RENDER_COMMAND(OdysseyTextureTestSnapshot)(
        [SnapshotTarget, Snapshot = MoveTemp(Snapshot)](FRHICommandListImmediate& RHICmdList)
        {
            FRDGBuilder Graph(RHICmdList);
            Snapshot(Graph, SnapshotTarget->GetRenderTargetResource()->GetRenderTargetTexture(Graph), FIntPoint::ZeroValue);
            Graph.Execute();
        });
    FlushRenderingCommands();
    TArray<FLinearColor> SnapshotPixels;
    FReadSurfaceDataFlags Flags(RCM_MinMax);
    Flags.SetLinearToGamma(false);
    SnapshotTarget->GameThread_GetRenderTargetResource()->ReadLinearColorPixels(SnapshotPixels, Flags);
    TestEqual(TEXT("Snapshot pixel count"), SnapshotPixels.Num(), BeforePixels.Num());
    for (int32 Index = 0; Index < SnapshotPixels.Num(); ++Index)
    {
        if (!SnapshotPixels[Index].Equals(BeforePixels[Index], 0.0001f))
        {
            AddError(TEXT("Prepared RDG snapshot was changed by a later edit"));
            break;
        }
    }
    Mutator.Abort();
    ComparePipelines(*this, Layer);
    return true;
}

#endif
