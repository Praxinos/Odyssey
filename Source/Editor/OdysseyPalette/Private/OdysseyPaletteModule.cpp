// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

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
    EAssetTypeCategories::Type category = assetTools.RegisterAdvancedAssetCategory(FName(TEXT("ILIAD")), LOCTEXT("asset-category.name", "ILIAD"));

    //Create Asset Types Actions
    mIliadTypeActions = MakeShareable(new FOdysseyPaletteAssetTypeActions(category));

    //Register created Asset Type Actions
    assetTools.RegisterAssetTypeActions(mIliadTypeActions.ToSharedRef());
}

void
FOdysseyPaletteModule::UnregisterAssetTypeActions()
{
    if (!FModuleManager::Get().IsModuleLoaded("AssetTools"))
        return;

    IAssetTools& assetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
    assetTools.UnregisterAssetTypeActions(mIliadTypeActions.ToSharedRef());
}

IMPLEMENT_MODULE(FOdysseyPaletteModule, OdysseyTexture);

#undef LOCTEXT_NAMESPACE
