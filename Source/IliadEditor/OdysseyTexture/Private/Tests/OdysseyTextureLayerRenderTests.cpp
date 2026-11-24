// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#if WITH_DEV_AUTOMATION_TESTS
#include "Engine/Texture2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Misc/AutomationTest.h"
#include "RenderingThread.h"
#include "TextureResource.h"
#include "UObject/Package.h"
#include "UObject/StrongObjectPtr.h"

#include "OdysseyRenderingType.h"
#include "OdysseyTextureLayerImageRaster.h"
#include "OdysseyTextureLayerImageVector.h"
#include "OdysseyTextureLayerStack.h"
#include "OdysseyVectorContent.h"

namespace Odyssey::TextureLayerTiledImageTests
{
UTexture2D* MakeTexture(UObject* Outer, ETextureSourceFormat Format);
UOdysseyTextureLayerImageRaster* MakeLayer(UTexture2D* Texture);
}

namespace Odyssey::TextureLayerRenderTests
{
    /** Creates a render target matching the test textures, cleared to a color no layer renders */
    UTextureRenderTarget2D* MakeTarget()
    {
        UTextureRenderTarget2D* Target = NewObject<UTextureRenderTarget2D>();
        Target->ClearColor = FLinearColor(1.f, 0.f, 1.f, 1.f);
        Target->InitCustomFormat(73, 67, PF_A32B32G32R32F, true);
        Target->UpdateResourceImmediate(true);
        return Target;
    }

    /** Reads back the pixels of a render target, once every pending render command ran */
    TArray<FLinearColor> ReadPixels(UTextureRenderTarget2D* Target)
    {
        FlushRenderingCommands();
        TArray<FLinearColor> Pixels;
        FReadSurfaceDataFlags Flags(RCM_MinMax);
        Flags.SetLinearToGamma(false);
        Target->GameThread_GetRenderTargetResource()->ReadLinearColorPixels(Pixels, Flags);
        return Pixels;
    }
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOdysseyTextureLayerVectorRenderTest, "Odyssey.Texture.Layer.VectorRender",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

/**
 * Renders a vector layer, whose scene is drawn as an opaque monochrome fill: the layer renders the scene through the tiles holding its render,
 * and renders the same pixels again from these tiles.
 */
bool FOdysseyTextureLayerVectorRenderTest::RunTest(const FString& Parameters)
{
    using namespace Odyssey::TextureLayerTiledImageTests;
    using namespace Odyssey::TextureLayerRenderTests;

    TStrongObjectPtr<UTexture2D> Texture(MakeTexture(GetTransientPackage(), TSF_BGRA8));
    UOdysseyLayerStack* Stack = MakeLayer(Texture.Get())->GetLayerStack();
    UOdysseyTextureLayerImageVector* Layer = CastChecked<UOdysseyTextureLayerImageVector>(Stack->AddLayer(UOdysseyTextureLayerImageVector::StaticClass()));
    // Without colors, the whole canvas is filled with the opaque monochrome color of the scene.
    Layer->SetIsColored(false);

    TStrongObjectPtr<UTextureRenderTarget2D> Target(MakeTarget());
    Layer->Render_GameThread(Target.Get(), EOdysseyRenderingType::Render);
    const TArray<FLinearColor> Pixels = ReadPixels(Target.Get());
    if (!TestEqual(TEXT("Render reads all pixels"), Pixels.Num(), 73 * 67))
        return false;
    TestNotNull(TEXT("The layer holds the render of its scene in a tiled image"), Layer->GetVectorContent().GetTiledImage());

    const FLinearColor Gray = Pixels[0];
    TestTrue(FString::Printf(TEXT("The layer renders an opaque gray, got %s"), *Gray.ToString()),
        FMath::IsNearlyEqual(Gray.A, 1.0f, 0.001f) && FMath::IsNearlyEqual(Gray.R, Gray.G, 0.001f) && FMath::IsNearlyEqual(Gray.R, Gray.B, 0.001f)
        && Gray.R > 0.2f && Gray.R < 0.8f);
    for (const int32 PixelIndex : { 36 + 33 * 73, 72 + 66 * 73 })
        TestTrue(FString::Printf(TEXT("Pixel %d is rendered as the others"), PixelIndex), Pixels[PixelIndex].Equals(Gray, 0.001f));

    // A new target, so that a render writing nothing cannot pass for the first one.
    TStrongObjectPtr<UTextureRenderTarget2D> SecondTarget(MakeTarget());
    Layer->Render_GameThread(SecondTarget.Get(), EOdysseyRenderingType::Render);
    const TArray<FLinearColor> SecondPixels = ReadPixels(SecondTarget.Get());
    if (!TestEqual(TEXT("Second render reads all pixels"), SecondPixels.Num(), Pixels.Num()))
        return false;
    for (const int32 PixelIndex : { 0, 36 + 33 * 73, 72 + 66 * 73 })
        TestTrue(FString::Printf(TEXT("Pixel %d is rendered again from the tiles"), PixelIndex), SecondPixels[PixelIndex].Equals(Pixels[PixelIndex], 0.001f));

    return true;
}

#endif
