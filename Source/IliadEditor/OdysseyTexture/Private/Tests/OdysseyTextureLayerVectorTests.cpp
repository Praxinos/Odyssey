// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#if WITH_DEV_AUTOMATION_TESTS
#include "Editor.h"
#include "Editor/Transactor.h"
#include "EditorValidatorSubsystem.h"
#include "Engine/Texture2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "HAL/FileManager.h"
#include "ImageCore.h"
#include "Misc/AutomationTest.h"
#include "Misc/PackageName.h"
#include "Misc/Paths.h"
#include "Misc/ScopeExit.h"
#include "RenderGraphBuilder.h"
#include "RenderGraphUtils.h"
#include "RenderingThread.h"
#include "ScopedTransaction.h"
#include "TextureCompiler.h"
#include "TextureResource.h"
#include "UObject/Linker.h"
#include "UObject/Package.h"
#include "UObject/SavePackage.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/UObjectGlobals.h"
#include "UObject/UObjectHash.h"
#include "UObject/UObjectIterator.h"
#include "UObject/UnrealType.h"

#include "OdysseyRasterEditSession.h"
#include "OdysseyRenderContext.h"
#include "OdysseyRenderingAbility.h"
#include "OdysseyTextureLayerImageVector.h"
#include "OdysseyTextureLayerStack.h"
#include "OdysseyTextureLayerStackUserData.h"
#include "OdysseyTiledImage.h"
#include "OdysseyVectorCell.h"
#include "OdysseyVectorContent.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorLayer.h"
#include "Undo/OdysseyVectorUndoSceneClear.h"

namespace Odyssey::TextureLayerVectorTests
{
constexpr int32 Width = 73;
constexpr int32 Height = 67;
constexpr EAutomationTestFlags Flags = EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter;
/** The colors the scene fills the canvas with; asymmetric, so that swapped channels are detected. */
const FColor Fill(200, 40, 120, 255);
const FColor OtherFill(30, 180, 90, 255);
/** The color an uncolored scene is drawn with. */
const FColor Monochrome(160, 160, 160, 255);

/** Creates an opaque 8-bit texture holding a pattern, with its layer stack updated manually. */
UTexture2D* MakeTexture(UObject* Outer, bool bSRGB)
{
    UTexture2D* Texture = NewObject<UTexture2D>(Outer, NAME_None, RF_Public | RF_Transactional);
    FImage Image(Width, Height, ERawImageFormat::BGRA8, bSRGB ? EGammaSpace::sRGB : EGammaSpace::Linear);
    for (int32 Index = 0; Index < Width * Height; ++Index)
    {
        Image.AsBGRA8()[Index] = FColor(uint8(Index % 251), uint8((Index % 7) * 30), 60, 255);
    }
    Texture->Source.Init(Image);
    Texture->SRGB = bSRGB;
    Texture->MipGenSettings = TMGS_NoMipmaps;
    Texture->UpdateResource();
    FTextureCompilingManager::Get().FinishCompilation({ Texture });
    UOdysseyTextureLayerStackUserData* UserData = NewObject<UOdysseyTextureLayerStackUserData>(Texture);
    Texture->AddAssetUserData(UserData);
    UserData->InitWithDefaultLayerStack();
    UserData->LayerStack->SetTextureUpdateMode(EOdysseyTextureLayerStackTextureUpdateMode::Manually);
    return Texture;
}

UOdysseyTextureLayerStack* GetStack(UTexture2D* Texture)
{
    return CastChecked<UOdysseyTextureLayerStackUserData>(Texture->GetAssetUserDataOfClass(UOdysseyTextureLayerStackUserData::StaticClass()))->LayerStack;
}

UOdysseyTextureLayerImageVector* FindVectorLayer(UOdysseyTextureLayerStack* Stack)
{
    for (UOdysseyLayer* Layer : Stack->GetLayers())
    {
        if (UOdysseyTextureLayerImageVector* VectorLayer = Cast<UOdysseyTextureLayerImageVector>(Layer))
        {
            return VectorLayer;
        }
    }
    return nullptr;
}

UOdysseyTextureLayerImageVector* AddVectorLayer(UOdysseyTextureLayerStack* Stack)
{
    return CastChecked<UOdysseyTextureLayerImageVector>(Stack->AddLayer(UOdysseyTextureLayerImageVector::StaticClass()));
}

/**
 * Sets the color of the background of the scene, which fills the canvas when the scene is colored.
 * @param bRedraw False to change the scene without telling the layer, so that only a render of the whole canvas shows the change.
 */
void SetSceneColor(UOdysseyTextureLayerImageVector* Layer, const FColor& Color, bool bRedraw = true)
{
    FOdysseyVectorCell* Cell = Layer->GetVectorCell().Get();
    Cell->GetScene()->GetBackgroundBucket().SetSolidColor(Color.R, Color.G, Color.B, Color.A);
    if (bRedraw)
    {
        Cell->InvalidateRect();
        Cell->GetLayer()->RequestRedraw(Cell, 0);
    }
}

/** Clears the scene as an undoable vector edit, as the clear of a vector cell does. Needs a transaction. */
void ClearScene(UOdysseyTextureLayerImageVector* Layer)
{
    FOdysseyVectorCell* Cell = Layer->GetVectorCell().Get();
    GUndo->StoreUndo(GEditor, MakeUnique<FOdysseyVectorUndoSceneClear>(Cell->GetScene()));
    Cell->SetScene(new FOdysseyVectorGroupPaint(TEXT("Scene")));
    Cell->GetLayer()->RequestRedraw(Cell, 0);
    Cell->GetLayer()->Update(FOdysseyVectorObject::UPDATE_PAINTGROUPS);
}

/** Returns the linear color the layers hold for an 8-bit color of the render: decoded when the texture is sRGB. */
FLinearColor Linear(const FColor& Color, bool bSRGB)
{
    return bSRGB ? FLinearColor::FromSRGBColor(Color) : Color.ReinterpretAsLinear();
}

/** The precision of the tiles: half floats for decoded colors, bytes otherwise. */
float Tolerance(bool bSRGB)
{
    return bSRGB ? 0.002f : 0.5f / 255.0f + 0.0001f;
}

/** Returns the color the layer stack renders for an opaque 8-bit color of the render: the bytes of the texture, as it blends encoded colors by default. */
FLinearColor Stacked(const FColor& Color)
{
    return Color.ReinterpretAsLinear();
}

/** The precision of the stack renders: an sRGB stack decodes its layers into half floats, and encodes them back. */
float StackTolerance(bool bSRGB)
{
    return bSRGB ? 0.003f : 0.5f / 255.0f + 0.0001f;
}

/**
 * Renders the pipeline of a node into a new float render target and reads it back.
 * @param bShowSessions True to render as a view does, showing the active edit sessions.
 */
TArray<FLinearColor> ReadNode(const IOdysseyTextureRenderingAbility* Node, FIntPoint Size = FIntPoint(Width, Height), bool bShowSessions = true)
{
    FOdysseyRenderContext Context;
    if (bShowSessions)
    {
        FOdysseyRasterEditSession::AddOverrides(Context);
    }
    IOdysseyTextureRenderingAbility::FRenderFunction Draw;
    if (!Node->BuildRenderPipeline(Draw, {}, &Context))
    {
        return {};
    }
    TStrongObjectPtr<UTextureRenderTarget2D> Target(NewObject<UTextureRenderTarget2D>());
    Target->InitCustomFormat(Size.X, Size.Y, PF_A32B32G32R32F, true);
    Target->UpdateResourceImmediate(true);
    const FIntRect Rect(FIntPoint::ZeroValue, Size);
    ENQUEUE_RENDER_COMMAND(OdysseyTextureVectorTestRender)(
        [Target, Draw = MoveTemp(Draw), Rect](FRHICommandListImmediate& RHICmdList)
        {
            FRDGBuilder Graph(RHICmdList);
            FRDGTextureRef Destination = Target->GetRenderTargetResource()->GetRenderTargetTexture(Graph);
            AddClearRenderTargetPass(Graph, Destination, FLinearColor::Transparent);
            Draw(Graph, GMaxRHIFeatureLevel, Destination, Rect, Rect, FMatrix::Identity);
            Graph.Execute();
        });
    FlushRenderingCommands();
    TArray<FLinearColor> Pixels;
    FReadSurfaceDataFlags ReadFlags(RCM_MinMax);
    ReadFlags.SetLinearToGamma(false);
    Target->GameThread_GetRenderTargetResource()->ReadLinearColorPixels(Pixels, ReadFlags);
    return Pixels;
}

/** Reads the tiles of an image directly, without the render path of the layer. */
TArray<FLinearColor> ReadImage(const UOdysseyTiledImage* Image)
{
    TStrongObjectPtr<UTextureRenderTarget2D> Target(NewObject<UTextureRenderTarget2D>());
    Target->InitCustomFormat(Width, Height, PF_A32B32G32R32F, true);
    Target->UpdateResourceImmediate(true);
    Image->Render(Target.Get(), FIntRect(0, 0, Width, Height), FIntPoint::ZeroValue);
    FlushRenderingCommands();
    TArray<FLinearColor> Pixels;
    FReadSurfaceDataFlags ReadFlags(RCM_MinMax);
    ReadFlags.SetLinearToGamma(false);
    Target->GameThread_GetRenderTargetResource()->ReadLinearColorPixels(Pixels, ReadFlags);
    return Pixels;
}

/** Checks that every pixel of a render has the given color. */
bool CheckUniform(FAutomationTestBase& Test, const FString& What, const TArray<FLinearColor>& Pixels, const FLinearColor& Expected, float InTolerance,
    FIntPoint Size = FIntPoint(Width, Height))
{
    if (!Test.TestEqual(What + TEXT(": pixel count"), Pixels.Num(), Size.X * Size.Y))
    {
        return false;
    }
    for (int32 Index = 0; Index < Pixels.Num(); ++Index)
    {
        if (!Pixels[Index].Equals(Expected, InTolerance))
        {
            Test.AddError(FString::Printf(TEXT("%s: pixel (%d, %d) expected %s, got %s"), *What, Index % Size.X, Index / Size.X,
                *Expected.ToString(), *Pixels[Index].ToString()));
            return false;
        }
    }
    return true;
}

/** Checks the color of a pixel of a render of the default size. */
void CheckPixel(FAutomationTestBase& Test, const FString& What, const TArray<FLinearColor>& Pixels, FIntPoint Pixel, const FLinearColor& Expected, float InTolerance)
{
    if (!Test.TestEqual(What + TEXT(": pixel count"), Pixels.Num(), Width * Height))
    {
        return;
    }
    const FLinearColor& Actual = Pixels[Pixel.Y * Width + Pixel.X];
    Test.TestTrue(FString::Printf(TEXT("%s: pixel (%d, %d) expected %s, got %s"), *What, Pixel.X, Pixel.Y, *Expected.ToString(), *Actual.ToString()),
        Actual.Equals(Expected, InTolerance));
}

/** Renders the layer stack into the texture source and returns its bytes. */
TArray<FColor> ReadTextureBytes(UTexture2D* Texture)
{
    GetStack(Texture)->UpdateTexture(true);
    FImage Image;
    Texture->Source.GetMipImage(Image, 0);
    return TArray<FColor>(Image.AsBGRA8().GetData(), Image.AsBGRA8().Num());
}

bool CheckUniformBytes(FAutomationTestBase& Test, const FString& What, const TArray<FColor>& Bytes, const FColor& Expected)
{
    if (!Test.TestEqual(What + TEXT(": pixel count"), Bytes.Num(), Width * Height))
    {
        return false;
    }
    for (int32 Index = 0; Index < Bytes.Num(); ++Index)
    {
        const FColor& Actual = Bytes[Index];
        if (FMath::Abs(Actual.R - Expected.R) > 1 || FMath::Abs(Actual.G - Expected.G) > 1 || FMath::Abs(Actual.B - Expected.B) > 1 || Actual.A != Expected.A)
        {
            Test.AddError(FString::Printf(TEXT("%s: pixel (%d, %d) expected %s, got %s"), *What, Index % Width, Index / Width,
                *Expected.ToString(), *Actual.ToString()));
            return false;
        }
    }
    return true;
}

/** Sets the sRGB setting of a texture as the details view does, once the build started by the last update of the texture is done. */
void SetSRGB(UTexture2D* Texture, bool bSRGB)
{
    // A build in progress reads the setting: changing it meanwhile breaks the build.
    FTextureCompilingManager::Get().FinishCompilation({ Texture });
    FProperty* Property = FindFProperty<FProperty>(UTexture::StaticClass(), GET_MEMBER_NAME_CHECKED(UTexture, SRGB));
    Texture->PreEditChange(Property);
    Texture->SRGB = bSRGB;
    FPropertyChangedEvent Event(Property);
    Texture->PostEditChangeProperty(Event);
    FTextureCompilingManager::Get().FinishCompilation({ Texture });
}

TMap<FIntPoint, FOdysseyTileId> ReadTileIds(const UOdysseyTiledImage* Image)
{
    const FMapProperty* Property = FindFProperty<FMapProperty>(Image->GetClass(), TEXT("Tiles"));
    return *Property->ContainerPtrToValuePtr<TMap<FIntPoint, FOdysseyTileId>>(Image);
}

int32 CountRenderTargets()
{
    int32 Count = 0;
    for (TObjectIterator<UTextureRenderTarget2D> It; It; ++It)
    {
        ++Count;
    }
    return Count;
}

/** Counts the calls to Modify() on an object: the tiled image calls it before writing tiles. */
struct FModifyCounter
{
    explicit FModifyCounter(UObject* InObject)
        : Object(InObject)
    {
        // Modify() notifies once per object and frame, and the tests run within a frame.
        FCoreUObjectDelegates::ObjectsModifiedThisFrame.Remove(Object);
        Handle = FCoreUObjectDelegates::OnObjectModified.AddLambda([this](UObject* Modified)
        {
            if (Modified == Object)
            {
                ++Count;
                FCoreUObjectDelegates::ObjectsModifiedThisFrame.Remove(Object);
            }
        });
    }

    ~FModifyCounter()
    {
        FCoreUObjectDelegates::OnObjectModified.Remove(Handle);
    }

    UObject* Object;
    int32 Count = 0;
    FDelegateHandle Handle;
};

/** Counts the notifications of a changed render of a node. */
struct FRenderingChangedCounter
{
    explicit FRenderingChangedCounter(const FGuid& InId)
        : Id(InId)
    {
        Handle = IOdysseyRenderingAbility::OnRenderingChangedDelegate().AddLambda([this](const FOdysseyRenderingChangedEvent& Event)
        {
            if (Event.GetId() == Id)
            {
                ++(Event.IsInteractive() ? Interactive : NonInteractive);
            }
        });
    }

    ~FRenderingChangedCounter()
    {
        IOdysseyRenderingAbility::OnRenderingChangedDelegate().Remove(Handle);
    }

    FGuid Id;
    int32 Interactive = 0;
    int32 NonInteractive = 0;
    FDelegateHandle Handle;
};

/** Creates the render texture a vector layer saved before the tiled image held its render owned, under its former name. */
UTexture2D* MakeLegacyRenderTexture(UObject* Layer, EObjectFlags ObjectFlags)
{
    UTexture2D* Texture = NewObject<UTexture2D>(Layer, TEXT("Texture"), ObjectFlags);
    FImage Image(4, 4, ERawImageFormat::BGRA8, EGammaSpace::sRGB);
    for (FColor& Pixel : Image.AsBGRA8())
    {
        Pixel = Fill;
    }
    Texture->Source.Init(Image);
    Texture->MipGenSettings = TMGS_NoMipmaps;
    Texture->UpdateResource();
    FTextureCompilingManager::Get().FinishCompilation({ Texture });
    return Texture;
}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOdysseyTextureVectorStackRenderTest, "Odyssey.Texture.Vector.StackRenderAndDuplicate", Odyssey::TextureLayerVectorTests::Flags)

/**
 * A vector layer renders its scene into a transient tiled image when the layer stack is first displayed;
 * a duplicate renders the same pixels from its own scene, into its own image.
 */
bool FOdysseyTextureVectorStackRenderTest::RunTest(const FString& Parameters)
{
    using namespace Odyssey::TextureLayerVectorTests;
    for (const bool bSRGB : { false, true })
    {
        const FString Encoding = bSRGB ? TEXT("sRGB texture") : TEXT("linear texture");
        TStrongObjectPtr<UTexture2D> Texture(MakeTexture(GetTransientPackage(), bSRGB));
        UOdysseyTextureLayerStack* Stack = GetStack(Texture.Get());
        UOdysseyTextureLayerImageVector* Layer = AddVectorLayer(Stack);
        SetSceneColor(Layer, Fill);
        TestNull(Encoding + TEXT(": the scene is not rendered before it is displayed"), Layer->GetVectorContent().GetImage());

        CheckUniform(*this, Encoding + TEXT(": the stack shows the scene of its vector layer"), ReadNode(Stack->GetLayerRoot()), Stacked(Fill), StackTolerance(bSRGB));
        const UOdysseyTiledImage* Image = Layer->GetVectorContent().GetImage();
        if (!TestNotNull(Encoding + TEXT(": the render is held by a tiled image"), Image))
        {
            return false;
        }
        CheckUniform(*this, Encoding + TEXT(": the tiles hold the linear colors of the scene"), ReadImage(Image), Linear(Fill, bSRGB), Tolerance(bSRGB));
        TestEqual(Encoding + TEXT(": the image is not a subobject of the layer"), Image->GetOuter(), static_cast<UObject*>(GetTransientPackage()));
        TestTrue(Encoding + TEXT(": the image is never saved"), Image->HasAnyFlags(RF_Transient));
        TestFalse(Encoding + TEXT(": the tiles are not undoable, an undo redraws the scene"), Image->HasAnyFlags(RF_Transactional));
        TestEqual(Encoding + TEXT(": the tiles hold linear colors"), Image->GetFormat(), bSRGB ? EOdysseyTiledImageFormat::RGBA16F : EOdysseyTiledImageFormat::RGBA8);

        TStrongObjectPtr<UTexture2D> Duplicate(DuplicateObject<UTexture2D>(Texture.Get(), GetTransientPackage()));
        UOdysseyTextureLayerStack* DuplicateStack = GetStack(Duplicate.Get());
        UOdysseyTextureLayerImageVector* DuplicateLayer = FindVectorLayer(DuplicateStack);
        if (!TestNotNull(Encoding + TEXT(": the duplicate has a vector layer"), DuplicateLayer))
        {
            return false;
        }
        TestNull(Encoding + TEXT(": the render is not duplicated"), DuplicateLayer->GetVectorContent().GetImage());
        TestTrue(Encoding + TEXT(": the duplicate has its own scene"), DuplicateLayer->GetVectorCell() != Layer->GetVectorCell());
        CheckUniform(*this, Encoding + TEXT(": the duplicate shows the duplicated scene"), ReadNode(DuplicateStack->GetLayerRoot()), Stacked(Fill), StackTolerance(bSRGB));
        TestTrue(Encoding + TEXT(": the duplicate renders into its own image"), DuplicateLayer->GetVectorContent().GetImage() && DuplicateLayer->GetVectorContent().GetImage() != Image);

        // The content of the duplicate follows the scene of the duplicate.
        SetSceneColor(DuplicateLayer, OtherFill);
        CheckUniform(*this, Encoding + TEXT(": the duplicate shows the edits of its scene"), ReadNode(DuplicateStack->GetLayerRoot()), Stacked(OtherFill), StackTolerance(bSRGB));
        CheckUniform(*this, Encoding + TEXT(": the original is not changed by the edits of the duplicate"), ReadNode(Stack->GetLayerRoot()), Stacked(Fill), StackTolerance(bSRGB));
    }
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOdysseyTextureVectorDrawingFlagsTest, "Odyssey.Texture.Vector.DrawingFlags", Odyssey::TextureLayerVectorTests::Flags)

/**
 * The rendered tiles are kept until the scene asks for a redraw. Toggling the colors or the wireframe of the layer renders the whole canvas again:
 * the scene is changed without telling the layer, so only a new render of the whole canvas shows the change.
 */
bool FOdysseyTextureVectorDrawingFlagsTest::RunTest(const FString& Parameters)
{
    using namespace Odyssey::TextureLayerVectorTests;
    const float Precision = Tolerance(false);
    TStrongObjectPtr<UTexture2D> Texture(MakeTexture(GetTransientPackage(), false));
    UOdysseyTextureLayerImageVector* Layer = AddVectorLayer(GetStack(Texture.Get()));
    SetSceneColor(Layer, Fill);
    CheckUniform(*this, TEXT("The colored scene is rendered"), ReadNode(Layer), Linear(Fill, false), Precision);
    const UOdysseyTiledImage* Image = Layer->GetVectorContent().GetImage();
    if (!TestNotNull(TEXT("The render is held by a tiled image"), Image))
    {
        return false;
    }
    FModifyCounter Writes(const_cast<UOdysseyTiledImage*>(Image));

    SetSceneColor(Layer, OtherFill, false);
    CheckUniform(*this, TEXT("Without a redraw request, the tiles are kept"), ReadNode(Layer), Linear(Fill, false), Precision);
    TestEqual(TEXT("Without a redraw request, no tile is written"), Writes.Count, 0);

    Layer->SetIsColored(false);
    CheckUniform(*this, TEXT("An uncolored scene is drawn with its monochrome color"), ReadNode(Layer), Linear(Monochrome, false), Precision);
    Layer->SetIsColored(true);
    CheckUniform(*this, TEXT("Colors are rendered again on the whole canvas"), ReadNode(Layer), Linear(OtherFill, false), Precision);

    SetSceneColor(Layer, Fill, false);
    const int32 WritesBeforeWireframe = Writes.Count;
    Layer->SetIsWireframe(true);
    // The scene has no path: the wireframe draws nothing over the background.
    CheckUniform(*this, TEXT("The wireframe toggle renders the whole canvas again"), ReadNode(Layer), Linear(Fill, false), Precision);
    TestEqual(TEXT("The wireframe toggle writes the tiles once"), Writes.Count, WritesBeforeWireframe + 1);
    SetSceneColor(Layer, OtherFill, false);
    Layer->SetIsWireframe(false);
    CheckUniform(*this, TEXT("Turning the wireframe off renders the whole canvas again"), ReadNode(Layer), Linear(OtherFill, false), Precision);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOdysseyTextureVectorCanvasResizeTest, "Odyssey.Texture.Vector.CanvasResize", Odyssey::TextureLayerVectorTests::Flags)

/** When the texture changes size, the vector layer renders its scene again on the whole new canvas, and drops the tiles of the former one. */
bool FOdysseyTextureVectorCanvasResizeTest::RunTest(const FString& Parameters)
{
    using namespace Odyssey::TextureLayerVectorTests;
    const float Precision = Tolerance(false);
    TStrongObjectPtr<UTexture2D> Texture(MakeTexture(GetTransientPackage(), false));
    UOdysseyTextureLayerImageVector* Layer = AddVectorLayer(GetStack(Texture.Get()));
    SetSceneColor(Layer, Fill);
    CheckUniform(*this, TEXT("The scene is rendered on the canvas"), ReadNode(Layer), Linear(Fill, false), Precision);
    const UOdysseyTiledImage* Image = Layer->GetVectorContent().GetImage();
    if (!TestNotNull(TEXT("The render is held by a tiled image"), Image))
    {
        return false;
    }
    TestTrue(TEXT("The tiles cover the canvas"), Image->GetTileBounds() == FIntRect(0, 0, 128, 128));

    // Wider and shorter: pixels beyond the former width, and fewer tile rows.
    const FIntPoint NewSize(101, 45);
    Texture->Source.Init(NewSize.X, NewSize.Y, 1, 1, TSF_BGRA8);
    CheckUniform(*this, TEXT("The scene is rendered on the whole resized canvas"), ReadNode(Layer, NewSize), Linear(Fill, false), Precision, NewSize);
    TestTrue(FString::Printf(TEXT("The tiles of the former canvas are dropped, got %s"), *Image->GetTileBounds().ToString()),
        Image->GetTileBounds() == FIntRect(0, 0, 128, 64));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOdysseyTextureVectorSRGBChangeTest, "Odyssey.Texture.Vector.SRGBChangeKeepsBytes", Odyssey::TextureLayerVectorTests::Flags)

/** Changing the sRGB setting of a texture keeps the bytes of the render of its vector layers: their tiles follow the new encoding. */
bool FOdysseyTextureVectorSRGBChangeTest::RunTest(const FString& Parameters)
{
    using namespace Odyssey::TextureLayerVectorTests;
    TStrongObjectPtr<UTexture2D> Texture(MakeTexture(GetTransientPackage(), true));
    UOdysseyTextureLayerImageVector* Layer = AddVectorLayer(GetStack(Texture.Get()));
    SetSceneColor(Layer, Fill);
    CheckUniformBytes(*this, TEXT("An sRGB texture gets the bytes of the render"), ReadTextureBytes(Texture.Get()), Fill);
    if (!TestNotNull(TEXT("The render is held by a tiled image"), Layer->GetVectorContent().GetImage()))
    {
        return false;
    }
    TestEqual(TEXT("The render of an sRGB texture is decoded"), Layer->GetVectorContent().GetImage()->GetFormat(), EOdysseyTiledImageFormat::RGBA16F);

    SetSRGB(Texture.Get(), false);
    CheckUniformBytes(*this, TEXT("Bytes are kept when sRGB is turned off"), ReadTextureBytes(Texture.Get()), Fill);
    TestEqual(TEXT("The render of a linear texture holds its bytes"), Layer->GetVectorContent().GetImage()->GetFormat(), EOdysseyTiledImageFormat::RGBA8);

    SetSRGB(Texture.Get(), true);
    CheckUniformBytes(*this, TEXT("Bytes are kept when sRGB is turned on"), ReadTextureBytes(Texture.Get()), Fill);
    TestEqual(TEXT("The render is decoded again"), Layer->GetVectorContent().GetImage()->GetFormat(), EOdysseyTiledImageFormat::RGBA16F);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOdysseyTextureVectorInteractiveTest, "Odyssey.Texture.Vector.InteractiveRedraws", Odyssey::TextureLayerVectorTests::Flags)

/**
 * Interactive redraws of the scene are shown by the views through one edit session, without writing tiles nor creating render targets.
 * The final redraw ends the session; the areas it showed and the final area are rendered into the tiles once, when displayed.
 */
bool FOdysseyTextureVectorInteractiveTest::RunTest(const FString& Parameters)
{
    using namespace Odyssey::TextureLayerVectorTests;
    const float Precision = Tolerance(false);
    TStrongObjectPtr<UTexture2D> Texture(MakeTexture(GetTransientPackage(), false));
    UOdysseyTextureLayerImageVector* Layer = AddVectorLayer(GetStack(Texture.Get()));
    SetSceneColor(Layer, Fill);
    CheckUniform(*this, TEXT("The scene is rendered"), ReadNode(Layer), Linear(Fill, false), Precision);
    const UOdysseyTiledImage* Image = Layer->GetVectorContent().GetImage();
    if (!TestNotNull(TEXT("The render is held by a tiled image"), Image))
    {
        return false;
    }
    const FGuid Id = Layer->GetRenderingId();
    const TMap<FIntPoint, FOdysseyTileId> TilesBefore = ReadTileIds(Image);
    FModifyCounter Writes(const_cast<UOdysseyTiledImage*>(Image));
    FRenderingChangedCounter Changes(Id);

    // A tool drags over the canvas: each step invalidates the area it changed.
    FOdysseyVectorCell* Cell = Layer->GetVectorCell().Get();
    SetSceneColor(Layer, OtherFill, false);
    UTextureRenderTarget2D* SessionTarget = nullptr;
    int32 RenderTargets = 0;
    for (int32 Step = 0; Step < 5; ++Step)
    {
        Cell->InvalidateRect(::ULIS::FRectD(10 + 2 * Step, 12, 8, 8));
        Cell->GetLayer()->RequestRedraw(Cell, FOdysseyVectorCell::REDRAW_INTERACTIVE);
        TSharedPtr<FOdysseyRasterEditSession> Session = FOdysseyRasterEditSession::Find(Id);
        if (!TestTrue(FString::Printf(TEXT("Step %d: an edit session shows the interactive redraws"), Step), Session.IsValid() && Session->IsActive()))
        {
            return false;
        }
        if (Step == 0)
        {
            SessionTarget = Session->GetTarget();
            RenderTargets = CountRenderTargets();
        }
        else
        {
            TestTrue(FString::Printf(TEXT("Step %d: the redraws are shown by the same session"), Step), Session->GetTarget() == SessionTarget);
            TestEqual(FString::Printf(TEXT("Step %d: no render target is created"), Step), CountRenderTargets(), RenderTargets);
        }
    }
    TestEqual(TEXT("No tile is written during the interaction"), Writes.Count, 0);
    TestTrue(TEXT("The tiles are unchanged during the interaction"), ReadTileIds(Image).OrderIndependentCompareEqual(TilesBefore));
    TestEqual(TEXT("The owner is not told of a committed change during the interaction"), Changes.NonInteractive, 0);

    const TArray<FLinearColor> Preview = ReadNode(Layer);
    CheckPixel(*this, TEXT("The views show the interactive redraws"), Preview, FIntPoint(14, 15), Linear(OtherFill, false), Precision);
    CheckPixel(*this, TEXT("The views show the tiles outside the redrawn areas"), Preview, FIntPoint(60, 50), Linear(Fill, false), Precision);
    CheckPixel(*this, TEXT("Renders other than the views show the tiles"), ReadNode(Layer, FIntPoint(Width, Height), false), FIntPoint(14, 15), Linear(Fill, false), Precision);
    TWeakObjectPtr<UTextureRenderTarget2D> WeakTarget(SessionTarget);
    SessionTarget = nullptr;

    // The release redraws a last area only.
    Cell->InvalidateRect(::ULIS::FRectD(50, 40, 6, 6));
    Cell->GetLayer()->RequestRedraw(Cell, 0);
    TestFalse(TEXT("The final redraw ends the edit session"), FOdysseyRasterEditSession::Find(Id).IsValid());
    TestEqual(TEXT("The owner is told of the change once"), Changes.NonInteractive, 1);
    TestEqual(TEXT("The tiles are rendered when displayed"), Writes.Count, 0);

    const TArray<FLinearColor> Committed = ReadNode(Layer, FIntPoint(Width, Height), false);
    TestEqual(TEXT("The tiles are written once"), Writes.Count, 1);
    CheckPixel(*this, TEXT("The tiles hold the areas shown by the session"), Committed, FIntPoint(14, 15), Linear(OtherFill, false), Precision);
    CheckPixel(*this, TEXT("The tiles hold the area of the final redraw"), Committed, FIntPoint(52, 42), Linear(OtherFill, false), Precision);
    CheckPixel(*this, TEXT("The tiles keep the areas no redraw asked for"), Committed, FIntPoint(2, 60), Linear(Fill, false), Precision);
    ReadNode(Layer);
    TestEqual(TEXT("Displaying again writes no tile"), Writes.Count, 1);

    CollectGarbage(GARBAGE_COLLECTION_KEEPFLAGS);
    TestFalse(TEXT("The render target of the ended session is released"), WeakTarget.IsValid());
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOdysseyTextureVectorUndoTest, "Odyssey.Texture.Vector.Undo", Odyssey::TextureLayerVectorTests::Flags)

/**
 * Undoing a vector edit shows the previous render, whether the edit was displayed after its transaction,
 * or within it. The tiles are not recorded in the transaction: the undo redraws the scene.
 */
bool FOdysseyTextureVectorUndoTest::RunTest(const FString& Parameters)
{
    using namespace Odyssey::TextureLayerVectorTests;
    const float Precision = Tolerance(false);
    TStrongObjectPtr<UTexture2D> Texture(MakeTexture(GetTransientPackage(), false));
    UOdysseyTextureLayerImageVector* Layer = AddVectorLayer(GetStack(Texture.Get()));
    SetSceneColor(Layer, Fill);
    CheckUniform(*this, TEXT("The scene is rendered"), ReadNode(Layer), Linear(Fill, false), Precision);

    {
        FScopedTransaction Transaction(NSLOCTEXT("OdysseyTextureLayerVectorTests", "Clear", "Clear the scene"));
        ClearScene(Layer);
    }
    CheckUniform(*this, TEXT("The cleared scene renders nothing"), ReadNode(Layer), FLinearColor::Transparent, Precision);
    TestTrue(TEXT("Undo the clear"), GEditor->UndoTransaction());
    CheckUniform(*this, TEXT("The undone clear shows the previous render"), ReadNode(Layer), Linear(Fill, false), Precision);
    TestTrue(TEXT("Redo the clear"), GEditor->RedoTransaction());
    CheckUniform(*this, TEXT("The redone clear renders nothing"), ReadNode(Layer), FLinearColor::Transparent, Precision);
    TestTrue(TEXT("Undo the clear again"), GEditor->UndoTransaction());
    CheckUniform(*this, TEXT("The clear undone again shows the previous render"), ReadNode(Layer), Linear(Fill, false), Precision);

    {
        FScopedTransaction Transaction(NSLOCTEXT("OdysseyTextureLayerVectorTests", "ClearDisplayed", "Clear and display the scene"));
        ClearScene(Layer);
        CheckUniform(*this, TEXT("The clear displayed within its transaction renders nothing"), ReadNode(Layer), FLinearColor::Transparent, Precision);
    }
    const FTransaction* Displayed = GEditor->Trans->GetTransaction(GEditor->Trans->GetQueueLength() - 1);
    TestFalse(TEXT("The transaction does not record the tiles"), Displayed && Displayed->ContainsObject(Layer->GetVectorContent().GetImage()));
    TestTrue(TEXT("Undo the displayed clear"), GEditor->UndoTransaction());
    CheckUniform(*this, TEXT("The undone displayed clear shows the previous render"), ReadNode(Layer), Linear(Fill, false), Precision);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOdysseyTextureVectorGarbageCollectionTest, "Odyssey.Texture.Vector.GarbageCollection", Odyssey::TextureLayerVectorTests::Flags)

/**
 * The vector content keeps its image alive while the layer lives, and does not keep the layer alive: both are collected with the texture.
 */
bool FOdysseyTextureVectorGarbageCollectionTest::RunTest(const FString& Parameters)
{
    using namespace Odyssey::TextureLayerVectorTests;
    const float Precision = Tolerance(false);
    TStrongObjectPtr<UTexture2D> Texture(MakeTexture(GetTransientPackage(), false));
    UOdysseyTextureLayerImageVector* Layer = AddVectorLayer(GetStack(Texture.Get()));
    SetSceneColor(Layer, Fill);
    CheckUniform(*this, TEXT("The scene is rendered"), ReadNode(Layer), Linear(Fill, false), Precision);
    TWeakObjectPtr<const UOdysseyTiledImage> Image(Layer->GetVectorContent().GetImage());
    TWeakObjectPtr<UOdysseyTextureLayerImageVector> WeakLayer(Layer);
    TestTrue(TEXT("The render is held by a tiled image"), Image.IsValid());

    CollectGarbage(GARBAGE_COLLECTION_KEEPFLAGS);
    TestTrue(TEXT("The image survives a collection while the layer lives"), Image.IsValid() && Image.Get() == Layer->GetVectorContent().GetImage());
    CheckUniform(*this, TEXT("The collected scene still shows its render"), ReadNode(Layer), Linear(Fill, false), Precision);

    Texture.Reset();
    // The first collection destroys the layer, which releases its content; the next one collects the image.
    CollectGarbage(GARBAGE_COLLECTION_KEEPFLAGS);
    CollectGarbage(GARBAGE_COLLECTION_KEEPFLAGS);
    TestFalse(TEXT("The layer is collected with its texture"), WeakLayer.IsValid());
    TestFalse(TEXT("The image is collected with its layer"), Image.IsValid());
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOdysseyTextureVectorPersistenceTest, "Odyssey.Texture.Vector.Persistence", Odyssey::TextureLayerVectorTests::Flags)

/**
 * The render of a vector layer is not saved: a loaded layer renders its scene when displayed, without dirtying its package.
 * The render texture of a layer saved before the tiled image held the render still loads, and is no longer saved.
 */
bool FOdysseyTextureVectorPersistenceTest::RunTest(const FString& Parameters)
{
    using namespace Odyssey::TextureLayerVectorTests;
    // This temporary fixture tests persistence, not project asset validation.
    FScopedDisableValidateOnSave DisableValidation;
    const float Precision = StackTolerance(true);
    const FString Id = FGuid::NewGuid().ToString(EGuidFormats::Digits);
    const FString MountPoint = TEXT("/OdysseyTextureVectorTests/");
    const FString Directory = FPaths::ConvertRelativePathToFull(FPaths::ProjectPluginsDir() / TEXT("Odyssey-Plugin/Intermediate/OdysseyTextureTests")) + TEXT("/");
    FPackageName::RegisterMountPoint(MountPoint, Directory);
    ON_SCOPE_EXIT { FPackageName::UnRegisterMountPoint(MountPoint, Directory); };
    const FString PackageName = MountPoint + Id;
    const FString Filename = Directory / (Id + TEXT(".uasset"));
    IFileManager::Get().MakeDirectory(*FPaths::GetPath(Filename), true);
    ON_SCOPE_EXIT { IFileManager::Get().Delete(*Filename); };
    FSavePackageArgs Args;
    Args.TopLevelFlags = RF_Public;
    Args.SaveFlags = SAVE_NoError;

    UPackage* Package = CreatePackage(*PackageName);
    TStrongObjectPtr<UTexture2D> Texture(MakeTexture(Package, true));
    UOdysseyTextureLayerStack* Stack = GetStack(Texture.Get());
    UOdysseyTextureLayerImageVector* Layer = AddVectorLayer(Stack);
    SetSceneColor(Layer, Fill);
    const TArray<FLinearColor> Reference = ReadNode(Stack->GetLayerRoot());
    CheckUniform(*this, TEXT("The scene is rendered before saving"), Reference, Stacked(Fill), Precision);
    // Public, so that the save writes it as the former render texture was: referenced by a property that no longer exists.
    MakeLegacyRenderTexture(Layer, RF_Public);
    const FString AssetName = Texture->GetName();
    if (!TestTrue(TEXT("Save the texture"), UPackage::SavePackage(Package, Texture.Get(), *Filename, Args)))
    {
        return false;
    }
    // Keep the original alive under another name to force a real disk load.
    Package->Rename(*(PackageName + TEXT("_Original")), nullptr, REN_DontCreateRedirectors | REN_NonTransactional);

    UPackage* LoadedPackage = LoadPackage(nullptr, *PackageName, LOAD_None);
    UTexture2D* LoadedTexture = LoadedPackage ? FindObject<UTexture2D>(LoadedPackage, *AssetName) : nullptr;
    if (!TestNotNull(TEXT("Load the texture"), LoadedTexture))
    {
        return false;
    }
    UOdysseyTextureLayerStack* LoadedStack = GetStack(LoadedTexture);
    UOdysseyTextureLayerImageVector* LoadedLayer = FindVectorLayer(LoadedStack);
    if (!TestNotNull(TEXT("Load the vector layer"), LoadedLayer))
    {
        return false;
    }
    UTexture2D* LoadedLegacyTexture = FindObject<UTexture2D>(LoadedLayer, TEXT("Texture"));
    TestNotNull(TEXT("The former render texture is loaded with the layer"), LoadedLegacyTexture);
    TestNull(TEXT("The render is not saved"), LoadedLayer->GetVectorContent().GetImage());
    TestFalse(TEXT("Loading does not dirty the package"), LoadedPackage->IsDirty());

    CheckUniform(*this, TEXT("The loaded layer renders its scene"), ReadNode(LoadedStack->GetLayerRoot()), Stacked(Fill), Precision);
    TestNotNull(TEXT("The loaded layer renders into a tiled image"), LoadedLayer->GetVectorContent().GetImage());
    TestFalse(TEXT("Displaying the loaded vector layer does not dirty its package"), LoadedPackage->IsDirty());
    // Check that this package can be dirtied, so that the check above is meaningful.
    LoadedLayer->Modify();
    TestTrue(TEXT("Editing the loaded layer dirties its package"), LoadedPackage->IsDirty());
    LoadedPackage->SetDirtyFlag(false);

    // Once loaded, nothing references the former render texture: the next save drops it.
    if (LoadedLegacyTexture)
    {
        LoadedLegacyTexture->ClearFlags(RF_Public);
    }
    if (!TestTrue(TEXT("Save the loaded texture"), UPackage::SavePackage(LoadedPackage, LoadedTexture, *Filename, Args)))
    {
        return false;
    }
    LoadedPackage->Rename(*(PackageName + TEXT("_Loaded")), nullptr, REN_DontCreateRedirectors | REN_NonTransactional);
    UPackage* ReloadedPackage = LoadPackage(nullptr, *PackageName, LOAD_None);
    // Release the file before it is deleted.
    ON_SCOPE_EXIT { if (ReloadedPackage) { ResetLoaders(ReloadedPackage); } };
    UTexture2D* ReloadedTexture = ReloadedPackage ? FindObject<UTexture2D>(ReloadedPackage, *AssetName) : nullptr;
    if (!TestNotNull(TEXT("Reload the texture"), ReloadedTexture))
    {
        return false;
    }
    UOdysseyTextureLayerStack* ReloadedStack = GetStack(ReloadedTexture);
    UOdysseyTextureLayerImageVector* ReloadedLayer = FindVectorLayer(ReloadedStack);
    if (!TestNotNull(TEXT("Reload the vector layer"), ReloadedLayer))
    {
        return false;
    }
    TestNull(TEXT("The former render texture is not saved again"), FindObject<UTexture2D>(ReloadedLayer, TEXT("Texture")));
    CheckUniform(*this, TEXT("The resaved layer renders its scene"), ReadNode(ReloadedStack->GetLayerRoot()), Stacked(Fill), Precision);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOdysseyTextureVectorLegacyAssetTest, "Odyssey.Texture.Vector.LegacyAsset", Odyssey::TextureLayerVectorTests::Flags)

/** A texture saved with a vector layer by a former version, with its former properties, loads and renders its vector layer without dirtying its package. */
bool FOdysseyTextureVectorLegacyAssetTest::RunTest(const FString& Parameters)
{
    using namespace Odyssey::TextureLayerVectorTests;
    UTexture2D* Texture = LoadObject<UTexture2D>(nullptr, TEXT("/Odyssey/Brushes/Vector_Tools/V_Pencil_0.V_Pencil_0"));
    if (!TestNotNull(TEXT("Load the texture saved by a former version"), Texture))
    {
        return false;
    }
    UOdysseyTextureLayerStack* Stack = GetStack(Texture);
    UOdysseyTextureLayerImageVector* Layer = FindVectorLayer(Stack);
    if (!TestNotNull(TEXT("The texture has a vector layer"), Layer))
    {
        return false;
    }
    UPackage* Package = Texture->GetPackage();
    const bool bWasDirty = Package->IsDirty();
    AddInfo(FString::Printf(TEXT("The package was %s before the display"), bWasDirty ? TEXT("dirty") : TEXT("clean")));
    const FIntPoint Size(Texture->Source.GetSizeX(), Texture->Source.GetSizeY());
    const TArray<FLinearColor> Pixels = ReadNode(Layer, Size, false);
    if (!TestEqual(TEXT("The vector layer renders the canvas"), Pixels.Num(), Size.X * Size.Y))
    {
        return false;
    }
    TestNotNull(TEXT("The vector layer renders into a tiled image"), Layer->GetVectorContent().GetImage());
    int32 Drawn = 0;
    for (const FLinearColor& Pixel : Pixels)
    {
        if (Pixel.A > 0.0f)
        {
            ++Drawn;
        }
    }
    AddInfo(FString::Printf(TEXT("%d of %d pixels drawn by the vector layer"), Drawn, Pixels.Num()));
    TestTrue(TEXT("The vector layer renders its scene"), Drawn > 0);
    if (!bWasDirty)
    {
        TestFalse(TEXT("Displaying the vector layer does not dirty its package"), Package->IsDirty());
    }
    return true;
}

#endif
