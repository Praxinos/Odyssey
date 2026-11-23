// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#if WITH_DEV_AUTOMATION_TESTS
#include "Editor.h"
#include "Editor/Transactor.h"
#include "EditorValidatorSubsystem.h"
#include "Engine/Texture2D.h"
#include "HAL/FileManager.h"
#include "Misc/AutomationTest.h"
#include "Misc/PackageName.h"
#include "Misc/Paths.h"
#include "Misc/ScopeExit.h"
#include "ScopedTransaction.h"
#include "TextureCompiler.h"
#include "UObject/Linker.h"
#include "UObject/Package.h"
#include "UObject/SavePackage.h"
#include "UObject/StrongObjectPtr.h"

#include "OdysseyAnimation.h"
#include "OdysseyAnimationCellImageRaster.h"
#include "OdysseyAnimationCellImageVector.h"
#include "OdysseyAnimationEditorData.h"
#include "OdysseyAnimationLayerImageRaster.h"
#include "OdysseyAnimationLayerImageVector.h"
#include "OdysseyAnimationLayerStackUserData.h"
#include "OdysseyRasterContentBlocks.h"
#include "OdysseyRenderingType.h"
#include "OdysseyTiledImage.h"
#include "OdysseyVectorCell.h"
#include "OdysseyVectorContent.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorLayer.h"

namespace Odyssey::AnimationTiledTests
{
TArray<FLinearColor> Render(UOdysseyAnimation* Animation, int32 Frame, uint64 Type);
bool ComparePixels(FAutomationTestBase& Test, const FString& What, const TArray<FLinearColor>& Expected, const TArray<FLinearColor>& Actual, float Tolerance);
}

namespace Odyssey::AnimationVectorTests
{
/** The size of the renders of Odyssey::AnimationTiledTests::Render(). */
constexpr int32 Width = 73;
constexpr int32 Height = 67;
constexpr EAutomationTestFlags Flags = EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter;
/** The colors the scene fills the canvas with; asymmetric, so that swapped channels are detected. */
const FColor Fill(200, 40, 120, 255);
const FColor OtherFill(30, 180, 90, 255);

/** Creates an animation with one layer of the given class, holding one cell of the given class exposed on two frames. */
UOdysseyAnimation* MakeAnimation(UObject* Outer, EOdysseyAnimationFormat Format, UClass* LayerClass, UClass* CellClass)
{
    UOdysseyAnimation* Animation = NewObject<UOdysseyAnimation>(Outer, NAME_None, RF_Public | RF_Transactional);
    Animation->Init(Width, Height, Format, 24.0f);
    UOdysseyAnimationLayerStack* LayerStack = OdysseyAnimationEditor::GetLayerStack(Animation);
    UOdysseyAnimationLayer* Layer = CastChecked<UOdysseyAnimationLayer>(LayerStack->AddLayer(LayerClass));
    LayerStack->SetCurrentLayer(Layer);
    Layer->AddCell(CellClass)->SetExposure(2);
    return Animation;
}

UOdysseyAnimationCell* FirstCell(const UOdysseyAnimation* Animation)
{
    return CastChecked<UOdysseyAnimationLayer>(OdysseyAnimationEditor::GetLayerStack(Animation)->GetRootLayers()[0])->GetCells()[0];
}

UOdysseyAnimationCellImageVector* FirstVectorCell(const UOdysseyAnimation* Animation)
{
    return CastChecked<UOdysseyAnimationCellImageVector>(FirstCell(Animation));
}

/** Sets the color of the background of the scene of a cell, which fills the canvas, and redraws the cell. */
void SetSceneColor(UOdysseyAnimationCellImageVector* Cell, const FColor& Color)
{
    FOdysseyVectorCell* VectorCell = Cell->GetVectorCell();
    VectorCell->GetScene()->GetBackgroundBucket().SetSolidColor(Color.R, Color.G, Color.B, Color.A);
    VectorCell->InvalidateRect();
    VectorCell->GetLayer()->RequestRedraw(VectorCell, 0);
}

/** Returns the linear color an animation renders for an 8-bit color of the render: decoded in 8-bit animations, as it is in float ones. */
FLinearColor Linear(const FColor& Color, EOdysseyAnimationFormat Format)
{
    return Format == EOdysseyAnimationFormat::BGRA8 ? FLinearColor::FromSRGBColor(Color) : Color.ReinterpretAsLinear();
}

/** The precision of the tiles: half floats for decoded colors, bytes otherwise. */
float Tolerance(EOdysseyAnimationFormat Format)
{
    return Format == EOdysseyAnimationFormat::BGRA8 ? 0.002f : 0.5f / 255.0f + 0.0001f;
}

const TCHAR* FormatName(EOdysseyAnimationFormat Format)
{
    return Format == EOdysseyAnimationFormat::BGRA8 ? TEXT("8-bit animation") : TEXT("float animation");
}

/** Checks that every pixel of a render has the given color. */
bool CheckUniform(FAutomationTestBase& Test, const FString& What, const TArray<FLinearColor>& Pixels, const FLinearColor& Expected, float InTolerance)
{
    if (!Test.TestEqual(What + TEXT(": pixel count"), Pixels.Num(), Width * Height))
    {
        return false;
    }
    for (int32 Index = 0; Index < Pixels.Num(); ++Index)
    {
        if (!Pixels[Index].Equals(Expected, InTolerance))
        {
            Test.AddError(FString::Printf(TEXT("%s: pixel (%d, %d) expected %s, got %s"), *What, Index % Width, Index / Width,
                *Expected.ToString(), *Pixels[Index].ToString()));
            return false;
        }
    }
    return true;
}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOdysseyAnimationVectorRenderTest, "Odyssey.Animation.Vector.Render", Odyssey::AnimationVectorTests::Flags)

/**
 * A vector cell renders its scene into a transient tiled image when its animation is first displayed,
 * applies its out of pegs transform as a raster cell does, and a duplicate renders from its own scene.
 */
bool FOdysseyAnimationVectorRenderTest::RunTest(const FString& Parameters)
{
    using namespace Odyssey::AnimationVectorTests;
    using Odyssey::AnimationTiledTests::Render;
    for (const EOdysseyAnimationFormat Format : { EOdysseyAnimationFormat::BGRA8, EOdysseyAnimationFormat::RGBAF })
    {
        const FString Name = FormatName(Format);
        const float Precision = Tolerance(Format);
        TStrongObjectPtr<UOdysseyAnimation> Animation(MakeAnimation(GetTransientPackage(), Format,
            UOdysseyAnimationLayerImageVector::StaticClass(), UOdysseyAnimationCellImageVector::StaticClass()));
        UOdysseyAnimationCellImageVector* Cell = FirstVectorCell(Animation.Get());
        SetSceneColor(Cell, Fill);
        TestNull(Name + TEXT(": the scene is not rendered before it is displayed"), Cell->GetVectorContent().GetImage());

        const TArray<FLinearColor> Plain = Render(Animation.Get(), 0, EOdysseyRenderingType::Render);
        CheckUniform(*this, Name + TEXT(": the animation shows the scene of its vector cell"), Plain, Linear(Fill, Format), Precision);
        CheckUniform(*this, Name + TEXT(": the cell is shown on its second frame"), Render(Animation.Get(), 1, EOdysseyRenderingType::Render), Linear(Fill, Format), Precision);
        const UOdysseyTiledImage* Image = Cell->GetVectorContent().GetImage();
        if (!TestNotNull(Name + TEXT(": the render is held by a tiled image"), Image))
        {
            return false;
        }
        TestEqual(Name + TEXT(": the image is not a subobject of the cell"), Image->GetOuter(), static_cast<UObject*>(GetTransientPackage()));
        TestTrue(Name + TEXT(": the image is never saved"), Image->HasAnyFlags(RF_Transient));
        TestFalse(Name + TEXT(": the tiles are not undoable, an undo redraws the scene"), Image->HasAnyFlags(RF_Transactional));
        TestEqual(Name + TEXT(": the tiles hold the colors the animation renders"), Image->GetFormat(),
            Format == EOdysseyAnimationFormat::BGRA8 ? EOdysseyTiledImageFormat::RGBA16F : EOdysseyTiledImageFormat::RGBA8);

        // A raster cell holding the same pixels is the reference of the out of pegs transform.
        TStrongObjectPtr<UOdysseyAnimation> RasterAnimation(MakeAnimation(GetTransientPackage(), Format,
            UOdysseyAnimationLayerImageRaster::StaticClass(), UOdysseyAnimationCellImageRaster::StaticClass()));
        UOdysseyAnimationCellImageRaster* RasterCell = CastChecked<UOdysseyAnimationCellImageRaster>(FirstCell(RasterAnimation.Get()));
        Odyssey::RasterContentBlocks::EditBlock(RasterCell->GetRasterContent(), [](ULIS::FBlock& Block)
        {
            for (int32 Y = 0; Y < Height; ++Y)
            {
                for (int32 X = 0; X < Width; ++X)
                {
                    Block.SetPixel(X, Y, ULIS::FColor(ULIS::Format_RGBA8, { Fill.R, Fill.G, Fill.B, Fill.A }));
                }
            }
        });
        FOdysseyAnimationCellOutOfPegs Pegs;
        Pegs.Pan = FVector2D(7, -4);
        Pegs.Rotation = 17;
        Pegs.Zoom = 80;
        Cell->SetOutOfPegs(Pegs);
        RasterCell->SetOutOfPegs(Pegs);
        const uint64 OutOfPegs = EOdysseyRenderingType::Editor | EOdysseyRenderingType::OutOfPegs;
        const TArray<FLinearColor> Moved = Render(Animation.Get(), 0, OutOfPegs);
        Odyssey::AnimationTiledTests::ComparePixels(*this, Name + TEXT(": the vector cell is moved out of pegs as a raster cell"),
            Render(RasterAnimation.Get(), 0, OutOfPegs), Moved, Precision);
        TestFalse(Name + TEXT(": the out of pegs transform changes the render"), Moved == Plain);

        TStrongObjectPtr<UOdysseyAnimation> Duplicate(DuplicateObject<UOdysseyAnimation>(Animation.Get(), GetTransientPackage()));
        UOdysseyAnimationCellImageVector* DuplicateCell = FirstVectorCell(Duplicate.Get());
        TestNull(Name + TEXT(": the render is not duplicated"), DuplicateCell->GetVectorContent().GetImage());
        TestTrue(Name + TEXT(": the duplicate has its own scene"), DuplicateCell->GetVectorCell() != Cell->GetVectorCell());
        CheckUniform(*this, Name + TEXT(": the duplicate shows the duplicated scene"), Render(Duplicate.Get(), 0, EOdysseyRenderingType::Render), Linear(Fill, Format), Precision);
        TestTrue(Name + TEXT(": the duplicate renders into its own image"), DuplicateCell->GetVectorContent().GetImage() && DuplicateCell->GetVectorContent().GetImage() != Image);
        SetSceneColor(DuplicateCell, OtherFill);
        CheckUniform(*this, Name + TEXT(": the duplicate shows the edits of its scene"), Render(Duplicate.Get(), 0, EOdysseyRenderingType::Render), Linear(OtherFill, Format), Precision);
        CheckUniform(*this, Name + TEXT(": the original is not changed by the edits of the duplicate"), Render(Animation.Get(), 0, EOdysseyRenderingType::Render), Linear(Fill, Format), Precision);
    }
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOdysseyAnimationVectorUndoTest, "Odyssey.Animation.Vector.UndoClear", Odyssey::AnimationVectorTests::Flags)

/**
 * Undoing the clear of a vector cell shows the previous render, whether the clear was displayed after its transaction,
 * or within it. The tiles are not recorded in the transaction: the undo redraws the scene.
 */
bool FOdysseyAnimationVectorUndoTest::RunTest(const FString& Parameters)
{
    using namespace Odyssey::AnimationVectorTests;
    using Odyssey::AnimationTiledTests::Render;
    const EOdysseyAnimationFormat Format = EOdysseyAnimationFormat::BGRA8;
    const float Precision = Tolerance(Format);
    TStrongObjectPtr<UOdysseyAnimation> Animation(MakeAnimation(GetTransientPackage(), Format,
        UOdysseyAnimationLayerImageVector::StaticClass(), UOdysseyAnimationCellImageVector::StaticClass()));
    UOdysseyAnimationCellImageVector* Cell = FirstVectorCell(Animation.Get());
    SetSceneColor(Cell, Fill);
    CheckUniform(*this, TEXT("The scene is rendered"), Render(Animation.Get(), 0, EOdysseyRenderingType::Render), Linear(Fill, Format), Precision);

    {
        // The clear is undoable within a transaction.
        FScopedTransaction Transaction(NSLOCTEXT("OdysseyAnimationVectorTests", "Clear", "Clear the cell"));
        Cell->Clear();
    }
    CheckUniform(*this, TEXT("The cleared cell renders nothing"), Render(Animation.Get(), 0, EOdysseyRenderingType::Render), FLinearColor::Transparent, Precision);
    TestTrue(TEXT("Undo the clear"), GEditor->UndoTransaction());
    CheckUniform(*this, TEXT("The undone clear shows the previous render"), Render(Animation.Get(), 0, EOdysseyRenderingType::Render), Linear(Fill, Format), Precision);
    TestTrue(TEXT("Redo the clear"), GEditor->RedoTransaction());
    CheckUniform(*this, TEXT("The redone clear renders nothing"), Render(Animation.Get(), 0, EOdysseyRenderingType::Render), FLinearColor::Transparent, Precision);
    TestTrue(TEXT("Undo the clear again"), GEditor->UndoTransaction());
    CheckUniform(*this, TEXT("The clear undone again shows the previous render"), Render(Animation.Get(), 0, EOdysseyRenderingType::Render), Linear(Fill, Format), Precision);

    {
        FScopedTransaction Transaction(NSLOCTEXT("OdysseyAnimationVectorTests", "ClearDisplayed", "Clear and display the cell"));
        Cell->Clear();
        CheckUniform(*this, TEXT("The clear displayed within its transaction renders nothing"), Render(Animation.Get(), 0, EOdysseyRenderingType::Render), FLinearColor::Transparent, Precision);
    }
    const FTransaction* Displayed = GEditor->Trans->GetTransaction(GEditor->Trans->GetQueueLength() - 1);
    TestFalse(TEXT("The transaction does not record the tiles"), Displayed && Displayed->ContainsObject(Cell->GetVectorContent().GetImage()));
    TestTrue(TEXT("Undo the displayed clear"), GEditor->UndoTransaction());
    CheckUniform(*this, TEXT("The undone displayed clear shows the previous render"), Render(Animation.Get(), 0, EOdysseyRenderingType::Render), Linear(Fill, Format), Precision);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOdysseyAnimationVectorUndoCellsTest, "Odyssey.Animation.Vector.UndoCells", Odyssey::AnimationVectorTests::Flags)

/**
 * Undoing a change of the cells of a vector layer, e.g. in the timeline, gives its vector layer the restored cells,
 * from which the inbetweens of the cells are drawn.
 */
bool FOdysseyAnimationVectorUndoCellsTest::RunTest(const FString& Parameters)
{
    using namespace Odyssey::AnimationVectorTests;
    TStrongObjectPtr<UOdysseyAnimation> Animation(MakeAnimation(GetTransientPackage(), EOdysseyAnimationFormat::BGRA8,
        UOdysseyAnimationLayerImageVector::StaticClass(), UOdysseyAnimationCellImageVector::StaticClass()));
    UOdysseyAnimationCellImageVector* First = FirstVectorCell(Animation.Get());
    UOdysseyAnimationLayerImageVector* Layer = CastChecked<UOdysseyAnimationLayerImageVector>(First->GetLayer());
    TSharedPtr<FOdysseyVectorLayer> VectorLayer = Layer->GetVectorLayer();

    UOdysseyAnimationCellImageVector* Added = nullptr;
    {
        FScopedTransaction Transaction(NSLOCTEXT("OdysseyAnimationVectorTests", "AddCell", "Add a cell"));
        Layer->Modify();
        Added = Cast<UOdysseyAnimationCellImageVector>(Layer->AddCell(UOdysseyAnimationCellImageVector::StaticClass()));
    }
    if (!TestNotNull(TEXT("A vector cell is added"), Added))
    {
        return false;
    }
    TestTrue(TEXT("The vector layer holds the added cell"), VectorLayer->Contains(Added->GetVectorCell()));

    TestTrue(TEXT("Undo the added cell"), GEditor->UndoTransaction());
    TestEqual(TEXT("The layer holds its cell again"), Layer->GetCells().Num(), 1);
    TestFalse(TEXT("The vector layer no longer holds the removed cell"), VectorLayer->Contains(Added->GetVectorCell()));
    TestTrue(TEXT("The vector layer still holds the remaining cell"), VectorLayer->Contains(First->GetVectorCell()));

    TestTrue(TEXT("Redo the added cell"), GEditor->RedoTransaction());
    TestEqual(TEXT("The layer holds both cells"), Layer->GetCells().Num(), 2);
    TestTrue(TEXT("The vector layer holds the restored cell"), VectorLayer->Contains(Added->GetVectorCell()));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOdysseyAnimationVectorPersistenceTest, "Odyssey.Animation.Vector.Persistence", Odyssey::AnimationVectorTests::Flags)

/**
 * The scene of a vector cell is saved, not its render: a loaded cell renders its scene when displayed, without dirtying its package.
 * The render texture of a cell saved before the tiled image held the render still loads, and is no longer saved.
 */
bool FOdysseyAnimationVectorPersistenceTest::RunTest(const FString& Parameters)
{
    using namespace Odyssey::AnimationVectorTests;
    using Odyssey::AnimationTiledTests::Render;
    FScopedDisableValidateOnSave DisableValidation;
    const EOdysseyAnimationFormat Format = EOdysseyAnimationFormat::BGRA8;
    const float Precision = Tolerance(Format);
    const FString Id = FGuid::NewGuid().ToString(EGuidFormats::Digits);
    const FString Mount = TEXT("/OdysseyAnimationVectorTest/");
    const FString Directory = FPaths::ConvertRelativePathToFull(FPaths::ProjectPluginsDir() / TEXT("Odyssey-Plugin/Intermediate/OdysseyAnimationTests/"));
    FPackageName::RegisterMountPoint(Mount, Directory + TEXT("/"));
    ON_SCOPE_EXIT { FPackageName::UnRegisterMountPoint(Mount, Directory + TEXT("/")); };
    const FString PackageName = Mount + Id;
    const FString Filename = Directory / (Id + TEXT(".uasset"));
    IFileManager::Get().MakeDirectory(*Directory, true);
    ON_SCOPE_EXIT { IFileManager::Get().Delete(*Filename); };
    FSavePackageArgs Args;
    Args.TopLevelFlags = RF_Public;
    Args.SaveFlags = SAVE_NoError;

    UPackage* Package = CreatePackage(*PackageName);
    TStrongObjectPtr<UOdysseyAnimation> Animation(MakeAnimation(Package, Format,
        UOdysseyAnimationLayerImageVector::StaticClass(), UOdysseyAnimationCellImageVector::StaticClass()));
    UOdysseyAnimationCellImageVector* Cell = FirstVectorCell(Animation.Get());
    SetSceneColor(Cell, Fill);
    CheckUniform(*this, TEXT("The scene is rendered before saving"), Render(Animation.Get(), 0, EOdysseyRenderingType::Render), Linear(Fill, Format), Precision);
    {
        // Public, so that the save writes it as the former render texture was: referenced by a property that no longer exists.
        UTexture2D* LegacyTexture = NewObject<UTexture2D>(Cell, TEXT("Texture"), RF_Public);
        TArray<FColor> LegacyPixels;
        LegacyPixels.Init(Fill, 4 * 4);
        LegacyTexture->Source.Init(4, 4, 1, 1, TSF_BGRA8, reinterpret_cast<const uint8*>(LegacyPixels.GetData()));
        LegacyTexture->MipGenSettings = TMGS_NoMipmaps;
        LegacyTexture->UpdateResource();
        FTextureCompilingManager::Get().FinishCompilation({ LegacyTexture });
    }
    const FString AssetName = Animation->GetName();
    if (!TestTrue(TEXT("Save the animation"), UPackage::SavePackage(Package, Animation.Get(), *Filename, Args)))
    {
        return false;
    }
    // Keep the original alive under another name to force a real disk load.
    Package->Rename(*(PackageName + TEXT("_Original")), nullptr, REN_DontCreateRedirectors | REN_NonTransactional);

    UPackage* LoadedPackage = LoadPackage(nullptr, *PackageName, LOAD_None);
    UOdysseyAnimation* Loaded = LoadedPackage ? FindObject<UOdysseyAnimation>(LoadedPackage, *AssetName) : nullptr;
    if (!TestNotNull(TEXT("Load the animation"), Loaded))
    {
        return false;
    }
    UOdysseyAnimationCellImageVector* LoadedCell = FirstVectorCell(Loaded);
    UTexture2D* LoadedLegacyTexture = FindObject<UTexture2D>(LoadedCell, TEXT("Texture"));
    TestNotNull(TEXT("The former render texture is loaded with the cell"), LoadedLegacyTexture);
    TestNull(TEXT("The render is not saved"), LoadedCell->GetVectorContent().GetImage());
    // Loading the cell requests a redraw, which tells the layer stack: it must not dirty the package while loading.
    TestFalse(TEXT("Loading does not dirty the package"), LoadedPackage->IsDirty());
    LoadedPackage->SetDirtyFlag(false);

    CheckUniform(*this, TEXT("The loaded cell renders its saved scene"), Render(Loaded, 0, EOdysseyRenderingType::Render), Linear(Fill, Format), Precision);
    TestNotNull(TEXT("The loaded cell renders into a tiled image"), LoadedCell->GetVectorContent().GetImage());
    TestFalse(TEXT("Displaying the loaded vector cell does not dirty its package"), LoadedPackage->IsDirty());
    // Check that this package can be dirtied, so that the check above is meaningful.
    LoadedCell->Modify();
    TestTrue(TEXT("Editing the loaded cell dirties its package"), LoadedPackage->IsDirty());
    LoadedPackage->SetDirtyFlag(false);

    // Once loaded, nothing references the former render texture: the next save drops it.
    if (LoadedLegacyTexture)
    {
        LoadedLegacyTexture->ClearFlags(RF_Public);
    }
    if (!TestTrue(TEXT("Save the loaded animation"), UPackage::SavePackage(LoadedPackage, Loaded, *Filename, Args)))
    {
        return false;
    }
    LoadedPackage->Rename(*(PackageName + TEXT("_Loaded")), nullptr, REN_DontCreateRedirectors | REN_NonTransactional);
    UPackage* ReloadedPackage = LoadPackage(nullptr, *PackageName, LOAD_None);
    // Release the file before it is deleted.
    ON_SCOPE_EXIT { if (ReloadedPackage) { ResetLoaders(ReloadedPackage); } };
    UOdysseyAnimation* Reloaded = ReloadedPackage ? FindObject<UOdysseyAnimation>(ReloadedPackage, *AssetName) : nullptr;
    if (!TestNotNull(TEXT("Reload the animation"), Reloaded))
    {
        return false;
    }
    TestNull(TEXT("The former render texture is not saved again"), FindObject<UTexture2D>(FirstVectorCell(Reloaded), TEXT("Texture")));
    CheckUniform(*this, TEXT("The resaved cell renders its scene"), Render(Reloaded, 0, EOdysseyRenderingType::Render), Linear(Fill, Format), Precision);
    return true;
}

#endif
