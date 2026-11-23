// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#if WITH_DEV_AUTOMATION_TESTS
#include "Editor.h"
#include "Engine/Texture2D.h"
#include "ImageCore.h"
#include "Misc/AutomationTest.h"
#include "ScopedTransaction.h"
#include "TextureCompiler.h"
#include "UObject/Package.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/UnrealType.h"

#include "OdysseyColorSpaceShader.h"
#include "OdysseyRasterContent.h"
#include "OdysseyTextureLayerImageRaster.h"
#include "OdysseyTextureLayerImageVector.h"
#include "OdysseyTextureLayerStack.h"
#include "OdysseyTextureLayerStackUserData.h"
#include "OdysseyTiledImage.h"
#include "OdysseyTiledImageBlock.h"
#include "OdysseyRasterContentBlocks.h"
#include "OdysseyVectorContent.h"

namespace Odyssey::TextureColorTests
{
constexpr int32 Width = 37;
constexpr int32 Height = 29;
constexpr EAutomationTestFlags Flags = EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter;
/** The color of the layer blended over the pattern, as bytes. */
const FColor TopColor(200, 40, 120, 255);

/** Returns the bytes of the pattern at a pixel: opaque, covering dark and bright values. */
FColor PatternColor(int32 X, int32 Y)
{
    return FColor(uint8((X * 7) % 256), uint8((Y * 9) % 256), uint8((X * 3 + Y * 5) % 256), 255);
}

UTexture2D* MakeTexture(bool bSRGB)
{
    UTexture2D* Texture = NewObject<UTexture2D>(GetTransientPackage(), NAME_None, RF_Public | RF_Transactional);
    FImage Image(Width, Height, ERawImageFormat::BGRA8, bSRGB ? EGammaSpace::sRGB : EGammaSpace::Linear);
    for (int32 Y = 0; Y < Height; ++Y)
    {
        for (int32 X = 0; X < Width; ++X)
        {
            Image.AsBGRA8()[Y * Width + X] = PatternColor(X, Y);
        }
    }
    Texture->Source.Init(Image);
    Texture->SRGB = bSRGB;
    Texture->MipGenSettings = TMGS_NoMipmaps;
    Texture->UpdateResource();
    FTextureCompilingManager::Get().FinishCompilation({ Texture });
    return Texture;
}

UOdysseyTextureLayerStackUserData* MakeUserData(UTexture2D* Texture)
{
    UOdysseyTextureLayerStackUserData* UserData = NewObject<UOdysseyTextureLayerStackUserData>(Texture);
    Texture->AddAssetUserData(UserData);
    UserData->InitWithDefaultLayerStack();
    UserData->LayerStack->SetTextureUpdateMode(EOdysseyTextureLayerStackTextureUpdateMode::Manually);
    return UserData;
}

/** Renders the layer stack into the texture source and returns its bytes. */
TArray<FColor> ReadTextureBytes(UTexture2D* Texture, UOdysseyTextureLayerStack* Stack)
{
    Stack->UpdateTexture(true);
    FImage Image;
    Texture->Source.GetMipImage(Image, 0);
    TArray<FColor> Bytes(Image.AsBGRA8().GetData(), Image.AsBGRA8().Num());
    return Bytes;
}

/** Adds an opaque layer of TopColor, blended at half opacity, and returns it. */
UOdysseyTextureLayerImageRaster* AddHalfOpaqueTopLayer(UOdysseyTextureLayerStack* Stack)
{
    UOdysseyTextureLayerImageRaster* Top = CastChecked<UOdysseyTextureLayerImageRaster>(Stack->AddLayer(UOdysseyTextureLayerImageRaster::StaticClass()));
    FOdysseyRasterContent& Content = Top->GetRasterContent();
    TSharedPtr<ULIS::FBlock> Solid = MakeShared<ULIS::FBlock>(Width, Height, Content.Describe().Format);
    for (int32 Y = 0; Y < Height; ++Y)
    {
        for (int32 X = 0; X < Width; ++X)
        {
            uint8* Pixel = Solid->PixelBits(static_cast<uint16>(X), static_cast<uint16>(Y));
            // BGRA8, as the blocks of a BGRA8 texture.
            Pixel[0] = TopColor.B;
            Pixel[1] = TopColor.G;
            Pixel[2] = TopColor.R;
            Pixel[3] = TopColor.A;
        }
    }
    Odyssey::RasterContentBlocks::WriteBlock(Content, *Solid, { FIntRect(0, 0, Width, Height) });
    Top->SetOpacity(0.5f);
    return Top;
}

bool CompareBytes(FAutomationTestBase& Test, const TCHAR* What, TFunctionRef<FColor(int32, int32)> Expected, const TArray<FColor>& Actual, int32 Tolerance)
{
    if (!Test.TestEqual(FString::Printf(TEXT("%s: pixel count"), What), Actual.Num(), Width * Height))
    {
        return false;
    }
    for (int32 Index = 0; Index < Actual.Num(); ++Index)
    {
        const FColor ExpectedColor = Expected(Index % Width, Index / Width);
        const FColor& ActualColor = Actual[Index];
        if (FMath::Abs(ExpectedColor.R - ActualColor.R) > Tolerance || FMath::Abs(ExpectedColor.G - ActualColor.G) > Tolerance
            || FMath::Abs(ExpectedColor.B - ActualColor.B) > Tolerance || FMath::Abs(ExpectedColor.A - ActualColor.A) > Tolerance)
        {
            Test.AddError(FString::Printf(TEXT("%s: pixel (%d, %d) expected %s, got %s"), What, Index % Width, Index / Width,
                *ExpectedColor.ToString(), *ActualColor.ToString()));
            return false;
        }
    }
    return true;
}

/** Sets the sRGB setting of a texture as the details view does. */
void SetSRGB(UTexture2D* Texture, bool bSRGB)
{
    Texture->SRGB = bSRGB;
    FPropertyChangedEvent Event(FindFProperty<FProperty>(UTexture::StaticClass(), GET_MEMBER_NAME_CHECKED(UTexture, SRGB)));
    Texture->PostEditChangeProperty(Event);
    FTextureCompilingManager::Get().FinishCompilation({ Texture });
}

FColor Lerp(const FColor& A, const FColor& B)
{
    return FColor(uint8((A.R + B.R + 1) / 2), uint8((A.G + B.G + 1) / 2), uint8((A.B + B.B + 1) / 2), 255);
}

FColor LerpLinear(const FColor& A, const FColor& B)
{
    const FLinearColor Mixed = (FLinearColor::FromSRGBColor(A) + FLinearColor::FromSRGBColor(B)) * 0.5f;
    return FLinearColor(Mixed.R, Mixed.G, Mixed.B, 1.0f).ToFColorSRGB();
}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOdysseyTextureColorCompositingTest, "Odyssey.Texture.Color.Compositing", Odyssey::TextureColorTests::Flags)

/**
 * The layers of an sRGB texture hold decoded colors and write the same bytes back.
 * They are blended with encoded colors by default, as painting applications do, or with linear colors when asked.
 */
bool FOdysseyTextureColorCompositingTest::RunTest(const FString& Parameters)
{
    using namespace Odyssey::TextureColorTests;
    // The conversions are OpenColorIO transforms, compiled on first use.
    if (!TestTrue(TEXT("OpenColorIO converts between linear and sRGB encoded colors"), FOdysseyColorSpaceShader::IsAvailable()))
    {
        return false;
    }
    TStrongObjectPtr<UTexture2D> Texture(MakeTexture(true));
    UOdysseyTextureLayerStackUserData* UserData = MakeUserData(Texture.Get());
    UOdysseyTextureLayerStack* Stack = UserData->LayerStack;
    UOdysseyTextureLayerImageRaster* Layer = CastChecked<UOdysseyTextureLayerImageRaster>(Stack->GetCurrentLayer());
    TestTrue(TEXT("The texture is sRGB"), Stack->IsSRGB());
    TestFalse(TEXT("Layers are blended with encoded colors by default"), Stack->UsesLinearCompositing());
    TestEqual(TEXT("The layer holds decoded colors"), Layer->GetTiledImage()->GetFormat(), EOdysseyTiledImageFormat::RGBA16F);

    CompareBytes(*this, TEXT("A single layer writes the same bytes"), PatternColor, ReadTextureBytes(Texture.Get(), Stack), 0);
    UserData->bLinearCompositing = true;
    Stack->ColorSettingsChanged();
    CompareBytes(*this, TEXT("A single layer writes the same bytes when blended linearly"), PatternColor, ReadTextureBytes(Texture.Get(), Stack), 0);

    AddHalfOpaqueTopLayer(Stack);
    UserData->bLinearCompositing = false;
    Stack->ColorSettingsChanged();
    CompareBytes(*this, TEXT("Encoded colors are blended as bytes"),
        [](int32 X, int32 Y) { return Lerp(PatternColor(X, Y), TopColor); }, ReadTextureBytes(Texture.Get(), Stack), 1);
    UserData->bLinearCompositing = true;
    Stack->ColorSettingsChanged();
    CompareBytes(*this, TEXT("Linear colors are blended as light"),
        [](int32 X, int32 Y) { return LerpLinear(PatternColor(X, Y), TopColor); }, ReadTextureBytes(Texture.Get(), Stack), 1);

    // A linear texture is not affected by the option: its bytes are linear colors.
    TStrongObjectPtr<UTexture2D> LinearTexture(MakeTexture(false));
    UOdysseyTextureLayerStackUserData* LinearData = MakeUserData(LinearTexture.Get());
    TestFalse(TEXT("A linear texture is not sRGB"), LinearData->LayerStack->IsSRGB());
    TestEqual(TEXT("A linear layer holds its bytes"), CastChecked<UOdysseyTextureLayerImageRaster>(LinearData->LayerStack->GetCurrentLayer())->GetTiledImage()->GetFormat(), EOdysseyTiledImageFormat::RGBA8);
    CompareBytes(*this, TEXT("A linear texture writes the same bytes"), PatternColor, ReadTextureBytes(LinearTexture.Get(), LinearData->LayerStack), 0);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOdysseyTextureColorSRGBChangeTest, "Odyssey.Texture.Color.SRGBChange", Odyssey::TextureColorTests::Flags)

/** Changing the sRGB setting of a texture keeps the bytes of its layers by default, or their colors when asked. */
bool FOdysseyTextureColorSRGBChangeTest::RunTest(const FString& Parameters)
{
    using namespace Odyssey::TextureColorTests;
    {
        TStrongObjectPtr<UTexture2D> Texture(MakeTexture(true));
        UOdysseyTextureLayerStackUserData* UserData = MakeUserData(Texture.Get());
        UOdysseyTextureLayerImageRaster* Layer = CastChecked<UOdysseyTextureLayerImageRaster>(UserData->LayerStack->GetCurrentLayer());
        TestEqual(TEXT("Bytes are kept by default"), UserData->SRGBChangePolicy, EOdysseyTextureSRGBChangePolicy::KeepBytes);
        SetSRGB(Texture.Get(), false);
        TestEqual(TEXT("The layer holds the bytes as linear colors"), Layer->GetTiledImage()->GetFormat(), EOdysseyTiledImageFormat::RGBA8);
        CompareBytes(*this, TEXT("Bytes kept when sRGB is turned off"), PatternColor, ReadTextureBytes(Texture.Get(), UserData->LayerStack), 0);
        SetSRGB(Texture.Get(), true);
        TestEqual(TEXT("The layer holds decoded colors again"), Layer->GetTiledImage()->GetFormat(), EOdysseyTiledImageFormat::RGBA16F);
        CompareBytes(*this, TEXT("Bytes kept when sRGB is turned on"), PatternColor, ReadTextureBytes(Texture.Get(), UserData->LayerStack), 0);
    }
    {
        TStrongObjectPtr<UTexture2D> Texture(MakeTexture(true));
        UOdysseyTextureLayerStackUserData* UserData = MakeUserData(Texture.Get());
        UserData->SRGBChangePolicy = EOdysseyTextureSRGBChangePolicy::KeepColors;
        SetSRGB(Texture.Get(), false);
        // Linear 8-bit bytes cannot hold every dark color: they are rounded.
        CompareBytes(*this, TEXT("Colors kept when sRGB is turned off"),
            [](int32 X, int32 Y) { return FLinearColor::FromSRGBColor(PatternColor(X, Y)).ToFColor(false); },
            ReadTextureBytes(Texture.Get(), UserData->LayerStack), 1);
    }
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOdysseyTextureColorOffCanvasTest, "Odyssey.Texture.Color.SRGBChangeKeepsOffCanvas", Odyssey::TextureColorTests::Flags)

/** Pixels outside the canvas, e.g. left by a shrunk canvas, are kept and converted with the others when the format of the layer changes. */
bool FOdysseyTextureColorOffCanvasTest::RunTest(const FString& Parameters)
{
    using namespace Odyssey::TextureColorTests;
    TStrongObjectPtr<UTexture2D> Texture(MakeTexture(true));
    UOdysseyTextureLayerStackUserData* UserData = MakeUserData(Texture.Get());
    UserData->SRGBChangePolicy = EOdysseyTextureSRGBChangePolicy::KeepColors;
    UOdysseyTextureLayerImageRaster* Layer = CastChecked<UOdysseyTextureLayerImageRaster>(UserData->LayerStack->GetCurrentLayer());
    UOdysseyTiledImage* Image = const_cast<UOdysseyTiledImage*>(Layer->GetTiledImage());
    if (!TestNotNull(TEXT("The layer holds a tiled image"), Image))
    {
        return false;
    }

    // Gray pixels far outside the canvas.
    const FIntPoint OffCanvas(130, 140);
    ULIS::FBlock Gray(16, 16, ULIS::Format_RGBAF);
    for (int32 Index = 0; Index < 16 * 16; ++Index)
    {
        float* Pixel = reinterpret_cast<float*>(Gray.Bits()) + Index * 4;
        Pixel[0] = Pixel[1] = Pixel[2] = 0.5f;
        Pixel[3] = 1.0f;
    }
    Odyssey::TiledImage::CopyFromBlock(*Image, Gray, FIntRect(0, 0, 16, 16), OffCanvas);

    SetSRGB(Texture.Get(), false);
    TestEqual(TEXT("The layer was converted"), Image->GetFormat(), EOdysseyTiledImageFormat::RGBA8);
    TestTrue(TEXT("Tiles outside the canvas are kept"), Image->GetTileBounds().Contains(OffCanvas));
    TSharedPtr<ULIS::FBlock> Read = Odyssey::TiledImage::CopyToBlock(*Image, FIntRect(OffCanvas, OffCanvas + FIntPoint(1, 1)), ULIS::Format_RGBAF);
    if (TestTrue(TEXT("Read the pixel outside the canvas"), Read.IsValid()))
    {
        const float* Pixel = reinterpret_cast<const float*>(Read->Bits());
        TestTrue(FString::Printf(TEXT("Pixels outside the canvas keep their colors, got %f"), Pixel[0]), FMath::IsNearlyEqual(Pixel[0], 0.5f, 1.0f / 255.0f));
        TestEqual(TEXT("Pixels outside the canvas keep their alpha"), Pixel[3], 1.0f);
    }
    CompareBytes(*this, TEXT("Colors kept in the canvas"),
        [](int32 X, int32 Y) { return FLinearColor::FromSRGBColor(PatternColor(X, Y)).ToFColor(false); },
        ReadTextureBytes(Texture.Get(), UserData->LayerStack), 1);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOdysseyTextureColorMergeTest, "Odyssey.Texture.Color.Merge", Odyssey::TextureColorTests::Flags)

/**
 * Merging layers writes the colors they were blended into, as the texture showed them,
 * whether the layers of the sRGB texture are blended with encoded or linear colors. The merge is undoable.
 */
bool FOdysseyTextureColorMergeTest::RunTest(const FString& Parameters)
{
    using namespace Odyssey::TextureColorTests;
    if (!TestTrue(TEXT("OpenColorIO converts between linear and sRGB encoded colors"), FOdysseyColorSpaceShader::IsAvailable()))
    {
        return false;
    }
    for (bool bLinearCompositing : { false, true })
    {
        TStrongObjectPtr<UTexture2D> Texture(MakeTexture(true));
        UOdysseyTextureLayerStackUserData* UserData = MakeUserData(Texture.Get());
        UOdysseyTextureLayerStack* Stack = UserData->LayerStack;
        UserData->bLinearCompositing = bLinearCompositing;
        Stack->ColorSettingsChanged();
        UOdysseyLayer* Bottom = Stack->GetCurrentLayer();
        UOdysseyTextureLayerImageRaster* Top = AddHalfOpaqueTopLayer(Stack);
        const TArray<FColor> Blended = ReadTextureBytes(Texture.Get(), Stack);
        const int32 LayerCount = Stack->GetLayers().Num();
        TFunction<FColor(int32, int32)> Expected = [&Blended](int32 X, int32 Y) { return Blended[Y * Width + X]; };

        UOdysseyLayer* Merged = nullptr;
        {
            FScopedTransaction Transaction(NSLOCTEXT("OdysseyTextureColorTests", "Merge", "Merge layers"));
            Merged = Stack->MergeLayers({ Bottom, Top });
        }
        const TCHAR* Mode = bLinearCompositing ? TEXT("linear compositing") : TEXT("encoded compositing");
        if (!TestNotNull(FString::Printf(TEXT("Merge the layers with %s"), Mode), Merged))
        {
            return false;
        }
        TestEqual(TEXT("The merged layer replaces both layers"), Stack->GetLayers().Num(), LayerCount - 1);
        CompareBytes(*this, *FString::Printf(TEXT("The merged layer holds the blended colors with %s"), Mode), Expected, ReadTextureBytes(Texture.Get(), Stack), 1);

        TestTrue(TEXT("Undo the merge"), GEditor->UndoTransaction());
        TestEqual(TEXT("Undo restores the layers"), Stack->GetLayers().Num(), LayerCount);
        CompareBytes(*this, *FString::Printf(TEXT("Undone merge with %s"), Mode), Expected, ReadTextureBytes(Texture.Get(), Stack), 1);
    }
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOdysseyTextureColorRenderSideEffectTest, "Odyssey.Texture.Color.RenderDoesNotConvert", Odyssey::TextureColorTests::Flags)

/**
 * Rendering a raster layer has no side effect: a change of the sRGB setting not yet applied leaves the tiled image as it is
 * until the layer applies it, at an explicit point such as ColorSettingsChanged().
 */
bool FOdysseyTextureColorRenderSideEffectTest::RunTest(const FString& Parameters)
{
    using namespace Odyssey::TextureColorTests;
    TStrongObjectPtr<UTexture2D> Texture(MakeTexture(true));
    UOdysseyTextureLayerStackUserData* UserData = MakeUserData(Texture.Get());
    UOdysseyTextureLayerImageRaster* Layer = CastChecked<UOdysseyTextureLayerImageRaster>(UserData->LayerStack->GetCurrentLayer());
    TestEqual(TEXT("An 8-bit sRGB layer holds decoded colors"), Layer->GetTiledImage()->GetFormat(), EOdysseyTiledImageFormat::RGBA16F);

    // Change the setting without notifying the layer stack.
    Texture->SRGB = false;
    IOdysseyTextureRenderingAbility::FRenderFunction RenderFunction;
    TestTrue(TEXT("The layer renders its tiles"), Layer->BuildRenderPipeline(RenderFunction));
    TestEqual(TEXT("Rendering does not convert the tiles"), Layer->GetTiledImage()->GetFormat(), EOdysseyTiledImageFormat::RGBA16F);

    Layer->ColorSettingsChanged();
    TestEqual(TEXT("Applying the setting converts the tiles"), Layer->GetTiledImage()->GetFormat(), EOdysseyTiledImageFormat::RGBA8);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOdysseyTextureColorVectorLayerLoadTest, "Odyssey.Texture.Color.VectorLayerSRGBAfterLoad", Odyssey::TextureColorTests::Flags)

/**
 * The render of a vector layer holds the bytes of the texture: they are decoded into its tiles if they are sRGB encoded colors,
 * after loading as after creation, so that the texture gets the bytes of the render back and a reloaded vector layer is not shown brighter.
 */
bool FOdysseyTextureColorVectorLayerLoadTest::RunTest(const FString& Parameters)
{
    using namespace Odyssey::TextureColorTests;
    // Without colors, the scene is drawn as an opaque fill of its monochrome color.
    static const FColor Monochrome(160, 160, 160, 255);
    for (const bool bSRGB : { true, false })
    {
        TStrongObjectPtr<UTexture2D> Texture(MakeTexture(bSRGB));
        UOdysseyTextureLayerStackUserData* UserData = MakeUserData(Texture.Get());
        UOdysseyTextureLayerStack* Stack = UserData->LayerStack;
        UOdysseyTextureLayerImageVector* Layer = CastChecked<UOdysseyTextureLayerImageVector>(Stack->AddLayer(UOdysseyTextureLayerImageVector::StaticClass()));
        Layer->SetIsColored(false);
        const TCHAR* Encoding = bSRGB ? TEXT("sRGB texture") : TEXT("linear texture");
        const EOdysseyTiledImageFormat ExpectedFormat = bSRGB ? EOdysseyTiledImageFormat::RGBA16F : EOdysseyTiledImageFormat::RGBA8;

        CompareBytes(*this, *FString::Printf(TEXT("A new vector layer of a %s writes the bytes of its render"), Encoding),
            [](int32 X, int32 Y) { return Monochrome; }, ReadTextureBytes(Texture.Get(), Stack), 1);
        if (!TestNotNull(FString::Printf(TEXT("The vector layer of a %s holds its render in a tiled image"), Encoding), Layer->GetVectorContent().GetImage()))
        {
            return false;
        }
        TestEqual(FString::Printf(TEXT("The render of a new vector layer of a %s is decoded as the texture bytes are"), Encoding), Layer->GetVectorContent().GetImage()->GetFormat(), ExpectedFormat);

        Layer->PostLoad();
        CompareBytes(*this, *FString::Printf(TEXT("A loaded vector layer of a %s writes the bytes of its render"), Encoding),
            [](int32 X, int32 Y) { return Monochrome; }, ReadTextureBytes(Texture.Get(), Stack), 1);
        TestEqual(FString::Printf(TEXT("The render of a loaded vector layer of a %s is decoded as the texture bytes are"), Encoding), Layer->GetVectorContent().GetImage()->GetFormat(), ExpectedFormat);
    }
    return true;
}

#endif
