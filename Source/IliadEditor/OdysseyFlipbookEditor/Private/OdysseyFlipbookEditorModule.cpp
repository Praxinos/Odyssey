// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyFlipbookEditorModule.h"

#include "AssetToolsModule.h"
#include "ISettingsModule.h"
#include "PaperFlipbook.h"

#include "OdysseyFlipbookAssetTypeActions.h"
#include "OdysseyFlipbookContentBrowserExtensions.h"
#include "OdysseyFlipbookEditorSettings.h"

#define LOCTEXT_NAMESPACE "FlipbookEditor"

/*-----------------------------------------------------------------------------
   FOdysseyFlipbookEditorModule
-----------------------------------------------------------------------------*/

void
FOdysseyFlipbookEditorModule::StartupModule()
{
    // Register Assets Types Actions once the main loop is initialized
    // see here : https://udn.unrealengine.com/s/question/0D54z00007DVU5KCAX/two-assettypeactions-for-the-same-type-force-priority-
    FCoreDelegates::OnFEngineLoopInitComplete.AddRaw(this, &FOdysseyFlipbookEditorModule::RegisterAssetTypeActions);

    // Install Content Browser Extionsion Hooks
    if (!IsRunningCommandlet())
    {
        FOdysseyFlipbookContentBrowserExtensions::InstallHooks();
    }

    RegisterSettings();
}

void
FOdysseyFlipbookEditorModule::ShutdownModule()
{
    // Unregister Assets Types Actions
    FCoreDelegates::OnFEngineLoopInitComplete.RemoveAll(this);

    // Uninstall Content Browser Extionsion Hooks
    FOdysseyFlipbookContentBrowserExtensions::RemoveHooks();

    // Unregister Assets Type Actions
    UnregisterAssetTypeActions();

    UnregisterSettings();

    mOdysseyTypeActions = nullptr;
    mUETypeActions = nullptr;
}

void
FOdysseyFlipbookEditorModule::RegisterAssetTypeActions()
{
    IAssetTools& assetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
    // Create Asset Categories
    EAssetTypeCategories::Type category = assetTools.RegisterAdvancedAssetCategory(FName(TEXT("Odyssey")), LOCTEXT("asset-category.name", "Odyssey"));

    if( !mUETypeActions )
        mUETypeActions = assetTools.GetAssetTypeActionsForClass(UPaperFlipbook::StaticClass() ).Pin();
    if( !mOdysseyTypeActions )
        mOdysseyTypeActions = MakeShareable(new FOdysseyFlipbookAssetTypeActions(category));

    if( UOdysseyFlipbookEditorSettings::Get()->OdysseyDefaultEditorEnabled )
    {
        // Remove old AssetTypeAction from UE
        assetTools.UnregisterAssetTypeActions(mUETypeActions.ToSharedRef());

        //Register created Asset Type Actions
        assetTools.RegisterAssetTypeActions(mOdysseyTypeActions.ToSharedRef());

    }
    else
    {
        // Remove old AssetTypeAction
        assetTools.UnregisterAssetTypeActions(mOdysseyTypeActions.ToSharedRef());

        //Register created Asset Type Actions from UE
        assetTools.RegisterAssetTypeActions(mUETypeActions.ToSharedRef());
    }
}

void
FOdysseyFlipbookEditorModule::UnregisterAssetTypeActions()
{
    if (!FModuleManager::Get().IsModuleLoaded("AssetTools"))
        return;

    IAssetTools& assetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
    assetTools.UnregisterAssetTypeActions(mOdysseyTypeActions.ToSharedRef());
    assetTools.UnregisterAssetTypeActions(mUETypeActions.ToSharedRef());
}

void
FOdysseyFlipbookEditorModule::RegisterSettings()
{
    ISettingsModule* settingsModule = FModuleManager::GetModulePtr<ISettingsModule>( "Settings" );

    if( !settingsModule )
        return;

    settingsModule->RegisterSettings( "Editor", "Plugins", "OdysseyFlipbookEditor"
        , LOCTEXT( "settings.name", "Odyssey Flipbook Editor" )
        , LOCTEXT( "settings.tooltip", "Configure the look and feel of the Odyssey Editor." )
        , GetMutableDefault<UOdysseyFlipbookEditorSettings>() );
}

void
FOdysseyFlipbookEditorModule::UnregisterSettings()
{
    ISettingsModule* settingsModule = FModuleManager::GetModulePtr<ISettingsModule>( "Settings" );

    if( !settingsModule )
        return;

    settingsModule->UnregisterSettings( "Editor", "Plugins", "OdysseyFlipbookEditor" );
}

IMPLEMENT_MODULE( FOdysseyFlipbookEditorModule, OdysseyFlipbookEditor );

#undef LOCTEXT_NAMESPACE
