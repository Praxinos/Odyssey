// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#if WITH_DEV_AUTOMATION_TESTS
#include "Engine/Engine.h"
#include "Misc/AutomationTest.h"
#include "NamingTokensEngineSubsystem.h"
#include "UObject/Package.h"
#include "UObject/StrongObjectPtr.h"

#include "OdysseyAnimation.h"
#include "OdysseyAnimationLayerImageRaster.h"
#include "OdysseyAnimationLayerStack.h"
#include "OdysseyAnimationLayerStackUserData.h"
#include "OdysseyAnimationNamingTokens.h"
#include "OdysseyLayer.h"
#include "OdysseyAnimationEditorData.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOdysseyAnimationCurrentLayerNamingTokenTest, "Odyssey.Animation.NamingTokens.CurrentLayerName",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

/** Evaluates the current layer name token through the naming tokens subsystem, with and without animation editor data. */
bool FOdysseyAnimationCurrentLayerNamingTokenTest::RunTest(const FString& Parameters)
{
    UNamingTokensEngineSubsystem* NamingTokensSubsystem = GEngine ? GEngine->GetEngineSubsystem<UNamingTokensEngineSubsystem>() : nullptr;
    if (!TestNotNull(TEXT("Naming tokens subsystem"), NamingTokensSubsystem))
        return false;

    FNamingTokenFilterArgs Filter;
    Filter.AdditionalNamespacesToInclude.Add(UOdysseyAnimationNamingTokens::TokenNamespace);
    const FString Template = TEXT("{odysseyAnimation:currentLayerName}");

    TStrongObjectPtr<UOdysseyAnimation> Animation(NewObject<UOdysseyAnimation>(GetTransientPackage()));
    Animation->Init(16, 16, EOdysseyAnimationFormat::BGRA8, 24.f);
    TStrongObjectPtr<UOdysseyAnimationNamingTokensContext> Context(NewObject<UOdysseyAnimationNamingTokensContext>());
    Context->Animation = Animation.Get();

    FNamingTokenResultData Result = NamingTokensSubsystem->EvaluateTokenString(Template, Filter, { Context.Get() });
    TestTrue(TEXT("Animation without editor data has no current layer name"), Result.EvaluatedText.IsEmpty());
    TestNull(TEXT("Evaluating the token does not create the layer stack user data"), Animation->GetAssetUserData<UOdysseyAnimationLayerStackUserData>());

    UOdysseyAnimationLayerStack* LayerStack = OdysseyAnimationEditor::GetLayerStack(Animation.Get());
    if (!TestNotNull(TEXT("Animation layer stack"), LayerStack))
        return false;

    UOdysseyLayer* Layer = LayerStack->AddLayer(UOdysseyAnimationLayerImageRaster::StaticClass());
    Layer->SetLayerName(FText::FromString(TEXT("NamingTokenLayer")));
    LayerStack->SetCurrentLayer(Layer);

    Result = NamingTokensSubsystem->EvaluateTokenString(Template, Filter, { Context.Get() });
    TestEqual(TEXT("Token returns the current layer name of the editor data"), Result.EvaluatedText.ToString(), FString(TEXT("NamingTokenLayer")));

    return true;
}

#endif
