// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyTextureEditorModule.h"

#include "AssetToolsModule.h"
#include "CoreMinimal.h"
#include "ISettingsModule.h"
#include "LevelEditor.h"
#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"
#include "Settings/ContentBrowserSettings.h"
#include "Toolkits/AssetEditorToolkit.h"

#include "OdysseyTextureEditor.h"
#include "OdysseyTextureEditorSettings.h"
#include "OdysseyTextureEditorToolkit.h"
#include "OdysseyTexture_AssetTypeActions.h"

#define LOCTEXT_NAMESPACE "OdysseyTextureEditorModule"
const FName OdysseyTextureEditorAppIdentifier = FName( TEXT( "OdysseyTextureEditorApp" ) );

/*-----------------------------------------------------------------------------
   FOdysseyTextureEditorModule
-----------------------------------------------------------------------------*/

void
FOdysseyTextureEditorModule::StartupModule()
{
	// Register menu extensions
	mMenuExtensibilityManager = MakeShareable(new FExtensibilityManager);

	// Register Assets Types Actions
	RegisterAssetTypeActions();

	// Register Settings
    RegisterSettings();

	// Install Content Browser Extionsion Hooks
	if (!IsRunningCommandlet())
	{
		FOdysseyTextureContentBrowserExtensions::InstallHooks();
	}
}

void
FOdysseyTextureEditorModule::ShutdownModule()
{
	// Uninstall Content Browser Extionsion Hooks
	FOdysseyTextureContentBrowserExtensions::RemoveHooks();

	// Unregister Settings
    UnregisterSettings();

	// Unregister Assets Type Actions
	UnregisterAssetTypeActions();
    
	// Unregister menu extensions
	mMenuExtensibilityManager.Reset();
}

void
FOdysseyTextureEditorModule::RegisterAssetTypeActions()
{
	IAssetTools& assetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();

	// Create Asset Categories
	EAssetTypeCategories::Type category = assetTools.RegisterAdvancedAssetCategory(FName(TEXT("ILIAD")), LOCTEXT("IliadPainterAssetCategory", "ILIAD"));

	//Create Asset Types Actions
	mTypeActions.Add(MakeShareable(new FOdysseyTextureAssetTypeActions(category)));

	//Register created Asset Type Actions
	for (int32 index = 0; index < mTypeActions.Num(); ++index)
	{
		assetTools.RegisterAssetTypeActions(mTypeActions[index].ToSharedRef());
	}
}

void
FOdysseyTextureEditorModule::UnregisterAssetTypeActions()
{
	if (!FModuleManager::Get().IsModuleLoaded("AssetTools"))
		return;
	
	IAssetTools& assetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
	for (int32 index = 0; index < mTypeActions.Num(); ++index)
	{
		assetTools.UnregisterAssetTypeActions(mTypeActions[index].ToSharedRef());
	}
}

void
FOdysseyTextureEditorModule::RegisterSettings()
{
    ISettingsModule* settingsModule = FModuleManager::GetModulePtr<ISettingsModule>( "Settings" );
    if( !settingsModule )
		return;

	settingsModule->RegisterSettings( "Editor", "ContentEditors", "ILIADTextureEditor"
										, LOCTEXT( "OdysseyTextureEditorSettingsName", "ILIAD Texture Editor" )
										, LOCTEXT( "OdysseyTextureEditorSettingsDescription", "Configure the look and feel of the ILIAD Editor." )
										, GetMutableDefault<UOdysseyTextureEditorSettings>() );
}

void
FOdysseyTextureEditorModule::UnregisterSettings()
{
    ISettingsModule* settingsModule = FModuleManager::GetModulePtr<ISettingsModule>( "Settings" );

    if( !settingsModule )
		return;
    
	settingsModule->UnregisterSettings( "Editor", "ContentEditors", "OdysseyTextureEditor" );
}

TSharedRef<FOdysseyTextureEditorToolkit>
FOdysseyTextureEditorModule::CreateOdysseyTextureEditor( UTexture2D* iTexture )
{
    TSharedPtr<FOdysseyTextureEditorToolkit> toolkit = MakeShareable( new FOdysseyTextureEditorToolkit() );
	TSharedPtr<FOdysseyTextureEditor> editor = MakeShareable( new FOdysseyTextureEditor(iTexture, toolkit) );
	editor->Init();
    toolkit->Init( editor, iTexture );
    return toolkit.ToSharedRef();
}

TSharedPtr<FExtensibilityManager>
FOdysseyTextureEditorModule::GetMenuExtensibilityManager()
{
	return mMenuExtensibilityManager;
}

IMPLEMENT_MODULE( FOdysseyTextureEditorModule, OdysseyTextureEditor );

#undef LOCTEXT_NAMESPACE
