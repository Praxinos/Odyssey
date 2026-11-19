// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#include "OdysseyAnimationLayerStackUserData.h"

#include "Misc/FeedbackContext.h"
#include "Misc/PackageName.h"
#include "UObject/Package.h"
#include "UObject/SavePackage.h"
#include "UObject/UObjectGlobals.h"

#include "OdysseyAnimation.h"
#include "OdysseyAnimationCellImageVector.h"
#include "OdysseyAnimationLayer.h"
#include "OdysseyAnimationLayerStack.h"
#include "OdysseyLayer.h"

DEFINE_LOG_CATEGORY_STATIC(LogOdysseyAnimationLayerStackUserData, Log, All);

UOdysseyAnimationLayerStackUserData*
UOdysseyAnimationLayerStackUserData::FindOrCreate(const UOdysseyAnimation* iAnimation)
{
    if (!iAnimation)
        return nullptr;

    // Creating the editor data lazily does not change the animation content, so it is allowed from const accessors
    UOdysseyAnimation* animation = const_cast<UOdysseyAnimation*>(iAnimation);

    UOdysseyAnimationLayerStackUserData* userData = animation->GetAssetUserData<UOdysseyAnimationLayerStackUserData>();
    if (!userData)
    {
        userData = NewObject<UOdysseyAnimationLayerStackUserData>(animation, NAME_None, RF_Transactional);
        animation->AddAssetUserData(userData);
    }

    userData->MigrateLegacyData();
    userData->CreateLayerStackIfNeeded();

    return userData;
}

UOdysseyAnimationLayerStack*
UOdysseyAnimationLayerStackUserData::GetLayerStack(const UOdysseyAnimation* iAnimation)
{
    UOdysseyAnimationLayerStackUserData* userData = FindOrCreate(iAnimation);
    return userData ? userData->LayerStack : nullptr;
}

UOdysseyAnimation*
UOdysseyAnimationLayerStackUserData::GetAnimation() const
{
    return Cast<UOdysseyAnimation>(GetOuter());
}

const IOdysseyTextureRenderingAbility*
UOdysseyAnimationLayerStackUserData::GetAnimationRenderingSource() const
{
    return LayerStack;
}

FText
UOdysseyAnimationLayerStackUserData::GetCurrentLayerName() const
{
    UOdysseyLayer* currentLayer = LayerStack ? LayerStack->GetCurrentLayer() : nullptr;
    if (!currentLayer)
        return FText::GetEmpty();

    return currentLayer->GetLayerName();
}

void
UOdysseyAnimationLayerStackUserData::OnRefreshReferencedPalette(UOdysseyPalette* iPalette)
{
    UOdysseyAnimation* animation = GetAnimation();
    if (!animation || !LayerStack)
        return;

    //Save the animation before applying the palette, because all these refresh only affect the "on disk" version of the asset, not the dirty "on memory" one.
    //Therefore, if the palette is freshly added to the (then dirtied) animation, the refresh won't apply to it
    UPackage* package = animation->GetOutermost();
    FSavePackageArgs saveArgs;
    saveArgs.TopLevelFlags = RF_Standalone;
    saveArgs.Error = GWarn;
    saveArgs.SaveFlags = SAVE_NoError;
    FString packageFilename = FPackageName::LongPackageNameToFilename(package->GetName(), FPackageName::GetAssetPackageExtension());
    UPackage::SavePackage(package, animation, *packageFilename, saveArgs);
    FlushAsyncLoading();

    TArray<UOdysseyLayer*> layers = LayerStack->GetLayers();
    for (UOdysseyLayer* layer : layers)
    {
        UOdysseyAnimationLayer* animationLayer = Cast<UOdysseyAnimationLayer>(layer);
        if (!animationLayer)
            continue;

        for (UOdysseyAnimationCell* cell : animationLayer->GetCells())
        {
            UOdysseyAnimationCellImageVector* vectorCell = Cast<UOdysseyAnimationCellImageVector>(cell);
            if (!vectorCell)
                continue;

            vectorCell->OnRefreshReferencedPalette(iPalette);
        }
    }
}

void
UOdysseyAnimationLayerStackUserData::MigrateLegacyData()
{
    UOdysseyAnimation* animation = GetAnimation();
    if (!animation || !animation->HasLegacyEditorData())
        return;

    UObject* legacyLayerStack = nullptr;
    TArray<UOdysseyPaletteSet*> legacyPalettes;
    TArray<FSoftObjectPath> legacyToolCollections;
    animation->TakeLegacyEditorData(legacyLayerStack, legacyPalettes, legacyToolCollections);

    if (!LayerStack && legacyLayerStack)
    {
        LayerStack = Cast<UOdysseyAnimationLayerStack>(legacyLayerStack);
        if (!LayerStack)
            UE_LOG(LogOdysseyAnimationLayerStackUserData, Warning, TEXT("%s: legacy layer stack %s is not an animation layer stack and is dropped"), *animation->GetPathName(), *legacyLayerStack->GetName());
    }

    for (UOdysseyPaletteSet* paletteSet : legacyPalettes)
        Palettes.AddUnique(paletteSet);

    for (const FSoftObjectPath& toolCollection : legacyToolCollections)
        ToolCollections.AddUnique(toolCollection);

    animation->RenderingCompositionChanged();
}

void
UOdysseyAnimationLayerStackUserData::CreateLayerStackIfNeeded()
{
    if (LayerStack)
        return;

    FName layerStackName = MakeUniqueObjectName(this, UOdysseyAnimationLayerStack::StaticClass(), TEXT("LayerStack"));
    LayerStack = NewObject<UOdysseyAnimationLayerStack>(this, layerStackName, RF_Public | RF_Transactional);

    UOdysseyAnimation* animation = GetAnimation();
    if (animation)
        animation->RenderingCompositionChanged();
}
