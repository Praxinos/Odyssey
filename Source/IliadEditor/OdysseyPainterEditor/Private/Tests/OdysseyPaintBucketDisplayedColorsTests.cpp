// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#include "Tools/RasterPaintBucketTool/OdysseyPainterEditorRasterPaintBucketTool.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "Engine/Texture2D.h"
#include "Misc/AutomationTest.h"
#include "TextureCompiler.h"
#include "UObject/Package.h"
#include "UObject/StrongObjectPtr.h"

#include "OdysseyColorSpaceShader.h"
#include "OdysseyRenderContext.h"
#include "OdysseyTextureLayerImageRaster.h"
#include "OdysseyTextureLayerStack.h"
#include "OdysseyTextureLayerStackUserData.h"

namespace Odyssey::PaintBucketDisplayedColorsTests
{
constexpr EAutomationTestFlags TestFlags = EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter;
constexpr int32 Width = 33;
constexpr int32 Height = 21;

/** Returns the bytes of the texture at a pixel: opaque, covering dark and bright values. */
FColor PatternColor(int32 X, int32 Y)
{
    return FColor(uint8((X * 7) % 256), uint8((Y * 11) % 256), uint8((X * 3 + Y * 5) % 256), 255);
}

/** Creates an sRGB texture holding the pattern, with a layer stack of a single raster layer. */
UOdysseyTextureLayerStackUserData* MakeTexture(TStrongObjectPtr<UTexture2D>& OutTexture)
{
    TArray<FColor> Pixels;
    Pixels.Reserve(Width * Height);
    for (int32 Y = 0; Y < Height; ++Y)
    {
        for (int32 X = 0; X < Width; ++X)
        {
            Pixels.Add(PatternColor(X, Y));
        }
    }
    OutTexture.Reset(NewObject<UTexture2D>(GetTransientPackage(), NAME_None, RF_Public | RF_Transactional));
    OutTexture->Source.Init(Width, Height, 1, 1, TSF_BGRA8, reinterpret_cast<const uint8*>(Pixels.GetData()));
    OutTexture->SRGB = true;
    OutTexture->MipGenSettings = TMGS_NoMipmaps;
    OutTexture->UpdateResource();
    FTextureCompilingManager::Get().FinishCompilation({ OutTexture.Get() });

    UOdysseyTextureLayerStackUserData* UserData = NewObject<UOdysseyTextureLayerStackUserData>(OutTexture.Get());
    OutTexture->AddAssetUserData(UserData);
    UserData->InitWithDefaultLayerStack();
    UserData->LayerStack->SetTextureUpdateMode(EOdysseyTextureLayerStackTextureUpdateMode::Manually);
    return UserData;
}

/** Checks that a block holds the bytes of the texture, as floats. */
void CheckDisplayedColors(FAutomationTestBase& Test, const FString& What, const TSharedPtr<::ULIS::FBlock>& Block)
{
    if (!Test.TestTrue(FString::Printf(TEXT("%s: render read back"), *What), Block.IsValid() && Block->Width() == Width && Block->Height() == Height))
    {
        return;
    }
    // Half a byte, plus the precision of the 16-bit float render.
    const float Tolerance = 0.5f / 255.0f + 0.001f;
    for (int32 Y = 0; Y < Height; ++Y)
    {
        for (int32 X = 0; X < Width; ++X)
        {
            const FColor Expected = PatternColor(X, Y);
            const ::ULIS::FColor Actual = Block->Color(static_cast<uint16>(X), static_cast<uint16>(Y)).ToFormat(::ULIS::Format_RGBAF);
            if (!FMath::IsNearlyEqual(Actual.RedF(), Expected.R / 255.0f, Tolerance)
                || !FMath::IsNearlyEqual(Actual.GreenF(), Expected.G / 255.0f, Tolerance)
                || !FMath::IsNearlyEqual(Actual.BlueF(), Expected.B / 255.0f, Tolerance))
            {
                Test.AddError(FString::Printf(TEXT("%s: pixel (%d, %d) expected %s, got (%f, %f, %f)"), *What, X, Y,
                    *Expected.ToString(), Actual.RedF(), Actual.GreenF(), Actual.BlueF()));
                return;
            }
        }
    }
}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOdysseyPaintBucketDisplayedColorsTest, "Odyssey.PainterEditor.PaintBucket.DisplayedColors", Odyssey::PaintBucketDisplayedColorsTests::TestFlags)
/**
 * The paint bucket compares the colors the views display: rendering a layer or the whole stack of an sRGB texture gives
 * the bytes of the texture, whether the layers are blended with encoded or linear colors.
 */
bool FOdysseyPaintBucketDisplayedColorsTest::RunTest(const FString& Parameters)
{
    using namespace Odyssey::PaintBucketDisplayedColorsTests;
    if (!TestTrue(TEXT("OpenColorIO converts between linear and sRGB encoded colors"), FOdysseyColorSpaceShader::IsAvailable()))
    {
        return false;
    }
    TStrongObjectPtr<UTexture2D> Texture;
    UOdysseyTextureLayerStackUserData* UserData = MakeTexture(Texture);
    UOdysseyTextureLayerStack* Stack = UserData->LayerStack;
    const UOdysseyLayer* Layer = Stack->GetCurrentLayer();
    const FOdysseyRenderContext RenderContext;
    for (bool bLinearCompositing : { false, true })
    {
        UserData->bLinearCompositing = bLinearCompositing;
        Stack->ColorSettingsChanged();
        const FString Mode = bLinearCompositing ? TEXT("linear compositing") : TEXT("encoded compositing");
        TestEqual(FString::Printf(TEXT("With %s, a render of the stack shows the texture bytes"), *Mode),
            Stack->GetDisplayColorConversion(nullptr), EOdysseyColorConversion::None);
        TestEqual(FString::Printf(TEXT("With %s, a render of a layer is encoded only when it is linear"), *Mode),
            Stack->GetDisplayColorConversion(Layer), bLinearCompositing ? EOdysseyColorConversion::Encode : EOdysseyColorConversion::None);
        CheckDisplayedColors(*this, FString::Printf(TEXT("Layer with %s"), *Mode),
            UOdysseyPainterEditorRasterPaintBucketTool::RenderDisplayedColors(*Stack, Layer, RenderContext));
        CheckDisplayedColors(*this, FString::Printf(TEXT("Stack with %s"), *Mode),
            UOdysseyPainterEditorRasterPaintBucketTool::RenderDisplayedColors(*Stack, nullptr, RenderContext));
    }
    return true;
}

#endif
