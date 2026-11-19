// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#if WITH_DEV_AUTOMATION_TESTS
#include "Editor.h"
#include "EditorValidatorSubsystem.h"
#include "Engine/Texture2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "HAL/FileManager.h"
#include "ImageCore.h"
#include "ImageCoreUtils.h"
#include "Math/RotationMatrix.h"
#include "Math/ScaleMatrix.h"
#include "Math/TranslationMatrix.h"
#include "Misc/AutomationTest.h"
#include "Misc/PackageName.h"
#include "Misc/Paths.h"
#include "Misc/ScopeExit.h"
#include "RenderGraphUtils.h"
#include "RenderingThread.h"
#include "ScopedTransaction.h"
#include "TextureCompiler.h"
#include "TextureResource.h"
#include "UObject/Package.h"
#include "UObject/SavePackage.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/UObjectHash.h"
#include "UObject/UnrealType.h"

#include "OdysseyColorSpaceShader.h"
#include "OdysseyRasterBlockEdit.h"
#include "OdysseyRasterContent.h"
#include "OdysseyRasterEditSession.h"
#include "OdysseyRenderContext.h"
#include "OdysseyTextureLayerImageRaster.h"
#include "OdysseyTextureLayerStack.h"
#include "OdysseyTextureLayerStackUserData.h"
#include "OdysseyTiledImage.h"

namespace Odyssey::TextureLayerTiledImageTests
{
constexpr int32 Width = 73;
constexpr int32 Height = 67;
const ETextureSourceFormat Formats[] = { TSF_BGRA8, TSF_RGBA16, TSF_RGBA16F, TSF_RGBA32F };

/** Returns the color of the test pattern at the given pixel. */
FLinearColor PatternColor(int32 X, int32 Y)
{
    return FLinearColor(float(X % 17) / 16.0f, float(Y % 13) / 12.0f, 0.25f, (X + Y) % 3 ? 1.0f : 0.5f);
}

UTexture2D* MakeTexture(UObject* Outer, ETextureSourceFormat Format)
{
    UTexture2D* Texture = NewObject<UTexture2D>(Outer, NAME_None, RF_Public | RF_Transactional);
    FImage Image(Width, Height, ERawImageFormat::RGBA32F, EGammaSpace::Linear);
    for (int32 Y = 0; Y < Height; ++Y)
    {
        for (int32 X = 0; X < Width; ++X)
        {
            Image.AsRGBA32F()[Y * Width + X] = PatternColor(X, Y);
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

/** Returns the render texture an asset saved before the tiled image storage holds, read through its legacy property. */
UObject* LegacyRenderTexture(UObject* Owner)
{
    const FObjectProperty* Property = FindFProperty<FObjectProperty>(Owner->GetClass(), TEXT("Texture"));
    return Property ? Property->GetObjectPropertyValue_InContainer(Owner) : nullptr;
}

UOdysseyTextureLayerImageRaster* MakeLayer(UTexture2D* Texture)
{
    UOdysseyTextureLayerStackUserData* UserData = NewObject<UOdysseyTextureLayerStackUserData>(Texture);
    Texture->AddAssetUserData(UserData);
    UserData->InitWithDefaultLayerStack();
    UserData->LayerStack->SetTextureUpdateMode(EOdysseyTextureLayerStackTextureUpdateMode::Manually);
    return CastChecked<UOdysseyTextureLayerImageRaster>(UserData->LayerStack->GetCurrentLayer());
}

/** Renders the pipeline of a node as a view does, showing the active edit sessions, into a new float render target and reads it back. */
TArray<FLinearColor> ReadPipeline(const IOdysseyTextureRenderingAbility* Renderer,
    FIntRect SourceRect = FIntRect(0, 0, Width, Height), FIntRect DestinationRect = FIntRect(0, 0, Width, Height),
    FMatrix Transform = FMatrix::Identity)
{
    FOdysseyRenderContext Context;
    FOdysseyRasterEditSession::AddOverrides(Context);
    IOdysseyTextureRenderingAbility::FRenderFunction Draw;
    if (!Renderer->BuildRenderPipeline(Draw, {}, &Context))
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

/** Reads the tiled image of a layer directly, without the layer render path. */
TArray<FLinearColor> ReadImage(const UOdysseyTiledImage* Image)
{
    TStrongObjectPtr<UTextureRenderTarget2D> Target(NewObject<UTextureRenderTarget2D>());
    Target->InitCustomFormat(Width, Height, PF_A32B32G32R32F, true);
    Target->UpdateResourceImmediate(true);
    Image->Render(Target.Get(), FIntRect(0, 0, Width, Height), FIntPoint::ZeroValue);
    FlushRenderingCommands();
    TArray<FLinearColor> Pixels;
    FReadSurfaceDataFlags Flags(RCM_MinMax);
    Flags.SetLinearToGamma(false);
    Target->GameThread_GetRenderTargetResource()->ReadLinearColorPixels(Pixels, Flags);
    return Pixels;
}

bool ComparePixels(FAutomationTestBase& Test, const TCHAR* What, const TArray<FLinearColor>& Expected, const TArray<FLinearColor>& Actual, float Tolerance)
{
    if (!Test.TestEqual(FString::Printf(TEXT("%s: pixel count"), What), Actual.Num(), Expected.Num()) || Actual.IsEmpty())
    {
        return false;
    }
    for (int32 Index = 0; Index < Expected.Num(); ++Index)
    {
        if (!Actual[Index].Equals(Expected[Index], Tolerance))
        {
            Test.AddError(FString::Printf(TEXT("%s: pixel (%d, %d) expected %s, got %s"), What, Index % Width, Index / Width,
                *Expected[Index].ToString(), *Actual[Index].ToString()));
            return false;
        }
    }
    return true;
}

/** Returns the test pattern as stored with the precision of the given format. */
TArray<FLinearColor> ExpectedPattern(ETextureSourceFormat Format, float& OutTolerance)
{
    OutTolerance = Format == TSF_BGRA8 ? 0.5f / 255.0f + 0.0001f : Format == TSF_RGBA16F ? 0.001f : 0.0001f;
    TArray<FLinearColor> Pixels;
    Pixels.Reserve(Width * Height);
    for (int32 Y = 0; Y < Height; ++Y)
    {
        for (int32 X = 0; X < Width; ++X)
        {
            Pixels.Add(PatternColor(X, Y));
        }
    }
    return Pixels;
}

TMap<FIntPoint, FOdysseyTileId> ReadTileIds(const UOdysseyTiledImage* Image)
{
    const FMapProperty* Property = FindFProperty<FMapProperty>(Image->GetClass(), TEXT("Tiles"));
    return *Property->ContainerPtrToValuePtr<TMap<FIntPoint, FOdysseyTileId>>(Image);
}

/** Clears the whole layer by writing a transparent render target, as users other than tools do. */
void ClearLayer(UOdysseyTextureLayerImageRaster* Layer)
{
    FOdysseyRasterContent& Content = Layer->GetRasterContent();
    TStrongObjectPtr<UTextureRenderTarget2D> Transparent(Content.CreateRenderTarget());
    if (Transparent)
    {
        Content.Write(*Transparent, { FIntRect(0, 0, Width, Height) });
    }
}

/** Begins an edit changing no pixel on the layer, so that it renders its uncommitted edits until the edit is committed. */
TSharedPtr<FOdysseyRasterBlockEdit> BeginUnchangedEdit(FAutomationTestBase& Test, UOdysseyTextureLayerImageRaster* Layer)
{
    TSharedPtr<FOdysseyRasterBlockEdit> Edit = Layer->BeginRasterEdit();
    if (Test.TestTrue(TEXT("Begin an edit"), Edit.IsValid()))
    {
        Edit->Update({ FIntRect(0, 0, Width, Height) });
    }
    return Edit;
}

/**
 * Renders the node while the given layers display their uncommitted edits, then once these edits are committed into the tiles.
 * Both renders must match: the tiles keep the precision and the color encoding of the raster block.
 */
bool ComparePreviewAndTiles(FAutomationTestBase& Test, const IOdysseyTextureRenderingAbility* Renderer,
    const TArray<UOdysseyTextureLayerImageRaster*>& Layers, float Tolerance,
    FIntRect SourceRect = FIntRect(0, 0, Width, Height), FIntRect DestinationRect = FIntRect(0, 0, Width, Height),
    FMatrix Transform = FMatrix::Identity)
{
    TArray<TSharedPtr<FOdysseyRasterBlockEdit>> Edits;
    for (UOdysseyTextureLayerImageRaster* Layer : Layers)
    {
        Edits.Add(BeginUnchangedEdit(Test, Layer));
    }
    const TArray<FLinearColor> Preview = ReadPipeline(Renderer, SourceRect, DestinationRect, Transform);
    for (const TSharedPtr<FOdysseyRasterBlockEdit>& Edit : Edits)
    {
        if (Edit)
        {
            Edit->Commit();
        }
    }
    const TArray<FLinearColor> Tiles = ReadPipeline(Renderer, SourceRect, DestinationRect, Transform);
    return ComparePixels(Test, TEXT("Tiles render the previewed edits"), Preview, Tiles, Tolerance);
}

int32 CountRenderTextures(UObject* Layer)
{
    TArray<UObject*> Subobjects;
    GetObjectsWithOuter(Layer, Subobjects);
    return Subobjects.FilterByPredicate([](const UObject* Object) { return Object->IsA<UTexture2D>(); }).Num();
}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOdysseyTextureLayerTiledImageTest, "Odyssey.Texture.TiledImage.Integration",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

/**
 * The tiled image is the storage of raster layers: it is created with the layer stack, rendered by the layer,
 * restored by the native undo, and duplicated with the layer.
 */
bool FOdysseyTextureLayerTiledImageTest::RunTest(const FString& Parameters)
{
    using namespace Odyssey::TextureLayerTiledImageTests;
    TStrongObjectPtr<UOdysseyTextureLayerImageRaster> Detached(NewObject<UOdysseyTextureLayerImageRaster>());
    TestNull(TEXT("Detached layer has no image"), Detached->GetTiledImage());
    TestFalse(TEXT("Detached layer has no pixels"), Detached->GetRasterContent().ReadBlock().IsValid());

    for (ETextureSourceFormat Format : Formats)
    {
        TStrongObjectPtr<UTexture2D> Texture(MakeTexture(GetTransientPackage(), Format));
        UOdysseyTextureLayerImageRaster* Layer = MakeLayer(Texture.Get());
        const UOdysseyTiledImage* Image = Layer->GetTiledImage();
        if (!TestNotNull(TEXT("Layer stores its pixels in a tiled image"), Image))
        {
            return false;
        }
        TestEqual(TEXT("Image belongs to the layer"), Image->GetOuter(), static_cast<UObject*>(Layer));
        TestTrue(TEXT("Image is transactional"), Image->HasAnyFlags(RF_Transactional));
        TestNull(TEXT("Raster layer has no render texture"), LegacyRenderTexture(Layer));
        TestEqual(TEXT("Raster layer owns no texture"), CountRenderTextures(Layer), 0);

        float Tolerance = 0.0f;
        const TArray<FLinearColor> Pattern = ExpectedPattern(Format, Tolerance);
        ComparePixels(*this, TEXT("Layer renders the texture"), Pattern, ReadPipeline(Layer), Tolerance);
        ComparePixels(*this, TEXT("Image holds the texture"), Pattern, ReadImage(Image), Tolerance);

        const TMap<FIntPoint, FOdysseyTileId> Before = ReadTileIds(Image);
        {
            FScopedTransaction Transaction(NSLOCTEXT("OdysseyTextureTests", "Clear", "Clear raster test"));
            ClearLayer(Layer);
        }
        TestFalse(TEXT("Commit replaces the tiles"), ReadTileIds(Image).OrderIndependentCompareEqual(Before));
        const TArray<FLinearColor> Cleared = ReadPipeline(Layer);
        TArray<FLinearColor> Transparent;
        Transparent.Init(FLinearColor::Transparent, Width * Height);
        ComparePixels(*this, TEXT("Committed clear"), Transparent, Cleared, KINDA_SMALL_NUMBER);

        TestTrue(TEXT("Undo raster edit"), GEditor->UndoTransaction());
        TestTrue(TEXT("Undo restores the tiles"), ReadTileIds(Image).OrderIndependentCompareEqual(Before));
        ComparePixels(*this, TEXT("Undone clear"), Pattern, ReadPipeline(Layer), Tolerance);
        TestTrue(TEXT("Redo raster edit"), GEditor->RedoTransaction());
        ComparePixels(*this, TEXT("Redone clear"), Transparent, ReadPipeline(Layer), KINDA_SMALL_NUMBER);
        TestTrue(TEXT("Undo raster edit again"), GEditor->UndoTransaction());

        // The pixels shown by an edit must match the tiles after an undo.
        ComparePreviewAndTiles(*this, Layer, {Layer}, Tolerance);

        TStrongObjectPtr<UTexture2D> Duplicate(DuplicateObject<UTexture2D>(Texture.Get(), GetTransientPackage()));
        UOdysseyTextureLayerStackUserData* DuplicateData = CastChecked<UOdysseyTextureLayerStackUserData>(
            Duplicate->GetAssetUserDataOfClass(UOdysseyTextureLayerStackUserData::StaticClass()));
        UOdysseyTextureLayerImageRaster* DuplicateLayer = CastChecked<UOdysseyTextureLayerImageRaster>(DuplicateData->LayerStack->GetCurrentLayer());
        TestTrue(TEXT("Duplicate owns a separate image"), DuplicateLayer->GetTiledImage() != Image);
        ComparePixels(*this, TEXT("Duplicate renders the same pixels"), Pattern, ReadPipeline(DuplicateLayer), Tolerance);
    }
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOdysseyTextureLayerTiledImagePackageTest, "Odyssey.Texture.TiledImage.PackagePersistence",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

/** The tiled image is the only pixel storage saved with a raster layer. */
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
    if (!TestTrue(TEXT("Save texture package"), UPackage::SavePackage(Package, Texture.Get(), *Filename, Args)))
    {
        return false;
    }
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
    TestEqual(TEXT("No render texture is saved with the layer"), CountRenderTextures(LoadedLayer), 0);
    const UOdysseyTiledImage* Persisted = LoadedLayer->GetTiledImage();
    if (!TestNotNull(TEXT("Image is saved with the layer"), Persisted))
    {
        return false;
    }
    float Tolerance = 0.0f;
    const TArray<FLinearColor> Pattern = ExpectedPattern(TSF_BGRA8, Tolerance);
    ComparePixels(*this, TEXT("Reloaded image"), Pattern, ReadImage(Persisted), Tolerance);
    ComparePixels(*this, TEXT("Reloaded layer"), Pattern, ReadPipeline(LoadedLayer), Tolerance);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOdysseyTextureLayerLegacyMigrationTest, "Odyssey.Texture.TiledImage.LegacyMigration",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

/** Layers saved before the tiled image storage hold their pixels in a render texture, moved into the tiled image after loading. */
bool FOdysseyTextureLayerLegacyMigrationTest::RunTest(const FString& Parameters)
{
    using namespace Odyssey::TextureLayerTiledImageTests;
    FObjectProperty* TextureProperty = FindFProperty<FObjectProperty>(UOdysseyTextureLayerImageRaster::StaticClass(), TEXT("Texture"));
    if (!TestNotNull(TEXT("Render texture property"), TextureProperty))
    {
        return false;
    }
    for (ETextureSourceFormat Format : Formats)
    {
        TStrongObjectPtr<UTexture2D> Texture(MakeTexture(GetTransientPackage(), Format));
        UOdysseyTextureLayerImageRaster* Layer = MakeLayer(Texture.Get());
        ClearLayer(Layer);

        // Recreate the state of a loaded legacy layer: its pixels are only in its render texture.
        UTexture2D* LegacyTexture = MakeTexture(Layer, Format);
        TextureProperty->SetObjectPropertyValue_InContainer(Layer, LegacyTexture);
        Layer->PostLoad();

        TestNull(TEXT("Migration discards the render texture"), LegacyRenderTexture(Layer));
        float Tolerance = 0.0f;
        const TArray<FLinearColor> Pattern = ExpectedPattern(Format, Tolerance);
        ComparePixels(*this, TEXT("Migrated image"), Pattern, ReadImage(Layer->GetTiledImage()), Tolerance);
        ComparePixels(*this, TEXT("Migrated layer"), Pattern, ReadPipeline(Layer), Tolerance);
        ComparePreviewAndTiles(*this, Layer, {Layer}, Tolerance);
    }
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOdysseyTextureLayerTiledPipelineTest, "Odyssey.Texture.TiledImage.RDGPipeline",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

/** The tiles render exactly as the uncommitted edits did, under transforms and in a blended stack. */
bool FOdysseyTextureLayerTiledPipelineTest::RunTest(const FString& Parameters)
{
    using namespace Odyssey::TextureLayerTiledImageTests;
    for (ETextureSourceFormat Format : Formats)
    {
        TStrongObjectPtr<UTexture2D> Texture(MakeTexture(GetTransientPackage(), Format));
        UOdysseyTextureLayerImageRaster* Layer = MakeLayer(Texture.Get());
        const float Tolerance = 0.0001f;
        ComparePreviewAndTiles(*this, Layer, {Layer}, Tolerance);
        ComparePreviewAndTiles(*this, Layer, {Layer}, Tolerance, FIntRect(1, 3, 70, 65), FIntRect(4, 1, 64, 60));
        ComparePreviewAndTiles(*this, Layer, {Layer}, Tolerance, FIntRect(0, 0, Width, Height), FIntRect(0, 0, Width, Height),
            FScaleMatrix(FVector(1.25, 0.8, 1)) * FRotationMatrix(FRotator(0, 17, 0)) * FTranslationMatrix(FVector(3, 2, 0)));

        UOdysseyLayerStack* Stack = Layer->GetLayerStack();
        UOdysseyTextureLayerImageRaster* Top = CastChecked<UOdysseyTextureLayerImageRaster>(Stack->AddLayer(
            UOdysseyTextureLayerImageRaster::StaticClass()));
        TSharedPtr<ULIS::FBlock> Pixels = Layer->GetRasterContent().ReadBlock();
        if (TestTrue(TEXT("Read the layer pixels"), Pixels.IsValid()))
        {
            Top->GetRasterContent().WriteBlock(*Pixels, { FIntRect(0, 0, Width, Height) });
        }
        Top->SetOpacity(0.63f);
        for (EOdysseyBlendMode Mode : {EOdysseyBlendMode::Normal, EOdysseyBlendMode::Multiply, EOdysseyBlendMode::Screen})
        {
            Top->SetBlendMode(Mode);
            Top->SetInheritsAlpha(false);
            ComparePreviewAndTiles(*this, Stack->GetLayerRoot(), {Layer, Top}, Tolerance);
            Top->SetInheritsAlpha(true);
            ComparePreviewAndTiles(*this, Stack->GetLayerRoot(), {Layer, Top}, Tolerance);
        }
    }
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOdysseyTextureLayerTiledPartialTest, "Odyssey.Texture.TiledImage.PartialInteractiveUpdates",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

/** Uncommitted edits are rendered without touching the tiles; a commit replaces only the touched tiles. */
bool FOdysseyTextureLayerTiledPartialTest::RunTest(const FString& Parameters)
{
    using namespace Odyssey::TextureLayerTiledImageTests;
    TStrongObjectPtr<UTexture2D> Texture(MakeTexture(GetTransientPackage(), TSF_BGRA8));
    UOdysseyTextureLayerImageRaster* Layer = MakeLayer(Texture.Get());
    const UOdysseyTiledImage* Image = Layer->GetTiledImage();
    const TMap<FIntPoint, FOdysseyTileId> Before = ReadTileIds(Image);
    TestEqual(TEXT("Fixture spans four tiles"), Before.Num(), 4);
    // Keep a render snapshot alive while changing the image, as queued rendering does.
    UOdysseyTiledImage::FRenderFunction Snapshot = Image->BuildRenderFunction(FIntRect(0, 0, Width, Height));
    const TArray<FLinearColor> BeforePixels = ReadPipeline(Layer);

    const FIntPoint Erased(3, 4);
    const int32 ErasedIndex = Erased.Y * Width + Erased.X;
    TSharedPtr<FOdysseyRasterBlockEdit> Edit = Layer->BeginRasterEdit();
    if (!TestTrue(TEXT("Begin an edit"), Edit.IsValid()))
    {
        return false;
    }
    for (int32 Y = 3; Y < 8; ++Y)
    {
        FMemory::Memzero(Edit->GetBlock()->Bits() + (Y * Width + 2) * 4, 4 * 4);
    }
    Edit->Update({ FIntRect(2, 3, 6, 8) });
    TestTrue(TEXT("Uncommitted edits leave the tiles unchanged"), ReadTileIds(Image).OrderIndependentCompareEqual(Before));
    const TArray<FLinearColor> Interactive = ReadPipeline(Layer);
    if (TestEqual(TEXT("Interactive render pixel count"), Interactive.Num(), Width * Height))
    {
        TestTrue(TEXT("Uncommitted edits are rendered"), Interactive[ErasedIndex].Equals(FLinearColor::Transparent));
    }

    TestTrue(TEXT("Commit the edit"), Edit->Commit());
    TestFalse(TEXT("A commit ends the edit"), Edit->IsActive());
    const TMap<FIntPoint, FOdysseyTileId> After = ReadTileIds(Image);
    int32 ChangedTiles = 0;
    for (const TPair<FIntPoint, FOdysseyTileId>& Pair : Before)
    {
        ChangedTiles += !(After.FindChecked(Pair.Key) == Pair.Value);
    }
    TestEqual(TEXT("Commit replaces only the touched tile"), ChangedTiles, 1);
    ComparePixels(*this, TEXT("Committed tiles render the edit"), Interactive, ReadPipeline(Layer), 0.0001f);

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
    ComparePixels(*this, TEXT("Prepared RDG snapshot is not changed by a later commit"), BeforePixels, SnapshotPixels, 0.0001f);

    // An ended edit leaves the tiles as they are.
    TSharedPtr<FOdysseyRasterBlockEdit> Aborted = Layer->BeginRasterEdit();
    if (!TestTrue(TEXT("Begin another edit"), Aborted.IsValid()))
    {
        return false;
    }
    FMemory::Memzero(Aborted->GetBlock()->Bits() + (40 * Width + 40) * 4, 4 * 4);
    Aborted->Update({ FIntRect(40, 40, 44, 41) });
    Aborted->End();
    FOdysseyRenderContext EndedContext;
    FOdysseyRasterEditSession::AddOverrides(EndedContext);
    TestFalse(TEXT("An ended edit is not shown"), EndedContext.Overrides.Contains(Layer->GetRenderingId()));
    TestTrue(TEXT("Aborted edits leave the tiles unchanged"), ReadTileIds(Image).OrderIndependentCompareEqual(After));
    ComparePixels(*this, TEXT("Aborted edits are not rendered"), Interactive, ReadPipeline(Layer), 0.0001f);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOdysseyTextureLayerStackPreviewTest, "Odyssey.Texture.TiledImage.StackPreview",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

/**
 * While the texture is edited, its displayed pixels show the uncommitted edits through the view preview,
 * whereas its source, which is saved, holds the committed pixels only.
 */
bool FOdysseyTextureLayerStackPreviewTest::RunTest(const FString& Parameters)
{
    using namespace Odyssey::TextureLayerTiledImageTests;
    TStrongObjectPtr<UTexture2D> Texture(MakeTexture(GetTransientPackage(), TSF_BGRA8));
    UOdysseyTextureLayerImageRaster* Layer = MakeLayer(Texture.Get());
    UOdysseyTextureLayerStack* Stack = CastChecked<UOdysseyTextureLayerStack>(Layer->GetLayerStack());
    Stack->SetTextureUpdateMode(EOdysseyTextureLayerStackTextureUpdateMode::Instantaneous);
    Stack->ActivateTextureFastUpdate();
    FOdysseyRasterEditPreview Preview(MakeShared<FOdysseyRenderPreview>());
    Stack->SetRenderPreview(Preview.GetPreview());
    ON_SCOPE_EXIT { Stack->InactivateTextureFastUpdate(); };

    const FIntPoint Erased(3, 4);
    auto ReadDisplayedAlpha = [Stack, Erased]()
    {
        return Stack->GetSurface()->Block()->PixelBits(static_cast<uint16>(Erased.X), static_cast<uint16>(Erased.Y))[3];
    };
    auto ReadSourceAlpha = [&Texture, Erased]()
    {
        FImage Image;
        Texture->Source.GetMipImage(Image, 0);
        return Image.AsBGRA8()[Erased.Y * Width + Erased.X].A;
    };
    TestNotEqual(TEXT("Fixture pixel is not transparent"), int32(ReadDisplayedAlpha()), 0);

    TSharedPtr<FOdysseyRasterBlockEdit> Edit = Layer->BeginRasterEdit();
    if (!TestTrue(TEXT("Begin an edit"), Edit.IsValid()))
    {
        return false;
    }
    for (int32 Y = 3; Y < 8; ++Y)
    {
        FMemory::Memzero(Edit->GetBlock()->Bits() + (Y * Width + 2) * 4, 4 * 4);
    }
    Edit->Update({ FIntRect(2, 3, 6, 8) });
    TestTrue(TEXT("An edit session shows the uncommitted edits"), Preview.GetPreview()->GetContext().Overrides.Contains(Layer->GetRenderingId()));
    TestEqual(TEXT("The displayed texture shows the uncommitted edits"), int32(ReadDisplayedAlpha()), 0);

    // Leaving the edition writes the texture source, which must not hold the uncommitted edits.
    Stack->InactivateTextureFastUpdate();
    TestNotEqual(TEXT("The texture source holds the committed pixels"), int32(ReadSourceAlpha()), 0);
    Stack->ActivateTextureFastUpdate();
    Stack->SetRenderPreview(Preview.GetPreview());

    Edit->Commit();
    TestFalse(TEXT("The commit ends the edit session"), Preview.GetPreview()->GetContext().Overrides.Contains(Layer->GetRenderingId()));
    TestEqual(TEXT("The displayed texture shows the committed edits"), int32(ReadDisplayedAlpha()), 0);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOdysseyTextureLayerEditAlphaLockTest, "Odyssey.Texture.TiledImage.EditAlphaLock",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

/** While the alpha of a layer is locked, its edits change colors only: the views and the tiles keep the alpha of the layer. */
bool FOdysseyTextureLayerEditAlphaLockTest::RunTest(const FString& Parameters)
{
    using namespace Odyssey::TextureLayerTiledImageTests;
    TStrongObjectPtr<UTexture2D> Texture(MakeTexture(GetTransientPackage(), TSF_BGRA8));
    UOdysseyTextureLayerImageRaster* Layer = MakeLayer(Texture.Get());

    TSharedPtr<FOdysseyRasterBlockEdit> Unlocked = Layer->BeginRasterEdit();
    if (!TestTrue(TEXT("Begin an edit"), Unlocked.IsValid()))
    {
        return false;
    }
    TestFalse(TEXT("An edit of an unlocked layer edits alpha"), Unlocked->GetSession().PreservesAlpha());
    Unlocked->End();

    Layer->SetIsAlphaLocked(true);
    TSharedPtr<FOdysseyRasterBlockEdit> Edit = Layer->BeginRasterEdit();
    if (!TestTrue(TEXT("Begin an edit of the locked layer"), Edit.IsValid()))
    {
        return false;
    }
    TestTrue(TEXT("An edit of a locked layer preserves alpha"), Edit->GetSession().PreservesAlpha());

    // The fixture is opaque at (3, 4) and half transparent at (5, 4). The block holds BGRA8 linear colors.
    const FIntPoint Erased(3, 4);
    const FIntPoint Painted(5, 4);
    uint8* Bits = Edit->GetBlock()->Bits();
    FMemory::Memzero(Bits + (Erased.Y * Width + Erased.X) * 4, 4);
    uint8* PaintedPixel = Bits + (Painted.Y * Width + Painted.X) * 4;
    PaintedPixel[0] = 0;
    PaintedPixel[1] = 0;
    PaintedPixel[2] = 255;
    PaintedPixel[3] = 255;
    Edit->Update({ FIntRect(Erased.X, Erased.Y, Painted.X + 1, Painted.Y + 1) });

    const float Tolerance = 0.5f / 255.0f + 0.0001f;
    TFunction<void(const TCHAR*, const TArray<FLinearColor>&)> CheckPixels = [this, Erased, Painted, Tolerance](const TCHAR* What, const TArray<FLinearColor>& Pixels)
    {
        if (!TestEqual(FString::Printf(TEXT("%s: pixel count"), What), Pixels.Num(), Width * Height))
        {
            return;
        }
        const FLinearColor ErasedColor = Pixels[Erased.Y * Width + Erased.X];
        const FLinearColor PaintedColor = Pixels[Painted.Y * Width + Painted.X];
        TestTrue(FString::Printf(TEXT("%s: the erased pixel keeps its alpha, got %s"), What, *ErasedColor.ToString()),
            ErasedColor.Equals(FLinearColor(0.0f, 0.0f, 0.0f, 1.0f), Tolerance));
        TestTrue(FString::Printf(TEXT("%s: the painted pixel keeps its alpha, got %s"), What, *PaintedColor.ToString()),
            PaintedColor.Equals(FLinearColor(1.0f, 0.0f, 0.0f, 0.5f), Tolerance));
    };
    CheckPixels(TEXT("Uncommitted edit"), ReadPipeline(Layer));
    {
        FScopedTransaction Transaction(NSLOCTEXT("OdysseyTextureLayerTiledImageTests", "EditAlphaLock", "Edit a locked layer"));
        TestTrue(TEXT("Commit the edit"), Edit->Commit());
    }
    CheckPixels(TEXT("Committed tiles"), ReadImage(Layer->GetTiledImage()));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOdysseyTextureLayerContentReadWriteTest, "Odyssey.Texture.TiledImage.ContentReadWrite",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

/**
 * Users other than tools read the content into render targets and write render targets or blocks into it:
 * writes convert colors when asked, are undoable, keep a locked alpha and end the active edit.
 */
bool FOdysseyTextureLayerContentReadWriteTest::RunTest(const FString& Parameters)
{
    using namespace Odyssey::TextureLayerTiledImageTests;
    if (!TestTrue(TEXT("OpenColorIO converts between linear and sRGB encoded colors"), FOdysseyColorSpaceShader::IsAvailable()))
    {
        return false;
    }
    TStrongObjectPtr<UTexture2D> Texture(MakeTexture(GetTransientPackage(), TSF_BGRA8));
    UOdysseyTextureLayerImageRaster* Layer = MakeLayer(Texture.Get());
    FOdysseyRasterContent& Content = Layer->GetRasterContent();
    const float Tolerance = 0.5f / 255.0f + 0.0001f;
    const TArray<FLinearColor> Original = ReadImage(Layer->GetTiledImage());

    // Read into a float render target, which keeps every value when read back as linear colors.
    TStrongObjectPtr<UTextureRenderTarget2D> Target(NewObject<UTextureRenderTarget2D>());
    Target->InitCustomFormat(Width, Height, PF_A32B32G32R32F, true);
    Target->UpdateResourceImmediate(true);
    if (!TestTrue(TEXT("Read the content"), Content.Read(*Target, FIntRect(0, 0, Width, Height), FIntPoint::ZeroValue)))
    {
        return false;
    }
    FlushRenderingCommands();
    TArray<FLinearColor> Read;
    FReadSurfaceDataFlags Flags(RCM_MinMax);
    Flags.SetLinearToGamma(false);
    Target->GameThread_GetRenderTargetResource()->ReadLinearColorPixels(Read, Flags);
    ComparePixels(*this, TEXT("The content is read as it is held"), Original, Read, 0.0001f);

    // Write encoded colors, decoded into the linear colors of the content; the active edit ends.
    TSharedPtr<FOdysseyRasterBlockEdit> Edit = Layer->BeginRasterEdit();
    TStrongObjectPtr<UTextureRenderTarget2D> Encoded(NewObject<UTextureRenderTarget2D>());
    Encoded->ClearColor = FLinearColor(0.5f, 0.5f, 0.5f, 1.0f);
    Encoded->InitCustomFormat(Width, Height, PF_A32B32G32R32F, true);
    Encoded->UpdateResourceImmediate(true);
    const FIntRect Written(0, 0, 10, 10);
    {
        FScopedTransaction Transaction(NSLOCTEXT("OdysseyTextureLayerTiledImageTests", "Write", "Write content"));
        TestTrue(TEXT("Write a render target"), Content.Write(*Encoded, { Written }, EOdysseyColorConversion::Decode));
    }
    TestFalse(TEXT("A write ends the active edit"), Edit.IsValid() && Edit->IsActive());
    const float Decoded = FLinearColor::FromSRGBColor(FColor(128, 128, 128)).R;
    TArray<FLinearColor> Tiles = ReadImage(Layer->GetTiledImage());
    if (TestEqual(TEXT("Pixel count"), Tiles.Num(), Width * Height))
    {
        TestTrue(FString::Printf(TEXT("Written colors are decoded, got %s"), *Tiles[5 * Width + 5].ToString()),
            Tiles[5 * Width + 5].Equals(FLinearColor(Decoded, Decoded, Decoded, 1.0f), Tolerance));
        TestTrue(TEXT("Pixels outside the written areas are kept"), Tiles[20 * Width + 20].Equals(Original[20 * Width + 20], 0.0001f));
    }
    TestTrue(TEXT("Undo the write"), GEditor->UndoTransaction());
    ComparePixels(*this, TEXT("Undone write"), Original, ReadImage(Layer->GetTiledImage()), 0.0001f);

    // A locked alpha is kept: writing transparent pixels only changes colors.
    Layer->SetIsAlphaLocked(true);
    TStrongObjectPtr<UTextureRenderTarget2D> Transparent(Content.CreateRenderTarget());
    TestTrue(TEXT("Write transparent pixels"), Content.Write(*Transparent, { Written }));
    Tiles = ReadImage(Layer->GetTiledImage());
    if (TestEqual(TEXT("Pixel count"), Tiles.Num(), Width * Height))
    {
        TestEqual(TEXT("A locked alpha is kept"), Tiles[5 * Width + 5].A, Original[5 * Width + 5].A, Tolerance);
        TestTrue(TEXT("Colors are written"), Tiles[5 * Width + 5].R < Tolerance && Tiles[5 * Width + 5].G < Tolerance);
    }
    Layer->SetIsAlphaLocked(false);

    // Blocks are converted to the format the layer expects.
    ULIS::FBlock Gray(Width, Height, ULIS::Format_RGBAF);
    for (int32 Index = 0; Index < Width * Height; ++Index)
    {
        float* Pixel = reinterpret_cast<float*>(Gray.Bits()) + Index * 4;
        Pixel[0] = 0.25f;
        Pixel[1] = 0.25f;
        Pixel[2] = 0.25f;
        Pixel[3] = 1.0f;
    }
    TestTrue(TEXT("Write a block of another format"), Content.WriteBlock(Gray, { Written }));
    // The conversion of the block to 8 bits truncates values, as the raster block did.
    const float TruncationTolerance = 1.0f / 255.0f + 0.0001f;
    Tiles = ReadImage(Layer->GetTiledImage());
    if (TestEqual(TEXT("Pixel count"), Tiles.Num(), Width * Height))
    {
        TestTrue(FString::Printf(TEXT("Block colors are written, got %s"), *Tiles[5 * Width + 5].ToString()),
            Tiles[5 * Width + 5].Equals(FLinearColor(0.25f, 0.25f, 0.25f, 1.0f), TruncationTolerance));
    }
    TSharedPtr<ULIS::FBlock> Block = Content.ReadBlock();
    TestTrue(TEXT("Blocks are read in the format the layer expects"), Block.IsValid() && Block->Format() == Content.Describe().Format);
    return true;
}

#endif
