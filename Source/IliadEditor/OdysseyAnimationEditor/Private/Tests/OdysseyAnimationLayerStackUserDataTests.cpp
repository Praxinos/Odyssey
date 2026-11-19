// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#if WITH_DEV_AUTOMATION_TESTS
#include "Engine/AssetUserData.h"
#include "Misc/AutomationTest.h"
#include "UObject/Package.h"
#include "UObject/SoftObjectPath.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/UnrealType.h"

#include "OdysseyAnimation.h"
#include "OdysseyAnimationEditorData.h"
#include "OdysseyAnimationLayerImageRaster.h"
#include "OdysseyAnimationLayerStack.h"
#include "OdysseyAnimationLayerStackUserData.h"
#include "OdysseyLayer.h"
#include "OdysseyPalette.h"
#include "OdysseyRenderingType.h"

namespace Odyssey::AnimationLayerStackUserDataTests
{
    constexpr EAutomationTestFlags Flags = EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter;

    /** Writes a value in a legacy property of the animation, as loading an asset saved before the editor data existed would (UHT strips the _DEPRECATED suffix from property names) */
    template<typename T>
    bool SetLegacyProperty(FAutomationTestBase& Test, UOdysseyAnimation* Animation, const TCHAR* PropertyName, const T& Value)
    {
        FProperty* Property = FindFProperty<FProperty>(UOdysseyAnimation::StaticClass(), PropertyName);
        if (!Test.TestNotNull(FString::Printf(TEXT("Legacy property %s exists"), PropertyName), Property))
            return false;

        *Property->ContainerPtrToValuePtr<T>(Animation) = Value;
        return true;
    }

    /** Returns the composition the animation renders with when its editor data renders through iLayerStack */
    TArray<FGuid> ExpectedComposition(const UOdysseyAnimation* Animation, const UOdysseyAnimationLayerStack* LayerStack)
    {
        TArray<FGuid> Composition = { Animation->GetRenderingId() };
        if (LayerStack)
            Composition.Append(LayerStack->GetRenderingComposition(EOdysseyRenderingType::Render, 0));
        return Composition;
    }
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOdysseyAnimationLegacyEditorDataMigrationTest, "Odyssey.Animation.EditorData.LegacyMigration",
    Odyssey::AnimationLayerStackUserDataTests::Flags)

/** Loads an animation holding its editor content the legacy way and checks the editor module moves it to the editor data. */
bool FOdysseyAnimationLegacyEditorDataMigrationTest::RunTest(const FString& Parameters)
{
    using namespace Odyssey::AnimationLayerStackUserDataTests;

    TStrongObjectPtr<UOdysseyAnimation> Animation(NewObject<UOdysseyAnimation>(GetTransientPackage()));
    Animation->Init(16, 16, EOdysseyAnimationFormat::BGRA8, 24.f);

    UOdysseyAnimationLayerStack* LegacyLayerStack = NewObject<UOdysseyAnimationLayerStack>(Animation.Get(), TEXT("LayerStack"), RF_Public | RF_Transactional);
    UOdysseyLayer* LegacyLayer = LegacyLayerStack->AddLayer(UOdysseyAnimationLayerImageRaster::StaticClass());
    UOdysseyPaletteSet* LegacyPaletteSet = NewObject<UOdysseyPaletteSet>(Animation.Get());
    const FSoftObjectPath LegacyToolCollection(TEXT("/Odyssey/Tools/LegacyToolCollection.LegacyToolCollection"));

    if (!SetLegacyProperty<TObjectPtr<UObject>>(*this, Animation.Get(), TEXT("mLayerStack"), LegacyLayerStack)
        || !SetLegacyProperty<TArray<UOdysseyPaletteSet*>>(*this, Animation.Get(), TEXT("Palettes"), { LegacyPaletteSet })
        || !SetLegacyProperty<TArray<FSoftObjectPath>>(*this, Animation.Get(), TEXT("ToolCollections"), { LegacyToolCollection }))
    {
        return false;
    }

    TestTrue(TEXT("Animation holds legacy editor content"), Animation->HasLegacyEditorData());
    TestNull(TEXT("Legacy animation has no editor data yet"), Animation->GetEditorData());
    TestEqual(TEXT("Without editor data, the animation only renders its own id"),
        Animation->GetRenderingComposition(EOdysseyRenderingType::Render, 0), ExpectedComposition(Animation.Get(), nullptr));

    // The runtime class broadcasts the migration request on load, the editor module answers it
    Animation->PostLoad();

    UOdysseyAnimationLayerStackUserData* EditorData = Animation->GetAssetUserData<UOdysseyAnimationLayerStackUserData>();
    if (!TestNotNull(TEXT("Loading creates the editor data"), EditorData))
        return false;

    TestFalse(TEXT("Legacy content is moved out of the animation"), Animation->HasLegacyEditorData());
    TestTrue(TEXT("Editor data is found through the runtime interface"), Animation->GetEditorData() == EditorData);
    TestTrue(TEXT("Legacy layer stack is kept, with its layers"), EditorData->LayerStack.Get() == LegacyLayerStack && LegacyLayerStack->GetLayers().Contains(LegacyLayer));
    TestTrue(TEXT("Legacy palette sets are moved"), EditorData->Palettes.Num() == 1 && EditorData->Palettes[0] == LegacyPaletteSet);
    TestTrue(TEXT("Legacy tool collections are moved"), EditorData->ToolCollections.Num() == 1 && EditorData->ToolCollections[0] == LegacyToolCollection);
    TestTrue(TEXT("Migrated layer stack still resolves its animation"), LegacyLayerStack->GetAnimation() == Animation.Get());
    TestEqual(TEXT("Animation renders through the migrated layer stack"),
        Animation->GetRenderingComposition(EOdysseyRenderingType::Render, 0), ExpectedComposition(Animation.Get(), LegacyLayerStack));

    // A second load or access must neither duplicate the editor data nor replace the layer stack
    Animation->PostLoad();
    TestTrue(TEXT("Accessing the editor data again returns the same one"), UOdysseyAnimationLayerStackUserData::FindOrCreate(Animation.Get()) == EditorData);
    TestEqual(TEXT("Animation has a single asset user data"), Animation->GetAssetUserDataArray()->Num(), 1);
    TestTrue(TEXT("Layer stack is unchanged"), UOdysseyAnimationLayerStackUserData::GetLayerStack(Animation.Get()) == LegacyLayerStack);

    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOdysseyAnimationEditorDataRenderingSourceTest, "Odyssey.Animation.EditorData.RenderingSource",
    Odyssey::AnimationLayerStackUserDataTests::Flags)

/** Checks the runtime animation renders, bounds and names layers through its editor data only while it has one. */
bool FOdysseyAnimationEditorDataRenderingSourceTest::RunTest(const FString& Parameters)
{
    using namespace Odyssey::AnimationLayerStackUserDataTests;

    TestNull(TEXT("No editor data without animation"), UOdysseyAnimationLayerStackUserData::FindOrCreate(nullptr));
    TestNull(TEXT("No layer stack without animation"), UOdysseyAnimationLayerStackUserData::GetLayerStack(nullptr));

    TStrongObjectPtr<UOdysseyAnimation> Animation(NewObject<UOdysseyAnimation>(GetTransientPackage()));
    Animation->Init(16, 16, EOdysseyAnimationFormat::BGRA8, 24.f);
    TestNull(TEXT("A new animation has no editor data until the editor creates it"), Animation->GetEditorData());
    TestFalse(TEXT("A new animation has no legacy content"), Animation->HasLegacyEditorData());

    UOdysseyAnimationLayerStack* LayerStack = UOdysseyAnimationLayerStackUserData::GetLayerStack(Animation.Get());
    if (!TestNotNull(TEXT("The editor creates an empty layer stack"), LayerStack))
        return false;

    TestTrue(TEXT("Layer stack is owned by the editor data"), LayerStack->GetOuter() == Animation->GetEditorData());
    TestTrue(TEXT("Layer stack resolves its animation through the editor data"), LayerStack->GetAnimation() == Animation.Get());

    UOdysseyLayer* Layer = LayerStack->AddLayer(UOdysseyAnimationLayerImageRaster::StaticClass());
    LayerStack->SetCurrentLayer(Layer);

    TestEqual(TEXT("Animation renders through its layer stack"),
        Animation->GetRenderingComposition(EOdysseyRenderingType::Render, 0), ExpectedComposition(Animation.Get(), LayerStack));
    TestTrue(TEXT("Automatic bounds follow the layer stack"), Animation->GetFrameRange() == FInt32Range::Inclusive(
        LayerStack->GetFrameRange().GetLowerBoundValue(), LayerStack->GetFrameRange().GetUpperBoundValue()));

    const IOdysseyAnimationEditorData* EditorData = Cast<IOdysseyAnimationEditorData>(Animation->GetEditorData());
    if (!TestNotNull(TEXT("Editor data implements the runtime interface"), EditorData))
        return false;

    TestEqual(TEXT("Current layer name is reachable from the runtime interface"), EditorData->GetCurrentLayerName().ToString(), Layer->GetLayerName().ToString());

    Animation->RemoveUserDataOfClass(UOdysseyAnimationLayerStackUserData::StaticClass());
    TestNull(TEXT("Removing the editor data detaches it"), Animation->GetEditorData());
    TestEqual(TEXT("Without editor data, the animation only renders its own id"),
        Animation->GetRenderingComposition(EOdysseyRenderingType::Render, 0), ExpectedComposition(Animation.Get(), nullptr));

    return true;
}

#endif
