// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyFlipbookEditorModule.h"

#include "AssetToolsModule.h"
#include "CoreMinimal.h"
#include "ISettingsModule.h"
#include "LevelEditor.h"
#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"
#include "Settings/ContentBrowserSettings.h"
#include "Toolkits/AssetEditorToolkit.h"

#include "OdysseyFlipbookEditor.h"
#include "OdysseyFlipbookEditorSettings.h"
#include "OdysseyFlipbookEditorToolkit.h"
#include "OdysseyFlipbook_AssetTypeActions.h"

#define LOCTEXT_NAMESPACE "OdysseyFlipbookEditorModule"

/*-----------------------------------------------------------------------------
   FOdysseyFlipbookEditorModule
-----------------------------------------------------------------------------*/

void
FOdysseyFlipbookEditorModule::StartupModule()
{
	// Register menu extensions
	mMenuExtensibilityManager = MakeShareable(new FExtensibilityManager);

	// Register Assets Types Actions
	RegisterAssetTypeActions();

	// Register Commands
	RegisterCommands();

	// Register Settings
    RegisterSettings();

	// Install Content Browser Extionsion Hooks
	if (!IsRunningCommandlet())
	{
		FOdysseyFlipbookContentBrowserExtensions::InstallHooks();
	}
}

void
FOdysseyFlipbookEditorModule::ShutdownModule()
{
	// Uninstall Content Browser Extionsion Hooks
	FOdysseyFlipbookContentBrowserExtensions::RemoveHooks();

	// Unregister Settings
    UnregisterSettings();

	// Unregister Commands
	UnregisterCommands();

	// Unregister Assets Type Actions
	UnregisterAssetTypeActions();
    
	// Unregister menu extensions
	mMenuExtensibilityManager.Reset();
}

void
FOdysseyFlipbookEditorModule::RegisterAssetTypeActions()
{
	IAssetTools& assetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();

	// Create Asset Categories
	EAssetTypeCategories::Type category = assetTools.RegisterAdvancedAssetCategory(FName(TEXT("ILIAD")), LOCTEXT("IliadPainterAssetCategory", "ILIAD"));

	//Create Asset Types Actions
	mTypeActions.Add(MakeShareable(new FOdysseyFlipbookAssetTypeActions(category)));

	//Register created Asset Type Actions
	for (int32 index = 0; index < mTypeActions.Num(); ++index)
	{
		assetTools.RegisterAssetTypeActions(mTypeActions[index].ToSharedRef());
	}
}

void
FOdysseyFlipbookEditorModule::UnregisterAssetTypeActions()
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
FOdysseyFlipbookEditorModule::RegisterSettings()
{
    ISettingsModule* settingsModule = FModuleManager::GetModulePtr<ISettingsModule>( "Settings" );
    if( !settingsModule )
		return;

	settingsModule->RegisterSettings( "Editor", "ContentEditors", "ILIADFlipbookEditor"
										, LOCTEXT( "OdysseyFlipbookEditorSettingsName", "ILIAD Flipbook Editor" )
										, LOCTEXT( "OdysseyFlipbookEditorSettingsDescription", "Configure the look and feel of the ILIAD Editor." )
										, GetMutableDefault<UOdysseyFlipbookEditorSettings>() );
}

void
FOdysseyFlipbookEditorModule::UnregisterSettings()
{
    ISettingsModule* settingsModule = FModuleManager::GetModulePtr<ISettingsModule>( "Settings" );

    if( !settingsModule )
		return;
    
	settingsModule->UnregisterSettings( "Editor", "ContentEditors", "OdysseyFlipbookEditor" );
}

TSharedRef<FOdysseyFlipbookEditorToolkit>
FOdysseyFlipbookEditorModule::CreateOdysseyFlipbookEditor( UPaperFlipbook* iFlipbook )
{
	TSharedPtr<FOdysseyFlipbookEditor> editor = MakeShareable(new FOdysseyFlipbookEditor(iFlipbook));
	TSharedRef<FOdysseyFlipbookEditorToolkit> toolkit = MakeShareable(new FOdysseyFlipbookEditorToolkit(editor));
	editor->Initialize(iFlipbook);
	toolkit->Initialize();
    return toolkit;
}

TSharedPtr<FExtensibilityManager>
FOdysseyFlipbookEditorModule::GetMenuExtensibilityManager()
{
	return mMenuExtensibilityManager;
}

void
FOdysseyFlipbookEditorModule::RegisterCommands()
{
	FOdysseyFlipbookEditorCommands::Register();
}

void
FOdysseyFlipbookEditorModule::UnregisterCommands()
{
	FOdysseyFlipbookEditorCommands::Unregister();
}

IMPLEMENT_MODULE( FOdysseyFlipbookEditorModule, OdysseyFlipbookEditor );

#undef LOCTEXT_NAMESPACE
