// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyPaletteModule.h"
#include "IAssetTools.h"
#include "OdysseyPaletteAssetTypeActions.h"

#define LOCTEXT_NAMESPACE "Palette"

void FOdysseyPaletteModule::StartupModule()
{
    FCoreDelegates::OnFEngineLoopInitComplete.AddRaw(this, &FOdysseyPaletteModule::RegisterAssetTypeActions);
}

void FOdysseyPaletteModule::ShutdownModule()
{
}

void
FOdysseyPaletteModule::RegisterAssetTypeActions()
{
    IAssetTools& assetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();

    // Create Asset Categories
    EAssetTypeCategories::Type category = assetTools.RegisterAdvancedAssetCategory(FName(TEXT("Odyssey")), LOCTEXT("asset-category.name", "Odyssey"));

    //Create Asset Types Actions
    mOdysseyTypeActions = MakeShareable(new FOdysseyPaletteAssetTypeActions(category));

    //Register created Asset Type Actions
    assetTools.RegisterAssetTypeActions(mOdysseyTypeActions.ToSharedRef());
}

void
FOdysseyPaletteModule::UnregisterAssetTypeActions()
{
    if (!FModuleManager::Get().IsModuleLoaded("AssetTools"))
        return;

    IAssetTools& assetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
    assetTools.UnregisterAssetTypeActions(mOdysseyTypeActions.ToSharedRef());
}

IMPLEMENT_MODULE(FOdysseyPaletteModule, OdysseyPalette);

#undef LOCTEXT_NAMESPACE
