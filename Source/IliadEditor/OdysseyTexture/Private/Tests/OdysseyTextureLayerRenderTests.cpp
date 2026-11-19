// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#if WITH_DEV_AUTOMATION_TESTS
#include "Engine/Texture2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "HAL/Event.h"
#include "HAL/IConsoleManager.h"
#include "HAL/PlatformProcess.h"
#include "Misc/AutomationTest.h"
#include "Misc/ScopeExit.h"
#include "RenderingThread.h"
#include "TextureCompiler.h"
#include "TextureResource.h"
#include "UObject/Package.h"
#include "UObject/StrongObjectPtr.h"

#include "OdysseyRenderingType.h"
#include "OdysseyTextureLayerImageRaster.h"
#include "OdysseyTextureLayerStack.h"

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

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOdysseyTextureLayerRenderPendingResourceTest, "Odyssey.Texture.Layer.RenderWhileResourceIsPending",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

/**
 * Renders a texture layer whose texture resource was just recreated, while the render thread has not created its RHI texture yet.
 * The layer must read the RHI texture when its render function runs on the render thread, not when it is built on the game thread.
 */
bool FOdysseyTextureLayerRenderPendingResourceTest::RunTest(const FString& Parameters)
{
    using namespace Odyssey::TextureLayerTiledImageTests;
    using namespace Odyssey::TextureLayerRenderTests;

    IConsoleVariable* TiledRendering = IConsoleManager::Get().FindConsoleVariable(TEXT("Odyssey.Texture.UseTiledRendering"));
    if (!TestNotNull(TEXT("Tiled rendering console variable"), TiledRendering))
        return false;

    const int32 PreviousTiledRendering = TiledRendering->GetInt();
    TiledRendering->Set(0, ECVF_SetByCode);
    ON_SCOPE_EXIT { TiledRendering->Set(PreviousTiledRendering, ECVF_SetByCode); };

    TStrongObjectPtr<UTexture2D> Texture(MakeTexture(GetTransientPackage(), TSF_BGRA8));
    UOdysseyTextureLayerImageRaster* Layer = MakeLayer(Texture.Get());
    UOdysseyLayerStack* Stack = Layer->GetLayerStack();
    TStrongObjectPtr<UTextureRenderTarget2D> Target(MakeTarget());

    Stack->Render_GameThread(Target.Get(), EOdysseyRenderingType::Render);
    const TArray<FLinearColor> Expected = ReadPixels(Target.Get());
    UTexture2D* LayerTexture = Layer->GetRenderTexture();
    if (!TestEqual(TEXT("Reference render reads all pixels"), Expected.Num(), 73 * 67) || !TestNotNull(TEXT("Layer render texture"), LayerTexture))
        return false;

    // Nothing may compile or flush the render thread while it is held below
    FTextureCompilingManager::Get().FinishCompilation({ LayerTexture });
    LayerTexture->BlockOnAnyAsyncBuild();
    FlushRenderingCommands();

    // Hold the render thread so that the recreated resource stays without RHI texture while the pipeline is built
    FEvent* RenderThreadGate = FPlatformProcess::GetSynchEventFromPool(true);
    ON_SCOPE_EXIT { FPlatformProcess::ReturnSynchEventToPool(RenderThreadGate); };
    ENQUEUE_RENDER_COMMAND(OdysseyHoldRenderThread)([RenderThreadGate](FRHICommandListImmediate&)
    {
        // The timeout only avoids a dead lock if something flushes the render thread meanwhile
        RenderThreadGate->Wait(FTimespan::FromSeconds(10));
    });

    // A new target, so that a render writing nothing cannot pass for the reference one
    TStrongObjectPtr<UTextureRenderTarget2D> PendingTarget(MakeTarget());

    LayerTexture->UpdateResource();
    const bool bRHIIsPending = !LayerTexture->GetResource() || !LayerTexture->GetResource()->TextureRHI;
    IOdysseyTextureRenderingAbility::FRenderFunction LayerRenderFunction;
    const bool bLayerPipelineBuilt = Layer->BuildRenderPipeline(LayerRenderFunction);
    Stack->Render_GameThread(PendingTarget.Get(), EOdysseyRenderingType::Render);
    RenderThreadGate->Trigger();

    TestTrue(TEXT("The pipeline is built before the render thread creates the RHI texture"), bRHIIsPending);
    TestTrue(TEXT("The layer builds its render function while its RHI texture is pending"), bLayerPipelineBuilt && LayerRenderFunction);
    const TArray<FLinearColor> Pixels = ReadPixels(PendingTarget.Get());
    if (!TestEqual(TEXT("Render reads all pixels"), Pixels.Num(), Expected.Num()))
        return false;

    for (const int32 PixelIndex : { 0, 36 + 33 * 73, 72 + 66 * 73 })
        TestTrue(FString::Printf(TEXT("Pixel %d matches the reference render"), PixelIndex), Pixels[PixelIndex].Equals(Expected[PixelIndex], 0.001f));

    return true;
}

#endif
