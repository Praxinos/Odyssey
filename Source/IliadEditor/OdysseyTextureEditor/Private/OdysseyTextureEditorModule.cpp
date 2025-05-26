// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyTextureEditorModule.h"

#include "AssetToolsModule.h"
#include "ContentBrowserModule.h"
#include "ISettingsModule.h"
#include "OdysseyTextureAssetTypeActions.h"
#include "OdysseyTextureContentBrowserExtensions.h"
#include "OdysseyTextureEditorSettings.h"
#include "OdysseyTextureExportFolderExtension.h"

#define LOCTEXT_NAMESPACE "TextureEditor"

void
FOdysseyTextureEditorModule::StartupModule()
{
    // Register Assets Types Actions once the main loop is initialized
    // see here : https://udn.unrealengine.com/s/question/0D54z00007DVU5KCAX/two-assettypeactions-for-the-same-type-force-priority-
    FCoreDelegates::OnFEngineLoopInitComplete.AddRaw(this, &FOdysseyTextureEditorModule::RegisterAssetTypeActions);

    // Install Content Browser Extionsion Hooks
    if (!IsRunningCommandlet())
    {
        FOdysseyTextureContentBrowserExtensions::InstallHooks();

        FContentBrowserModule& contentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>( TEXT("ContentBrowser") );
        FOdysseyTextureExportFolderExtension::Register( contentBrowserModule );
    }

    RegisterSettings();
}

void
FOdysseyTextureEditorModule::ShutdownModule()
{
    // Unregister Assets Types Actions
    FCoreDelegates::OnFEngineLoopInitComplete.RemoveAll(this);

    // Uninstall Content Browser Extionsion Hooks
    FOdysseyTextureContentBrowserExtensions::RemoveHooks();

    // Unregister Assets Type Actions
    UnregisterAssetTypeActions();

    UnregisterSettings();
}

void
FOdysseyTextureEditorModule::RegisterAssetTypeActions()
{
    IAssetTools& assetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();

    // Create Asset Categories
    EAssetTypeCategories::Type category = assetTools.RegisterAdvancedAssetCategory(FName(TEXT("Odyssey")), LOCTEXT("asset-category.name", "Odyssey"));

    //Create Asset Types Actions
    mOdysseyTypeActions = MakeShareable(new FOdysseyTextureAssetTypeActions(category));

    //Register created Asset Type Actions
    assetTools.RegisterAssetTypeActions(mOdysseyTypeActions.ToSharedRef());
}

void
FOdysseyTextureEditorModule::UnregisterAssetTypeActions()
{
    if (!FModuleManager::Get().IsModuleLoaded("AssetTools"))
        return;

    IAssetTools& assetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
    assetTools.UnregisterAssetTypeActions(mOdysseyTypeActions.ToSharedRef());
}

void
FOdysseyTextureEditorModule::RegisterSettings()
{
    ISettingsModule* settingsModule = FModuleManager::GetModulePtr<ISettingsModule>( "Settings" );

    if( !settingsModule )
        return;

    settingsModule->RegisterSettings( "Editor", "Plugins", "OdysseyTexture2DEditor"
        , LOCTEXT( "settings.name", "Odyssey Texture2D Editor" )
        , LOCTEXT( "settings.tooltip", "Configure the look and feel of the Odyssey Editor." )
        , GetMutableDefault<UOdysseyTextureEditorSettings>() );
}

void
FOdysseyTextureEditorModule::UnregisterSettings()
{
    ISettingsModule* settingsModule = FModuleManager::GetModulePtr<ISettingsModule>( "Settings" );

    if( !settingsModule )
        return;

    settingsModule->UnregisterSettings( "Editor", "Plugins", "OdysseyTexture2DEditor" );
}

IMPLEMENT_MODULE( FOdysseyTextureEditorModule, OdysseyTextureEditor );

#undef LOCTEXT_NAMESPACE
