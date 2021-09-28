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

#include "OdysseyTextureEditor.h"
#include "OdysseyTextureEditorSettings.h"

#define LOCTEXT_NAMESPACE "OdysseyTextureEditorModule"

/*-----------------------------------------------------------------------------
   FOdysseyTextureEditorModule
-----------------------------------------------------------------------------*/

void
FOdysseyTextureEditorModule::StartupModule()
{
	// Register Commands
	RegisterCommands();

	// Register Settings
    RegisterSettings();
}

void
FOdysseyTextureEditorModule::ShutdownModule()
{
	// Unregister Settings
    UnregisterSettings();

	// Unregister Commands
	UnregisterCommands();
}

void
FOdysseyTextureEditorModule::RegisterSettings()
{
    ISettingsModule* settingsModule = FModuleManager::GetModulePtr<ISettingsModule>( "Settings" );
    if( !settingsModule )
		return;

	settingsModule->RegisterSettings( "Editor", "Plugins", "ILIADTextureEditor"
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
    
	settingsModule->UnregisterSettings( "Editor", "Plugins", "OdysseyTextureEditor" );
}

void
FOdysseyTextureEditorModule::RegisterCommands()
{
	FOdysseyTextureEditorCommands::Register();
}

void
FOdysseyTextureEditorModule::UnregisterCommands()
{
	FOdysseyTextureEditorCommands::Unregister();
}

IMPLEMENT_MODULE( FOdysseyTextureEditorModule, OdysseyTextureEditor );

#undef LOCTEXT_NAMESPACE
