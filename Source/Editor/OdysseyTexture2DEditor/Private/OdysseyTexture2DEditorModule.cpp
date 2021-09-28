// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyTexture2DEditorModule.h"

#include "AssetToolsModule.h"
#include "CoreMinimal.h"
#include "ISettingsModule.h"
#include "LevelEditor.h"
#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"
#include "Settings/ContentBrowserSettings.h"
#include "Toolkits/AssetEditorToolkit.h"

#include "OdysseyTexture2DEditor.h"
#include "OdysseyTexture2DEditorSettings.h"
#include "OdysseyTexture2DEditorToolkit.h"
#include "OdysseyTexture2DAssetTypeActions.h"

#define LOCTEXT_NAMESPACE "OdysseyTexture2DEditorModule"

/*-----------------------------------------------------------------------------
   FOdysseyTexture2DEditorModule
-----------------------------------------------------------------------------*/

TSharedRef<FOdysseyTexture2DEditorToolkit>
FOdysseyTexture2DEditorModule::CreateOdysseyTexture2DEditor( UTexture2D* iTexture )
{
	TSharedPtr<FOdysseyTexture2DEditor> editor = MakeShareable(new FOdysseyTexture2DEditor(iTexture));
    TSharedPtr<FOdysseyTexture2DEditorToolkit> toolkit = MakeShareable( new FOdysseyTexture2DEditorToolkit(editor) );
	editor->Initialize(iTexture);
    toolkit->Initialize();
    return toolkit.ToSharedRef();
}

void
FOdysseyTexture2DEditorModule::StartupModule()
{
	// Register Assets Types Actions
	RegisterAssetTypeActions();

	// Register Commands
	RegisterCommands();

	// Register Settings
    RegisterSettings();

	// Install Content Browser Extionsion Hooks
	if (!IsRunningCommandlet())
	{
		FOdysseyTexture2DContentBrowserExtensions::InstallHooks();
	}
}

void
FOdysseyTexture2DEditorModule::ShutdownModule()
{
	// Uninstall Content Browser Extionsion Hooks
	FOdysseyTexture2DContentBrowserExtensions::RemoveHooks();

	// Unregister Settings
    UnregisterSettings();

	// Unregister Commands
	UnregisterCommands();

	// Unregister Assets Type Actions
	UnregisterAssetTypeActions();
}

void
FOdysseyTexture2DEditorModule::RegisterAssetTypeActions()
{
	IAssetTools& assetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();

	// Create Asset Categories
	EAssetTypeCategories::Type category = assetTools.RegisterAdvancedAssetCategory(FName(TEXT("ILIAD")), LOCTEXT("IliadPainterAssetCategory", "ILIAD"));

	//Create Asset Types Actions
	mTypeActions.Add(MakeShareable(new FOdysseyTexture2DAssetTypeActions(category)));

	//Register created Asset Type Actions
	for (int32 index = 0; index < mTypeActions.Num(); ++index)
	{
		assetTools.RegisterAssetTypeActions(mTypeActions[index].ToSharedRef());
	}
}

void
FOdysseyTexture2DEditorModule::UnregisterAssetTypeActions()
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
FOdysseyTexture2DEditorModule::RegisterSettings()
{
    ISettingsModule* settingsModule = FModuleManager::GetModulePtr<ISettingsModule>( "Settings" );
    if( !settingsModule )
		return;

	settingsModule->RegisterSettings( "Editor", "Plugins", "ILIADTexture2DEditor"
										, LOCTEXT( "OdysseyTextureEditor2DSettingsName", "ILIAD Texture2D Editor" )
										, LOCTEXT( "OdysseyTextureEditor2DSettingsDescription", "Configure the look and feel of the ILIAD Editor." )
										, GetMutableDefault<UOdysseyTexture2DEditorSettings>() );
}

void
FOdysseyTexture2DEditorModule::UnregisterSettings()
{
    ISettingsModule* settingsModule = FModuleManager::GetModulePtr<ISettingsModule>( "Settings" );

    if( !settingsModule )
		return;
    
	settingsModule->UnregisterSettings( "Editor", "Plugins", "ILIADTexture2DEditor" );
}

void
FOdysseyTexture2DEditorModule::RegisterCommands()
{
	FOdysseyTexture2DEditorCommands::Register();
}

void
FOdysseyTexture2DEditorModule::UnregisterCommands()
{
	FOdysseyTexture2DEditorCommands::Unregister();
}

IMPLEMENT_MODULE( FOdysseyTexture2DEditorModule, OdysseyTexture2DEditor );

#undef LOCTEXT_NAMESPACE
