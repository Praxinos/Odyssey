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
#include "OdysseyAnimationLayerImageRaster.h"
#include "OdysseyAnimationLayerStack.h"
#include "OdysseyAnimationLayerStackUserData.h"
#include "OdysseyAnimationPalettesUserData.h"
#include "OdysseyAnimationRenderingOverride.h"
#include "OdysseyAnimationToolCollectionsUserData.h"
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

    /** Returns the composition the animation renders with when its rendering is overridden by iLayerStack */
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

/** Loads an animation holding its editor content the legacy way and checks the editor module moves it to the three user data. */
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
    TestNull(TEXT("Legacy animation has no rendering override yet"), Animation->GetRenderingOverride());
    TestEqual(TEXT("Without rendering override, the animation only renders its own id"),
        Animation->GetRenderingComposition(EOdysseyRenderingType::Render, 0), ExpectedComposition(Animation.Get(), nullptr));

    // The runtime class broadcasts the migration request on load, the editor module answers it
    Animation->PostLoad();

    UOdysseyAnimationLayerStackUserData* LayerStackData = Animation->GetAssetUserData<UOdysseyAnimationLayerStackUserData>();
    UOdysseyAnimationPalettesUserData* PalettesData = Animation->GetAssetUserData<UOdysseyAnimationPalettesUserData>();
    UOdysseyAnimationToolCollectionsUserData* ToolCollectionsData = Animation->GetAssetUserData<UOdysseyAnimationToolCollectionsUserData>();
    if (!TestNotNull(TEXT("Loading creates the layer stack user data"), LayerStackData)
        || !TestNotNull(TEXT("Loading creates the palettes user data"), PalettesData)
        || !TestNotNull(TEXT("Loading creates the tool collections user data"), ToolCollectionsData))
    {
        return false;
    }

    TestFalse(TEXT("Legacy content is moved out of the animation"), Animation->HasLegacyEditorData());
    TestTrue(TEXT("Layer stack user data overrides the animation rendering"), Animation->GetRenderingOverride() == LegacyLayerStack);
    TestTrue(TEXT("Legacy layer stack is kept, with its layers"), LayerStackData->LayerStack.Get() == LegacyLayerStack && LegacyLayerStack->GetLayers().Contains(LegacyLayer));
    TestTrue(TEXT("Legacy palette sets are moved"), PalettesData->Palettes.Num() == 1 && PalettesData->Palettes[0] == LegacyPaletteSet);
    TestTrue(TEXT("Legacy tool collections are moved"), ToolCollectionsData->ToolCollections.Num() == 1 && ToolCollectionsData->ToolCollections[0] == LegacyToolCollection);
    TestTrue(TEXT("Migrated layer stack still resolves its animation"), LegacyLayerStack->GetAnimation() == Animation.Get());
    TestEqual(TEXT("Animation renders through the migrated layer stack"),
        Animation->GetRenderingComposition(EOdysseyRenderingType::Render, 0), ExpectedComposition(Animation.Get(), LegacyLayerStack));

    // A second load or access must neither duplicate the user data nor replace their content
    Animation->PostLoad();
    TestTrue(TEXT("Accessing the layer stack user data again returns the same one"), UOdysseyAnimationLayerStackUserData::FindOrCreate(Animation.Get()) == LayerStackData);
    TestTrue(TEXT("Accessing the palettes user data again returns the same one"), UOdysseyAnimationPalettesUserData::FindOrCreate(Animation.Get()) == PalettesData);
    TestTrue(TEXT("Accessing the tool collections user data again returns the same one"), UOdysseyAnimationToolCollectionsUserData::FindOrCreate(Animation.Get()) == ToolCollectionsData);
    TestEqual(TEXT("Animation has one asset user data per content"), Animation->GetAssetUserDataArray()->Num(), 3);
    TestTrue(TEXT("Layer stack is unchanged"), UOdysseyAnimationLayerStackUserData::GetLayerStack(Animation.Get()) == LegacyLayerStack);

    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOdysseyAnimationEditorDataRenderingSourceTest, "Odyssey.Animation.EditorData.RenderingSource",
    Odyssey::AnimationLayerStackUserDataTests::Flags)

/** Checks the animation renders and bounds through the user data overriding its rendering, and only while it has one. */
bool FOdysseyAnimationEditorDataRenderingSourceTest::RunTest(const FString& Parameters)
{
    using namespace Odyssey::AnimationLayerStackUserDataTests;

    TestNull(TEXT("No editor data without animation"), UOdysseyAnimationLayerStackUserData::FindOrCreate(nullptr));

    const FProperty* AssetUserDataProperty = FindFProperty<FProperty>(UOdysseyAnimation::StaticClass(), TEXT("AssetUserData"));
    if (!TestNotNull(TEXT("Animation asset user data property"), AssetUserDataProperty))
        return false;

    TestFalse(TEXT("Editor data cannot be removed from the details panel"), AssetUserDataProperty->HasAnyPropertyFlags(CPF_Edit));
    TestNull(TEXT("No layer stack without animation"), UOdysseyAnimationLayerStackUserData::GetLayerStack(nullptr));

    TStrongObjectPtr<UOdysseyAnimation> Animation(NewObject<UOdysseyAnimation>(GetTransientPackage()));
    Animation->Init(16, 16, EOdysseyAnimationFormat::BGRA8, 24.f);
    TestNull(TEXT("A new animation has no rendering override until the editor creates its layer stack"), Animation->GetRenderingOverride());

    // User data that do not override the rendering are ignored
    UOdysseyAnimationPalettesUserData::FindOrCreate(Animation.Get());
    UOdysseyAnimationToolCollectionsUserData::FindOrCreate(Animation.Get());
    TestNull(TEXT("Palettes and tool collections do not override the rendering"), Animation->GetRenderingOverride());
    TestFalse(TEXT("A new animation has no legacy content"), Animation->HasLegacyEditorData());

    UOdysseyAnimationLayerStack* LayerStack = UOdysseyAnimationLayerStackUserData::GetLayerStack(Animation.Get());
    if (!TestNotNull(TEXT("The editor creates an empty layer stack"), LayerStack))
        return false;

    TestTrue(TEXT("Layer stack is owned by its user data"), LayerStack->GetOuter() == Animation->GetAssetUserData<UOdysseyAnimationLayerStackUserData>());
    TestTrue(TEXT("Layer stack resolves its animation through its user data"), LayerStack->GetAnimation() == Animation.Get());
    TestTrue(TEXT("Layer stack overrides the animation rendering"), Animation->GetRenderingOverride() == LayerStack);

    UOdysseyLayer* Layer = LayerStack->AddLayer(UOdysseyAnimationLayerImageRaster::StaticClass());
    LayerStack->SetCurrentLayer(Layer);

    TestEqual(TEXT("Animation renders through its layer stack"),
        Animation->GetRenderingComposition(EOdysseyRenderingType::Render, 0), ExpectedComposition(Animation.Get(), LayerStack));
    TestTrue(TEXT("Automatic bounds follow the layer stack"), Animation->GetFrameRange() == FInt32Range::Inclusive(
        LayerStack->GetFrameRange().GetLowerBoundValue(), LayerStack->GetFrameRange().GetUpperBoundValue()));


    Animation->RemoveUserDataOfClass(UOdysseyAnimationLayerStackUserData::StaticClass());
    TestNull(TEXT("Removing the layer stack user data removes the rendering override"), Animation->GetRenderingOverride());
    TestEqual(TEXT("Without rendering override, the animation only renders its own id"),
        Animation->GetRenderingComposition(EOdysseyRenderingType::Render, 0), ExpectedComposition(Animation.Get(), nullptr));

    return true;
}

#endif
