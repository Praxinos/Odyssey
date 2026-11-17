// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019
#if WITH_DEV_AUTOMATION_TESTS
#include "Engine/TextureRenderTarget2D.h"
#include "HAL/FileManager.h"
#include "Misc/AutomationTest.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "RenderGraphUtils.h"
#include "RenderingThread.h"
#include "TextureResource.h"
#include "UObject/Package.h"
#include "UObject/SoftObjectPath.h"
#include "UObject/StrongObjectPtr.h"

#include "OdysseyAnimation.h"
#include "OdysseyAnimationCellImageRaster.h"
#include "OdysseyAnimationExport.h"
#include "OdysseyAnimationLayer.h"
#include "OdysseyAnimationRenderContext.h"
#include "OdysseyExportImage.h"
#include "OdysseyLayer.h"
#include "OdysseyLayerCellImageStagger.h"
#include "OdysseyLayerStack.h"
#include "OdysseyLayerStackRenderContext.h"
#include "OdysseyLighttable.h"
#include "OdysseyRenderContext.h"

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
    Context.Type = EOdysseyRenderingType::Editor;
    Context.FindOrAddExtension<FOdysseyAnimationRenderContext>().Frame = FFrameNumber(2);
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
    TestTrue(TEXT("Build lighttable with overrides"), Animation->BuildRenderPipeline(Draw, {}, &Context));
    TestEqual(TEXT("Preview excluded from lighttable by default"), DecoratorCalls, 0);
    Context.FindOrAddExtension<FOdysseyLayerStackRenderContext>().Overrides.FindOrAdd(First->GetRenderingId()).bApplyToLighttable = true;
    FOdysseyAnimationRenderContext& AnimationContext = Context.FindOrAddExtension<FOdysseyAnimationRenderContext>();
    AnimationContext.Frame = FFrameNumber(2);
    TestTrue(TEXT("Build lighttable relative to context frame"), Animation->BuildRenderPipeline(Draw, {}, &Context));
    TestEqual(TEXT("Lighttable decorator resolved exactly once"), DecoratorCalls, 1);
    TestFalse(TEXT("Asset lighttable remains disabled"), Second->GetLayer()->GetLighttable().bIsActivated);
    TStrongObjectPtr<UTextureRenderTarget2D> Target(NewObject<UTextureRenderTarget2D>());
    Target->InitCustomFormat(73, 67, PF_A32B32G32R32F, true);
    Target->UpdateResourceImmediate(true);
    Animation->Render_GameThread(Target.Get(), EOdysseyRenderingType::Render, &Context);
    FOdysseyRenderContext SavedContext = Context;
    Context.Overrides.Empty(); // Submitted commands must own the resolved preview.
    FlushRenderingCommands();
    TArray<FLinearColor> Pixels;
    FReadSurfaceDataFlags Flags(RCM_MinMax);
    Flags.SetLinearToGamma(false);
    TestTrue(TEXT("Read preview"), Target->GameThread_GetRenderTargetResource()->ReadLinearColorPixels(Pixels, Flags));
    TestTrue(TEXT("Cell replacement survives context destruction"), Pixels.Num() > 0 && Pixels[0].Equals(FLinearColor::Green, 0.001f));
    Animation->Render_GameThread(Target.Get(), EOdysseyRenderingType::Render);
    FlushRenderingCommands();
    Target->GameThread_GetRenderTargetResource()->ReadLinearColorPixels(Pixels, Flags);
    TestTrue(TEXT("Cell data was never modified"), Pixels.Num() > 0 && !Pixels[0].Equals(FLinearColor::Green, 0.001f));
    SavedContext.FindOrAddExtension<FOdysseyAnimationRenderContext>().Frame = FFrameNumber(2);
    Animation->Render_GameThread(Target.Get(), EOdysseyRenderingType::Render, &SavedContext);
    FlushRenderingCommands();
    Target->GameThread_GetRenderTargetResource()->ReadLinearColorPixels(Pixels, Flags);
    TestTrue(TEXT("Animation extension selects the nonzero frame"), Pixels.Num() > 0 && Pixels[0].Equals(FLinearColor::Green, 0.001f));
    SavedContext.FindOrAddExtension<FOdysseyAnimationRenderContext>().Frame.Reset();
    Animation->RenderAndResize_GameThread(Target.Get(), EOdysseyRenderingType::Render, &SavedContext);
    FlushRenderingCommands();
    Target->GameThread_GetRenderTargetResource()->ReadLinearColorPixels(Pixels, Flags);
    TestTrue(TEXT("Resize render uses frame zero after a nonzero request"), Pixels.Num() > 0 && !Pixels[0].Equals(FLinearColor::Green, 0.001f));
    TestEqual(TEXT("Copied frame settings remain independent"), Context.GetExtension<FOdysseyAnimationRenderContext>()->Frame.GetValue().Value, 2);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOdysseyAnimationLayerRenderContextTest, "Odyssey.Rendering.Context.AnimationLayerIntegration",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FOdysseyAnimationLayerRenderContextTest::RunTest(const FString& Parameters)
{
    using namespace Odyssey::AnimationTiledTests;
    TStrongObjectPtr<UOdysseyAnimation> Animation(MakeAnimation(GetTransientPackage(), EOdysseyAnimationFormat::BGRA8));
    UOdysseyAnimationLayer* Layer = CastChecked<UOdysseyAnimationLayer>(Cell(Animation.Get(), 0)->GetLayer());
    FOdysseyRenderContext Context;
    Context.FindOrAddExtension<FOdysseyAnimationRenderContext>().Frame = FFrameNumber(-1);
    const FLinearColor Colors[] = { FLinearColor::Red, FLinearColor::Green };
    for (int32 Index = 0; Index < 2; ++Index)
    {
        const FLinearColor Color = Colors[Index];
        Context.Overrides.FindOrAdd(Cell(Animation.Get(), Index)->GetRenderingId()).RenderReplacement =
            [Color](FRDGBuilder& Graph, ERHIFeatureLevel::Type, FRDGTextureRef Destination,
                const FIntRect&, const FIntRect& Rect, const FMatrix&)
            {
                AddClearRenderTargetPass(Graph, Destination, Color, Rect);
            };
    }
    TStrongObjectPtr<UTextureRenderTarget2D> Target(NewObject<UTextureRenderTarget2D>());
    Target->InitCustomFormat(73, 67, PF_A32B32G32R32F, true);
    Target->UpdateResourceImmediate(true);
    const TFunction<FLinearColor()> ReadPreview = [&]()
    {
        Animation->Render_GameThread(Target.Get(), EOdysseyRenderingType::Render, &Context);
        FlushRenderingCommands();
        TArray<FLinearColor> Pixels;
        FReadSurfaceDataFlags Flags(RCM_MinMax);
        Flags.SetLinearToGamma(false);
        if (!Target->GameThread_GetRenderTargetResource()->ReadLinearColorPixels(Pixels, Flags) || Pixels.IsEmpty())
        {
            AddError(TEXT("Unable to read animation layer output"));
            return FLinearColor::Transparent;
        }
        return Pixels[0];
    };
    TestTrue(TEXT("Before the animation range, None clears the output"), ReadPreview().Equals(FLinearColor::Transparent));
    Layer->SetPreBehaviour(EOdysseyLayerImagePostBehaviour::Hold);
    TestTrue(TEXT("Pre Hold renders the first cell"), ReadPreview().Equals(FLinearColor::Red));
    Layer->SetPreBehaviour(EOdysseyLayerImagePostBehaviour::Loop);
    TestTrue(TEXT("Pre Loop renders the last cell"), ReadPreview().Equals(FLinearColor::Green));
    Layer->SetPreBehaviour(EOdysseyLayerImagePostBehaviour::PingPong);
    TestTrue(TEXT("Pre PingPong reflects into the first cell"), ReadPreview().Equals(FLinearColor::Red));
    Context.FindOrAddExtension<FOdysseyAnimationRenderContext>().Frame = FFrameNumber(5);
    TestTrue(TEXT("After the animation range, None clears the output"), ReadPreview().Equals(FLinearColor::Transparent));
    Layer->SetPostBehaviour(EOdysseyLayerImagePostBehaviour::Hold);
    TestTrue(TEXT("Post Hold renders the last cell"), ReadPreview().Equals(FLinearColor::Green));
    Layer->SetPostBehaviour(EOdysseyLayerImagePostBehaviour::Loop);
    TestTrue(TEXT("Post Loop renders the first cell"), ReadPreview().Equals(FLinearColor::Red));
    Layer->SetPostBehaviour(EOdysseyLayerImagePostBehaviour::PingPong);
    Context.FindOrAddExtension<FOdysseyAnimationRenderContext>().Frame = FFrameNumber(7);
    TestTrue(TEXT("Post PingPong reflects into the first cell"), ReadPreview().Equals(FLinearColor::Red));
    const TArray<FGuid> Composition = Layer->GetRenderingComposition(EOdysseyRenderingType::Render, 7);
    TestTrue(TEXT("Rendering composition follows the resolved animation cell"), Composition.Contains(Cell(Animation.Get(), 0)->GetRenderingId()));
    TestFalse(TEXT("Rendering composition excludes the other animation cell"), Composition.Contains(Cell(Animation.Get(), 1)->GetRenderingId()));
    UOdysseyLayerCellImageStagger* Stagger = CastChecked<UOdysseyLayerCellImageStagger>(Layer->AddCell(UOdysseyLayerCellImageStagger::StaticClass()));
    Stagger->SetExposure(4);
    Context.FindOrAddExtension<FOdysseyAnimationRenderContext>().Frame = FFrameNumber(5);
    TestTrue(TEXT("Stagger renders the first referenced cell"), ReadPreview().Equals(FLinearColor::Red));
    Context.FindOrAddExtension<FOdysseyAnimationRenderContext>().Frame = FFrameNumber(7);
    TestTrue(TEXT("Stagger renders the later referenced cell"), ReadPreview().Equals(FLinearColor::Green));
    FSoftClassPath LegacyStaggerPath(TEXT("/Script/OdysseyLayerStack.OdysseyLayerCellImageStagger"));
    LegacyStaggerPath.FixupCoreRedirects();
    TestTrue(TEXT("Serialized Stagger class path loads the moved class"),
        LegacyStaggerPath.TryLoadClass<UOdysseyLayerCell>() == UOdysseyLayerCellImageStagger::StaticClass());
    const FString ExportDirectory = FPaths::Combine(FPaths::ProjectIntermediateDir(), TEXT("OdysseyAnimationExportTests"));
    const TArray<FString> ExportedPaths = Odyssey::ExportAsImageSequence(Animation.Get(), FInt32Range(5, 7),
        FGuid::NewGuid().ToString(EGuidFormats::Digits), ExportDirectory, EOdysseyExportImageFormat::PNG);
    if (TestEqual(TEXT("Sequence export resolves both referenced animation cells"), ExportedPaths.Num(), 2))
    {
        TArray<uint8> FirstImage;
        TArray<uint8> SecondImage;
        TestTrue(TEXT("Read first exported image"), FFileHelper::LoadFileToArray(FirstImage, *ExportedPaths[0]));
        TestTrue(TEXT("Read second exported image"), FFileHelper::LoadFileToArray(SecondImage, *ExportedPaths[1]));
        TestTrue(TEXT("Exported animation frames have different pixels"), FirstImage != SecondImage);
    }
    for (const FString& ExportedPath : ExportedPaths)
    {
        IFileManager::Get().Delete(*ExportedPath);
    }
    return true;
}
#endif
