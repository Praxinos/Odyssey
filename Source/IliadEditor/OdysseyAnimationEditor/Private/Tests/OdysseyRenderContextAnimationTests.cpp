// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019
#include "OdysseyRenderContext.h"
#include "OdysseyLayerStackRenderContext.h"
#include "OdysseyAnimationRenderContext.h"
#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "OdysseyAnimation.h"
#include "OdysseyAnimationCellImageRaster.h"
#include "OdysseyLayerStack.h"
#include "OdysseyLayer.h"
#include "OdysseyLighttable.h"
#include "Engine/TextureRenderTarget2D.h"
#include "RenderGraphUtils.h"
#include "RenderingThread.h"
#include "TextureResource.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/Package.h"

namespace Odyssey::AnimationTiledTests
{
UOdysseyAnimation* MakeAnimation(UObject* Outer, EOdysseyAnimationFormat Format);
UOdysseyAnimationCellImageRaster* Cell(UOdysseyAnimation* Animation, int32 Index);
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOdysseyRenderContextAnimationTest, "Odyssey.Rendering.Context.AnimationIntegration",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FOdysseyRenderContextAnimationTest::RunTest(const FString& Parameters)
{
    using namespace Odyssey::AnimationTiledTests;
    TStrongObjectPtr<UOdysseyAnimation> Animation(MakeAnimation(GetTransientPackage(), EOdysseyAnimationFormat::BGRA8));
    UOdysseyAnimationCellImageRaster* First = Cell(Animation.Get(), 0);
    UOdysseyAnimationCellImageRaster* Second = Cell(Animation.Get(), 1);
    FOdysseyRenderContext Context;
    int32 DecoratorCalls = 0;
    Context.Overrides.FindOrAdd(First->GetRenderingId()).RenderDecorator = [&DecoratorCalls](IOdysseyTextureRenderingAbility::FRenderFunction Original)
    {
        ++DecoratorCalls;
        return Original;
    };
    FOdysseyRenderOverride& SecondOverride = Context.Overrides.FindOrAdd(Second->GetRenderingId());
    SecondOverride.RenderReplacement = [](FRDGBuilder& Graph, ERHIFeatureLevel::Type, FRDGTextureRef Target,
        const FIntRect&, const FIntRect& Rect, const FMatrix&)
    {
        AddClearRenderTargetPass(Graph, Target, FLinearColor::Green, Rect);
    };
    TSharedPtr<FOdysseyLighttable> Lighttable = MakeShared<FOdysseyLighttable>();
    Lighttable->bIsActivated = true;
    Lighttable->PreviousKeys[0].bIsActivated = true;
    Lighttable->PreviousKeys[0].Opacity = 100.f;
    Context.FindOrAddExtension<FOdysseyLayerStackRenderContext>().Overrides.FindOrAdd(Second->GetLayer()->GetRenderingId()).Lighttable = Lighttable;
    IOdysseyTextureRenderingAbility::FRenderFunction Draw;
    TestTrue(TEXT("Build lighttable with overrides"), Animation->BuildRenderPipeline(FFrameNumber(2), EOdysseyRenderingType::Editor, Draw, {}, {}, &Context));
    TestEqual(TEXT("Preview excluded from lighttable by default"), DecoratorCalls, 0);
    Context.FindOrAddExtension<FOdysseyLayerStackRenderContext>().Overrides.FindOrAdd(First->GetRenderingId()).bApplyToLighttable = true;
    FOdysseyAnimationRenderContext& AnimationContext = Context.FindOrAddExtension<FOdysseyAnimationRenderContext>();
    AnimationContext.Frame = FFrameNumber(2);
    TestTrue(TEXT("Build lighttable relative to context frame"), Animation->BuildRenderPipeline(FFrameNumber(0), EOdysseyRenderingType::Editor, Draw, {}, {}, &Context));
    TestEqual(TEXT("Lighttable decorator resolved exactly once"), DecoratorCalls, 1);
    TestFalse(TEXT("Asset lighttable remains disabled"), Second->GetLayer()->GetLighttable().bIsActivated);
    TStrongObjectPtr<UTextureRenderTarget2D> Target(NewObject<UTextureRenderTarget2D>());
    Target->InitCustomFormat(73, 67, PF_A32B32G32R32F, true);
    Target->UpdateResourceImmediate(true);
    Animation->Render_GameThread(Target.Get(), FFrameNumber(0), EOdysseyRenderingType::Render, {}, &Context);
    Context.Overrides.Empty(); // Submitted commands must own the resolved preview.
    FlushRenderingCommands();
    TArray<FLinearColor> Pixels;
    FReadSurfaceDataFlags Flags(RCM_MinMax);
    Flags.SetLinearToGamma(false);
    TestTrue(TEXT("Read preview"), Target->GameThread_GetRenderTargetResource()->ReadLinearColorPixels(Pixels, Flags));
    TestTrue(TEXT("Cell replacement survives context destruction"), Pixels.Num() > 0 && Pixels[0].Equals(FLinearColor::Green, 0.001f));
    Animation->Render_GameThread(Target.Get(), FFrameNumber(0), EOdysseyRenderingType::Render);
    FlushRenderingCommands();
    Target->GameThread_GetRenderTargetResource()->ReadLinearColorPixels(Pixels, Flags);
    TestTrue(TEXT("Cell data was never modified"), Pixels.Num() > 0 && !Pixels[0].Equals(FLinearColor::Green, 0.001f));
    return true;
}
#endif
