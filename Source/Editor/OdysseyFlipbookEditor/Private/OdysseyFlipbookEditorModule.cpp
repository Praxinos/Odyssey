// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyFlipbookEditorModule.h"

#include "AssetToolsModule.h"
#include "CoreMinimal.h"
#include "ISettingsModule.h"
#include "LevelEditor.h"
#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"
#include "Settings/ContentBrowserSettings.h"
#include "Toolkits/AssetEditorToolkit.h"
#include "PaperFlipbook.h"

#include "OdysseyFlipbookEditor.h"
#include "OdysseyFlipbookEditorSettings.h"
#include "OdysseyFlipbookEditorToolkit.h"
#include "OdysseyFlipbookAssetTypeActions.h"

#define LOCTEXT_NAMESPACE "OdysseyFlipbookEditorModule"

/*-----------------------------------------------------------------------------
   FOdysseyFlipbookEditorModule
-----------------------------------------------------------------------------*/

void
FOdysseyFlipbookEditorModule::StartupModule()
{
    mIliadTypeActions = nullptr;
    mUETypeActions = nullptr;

    // Register Assets Types Actions once the main loop is initialized
    // see here : https://udn.unrealengine.com/s/question/0D54z00007DVU5KCAX/two-assettypeactions-for-the-same-type-force-priority-
    FCoreDelegates::OnFEngineLoopInitComplete.AddRaw(this, &FOdysseyFlipbookEditorModule::RegisterAssetTypeActions);

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
    // Unregister Assets Types Actions
    FCoreDelegates::OnFEngineLoopInitComplete.RemoveAll(this);

	// Uninstall Content Browser Extionsion Hooks
	FOdysseyFlipbookContentBrowserExtensions::RemoveHooks();

	// Unregister Settings
    UnregisterSettings();

	// Unregister Commands
	UnregisterCommands();

	// Unregister Assets Type Actions
	UnregisterAssetTypeActions();
}

void
FOdysseyFlipbookEditorModule::RegisterAssetTypeActions()
{
	IAssetTools& assetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
    // Create Asset Categories
	EAssetTypeCategories::Type category = assetTools.RegisterAdvancedAssetCategory(FName(TEXT("ILIAD")), LOCTEXT("IliadPainterAssetCategory", "ILIAD"));

    if( !mUETypeActions )
        mUETypeActions = assetTools.GetAssetTypeActionsForClass(UPaperFlipbook::StaticClass() ).Pin();
    if( !mIliadTypeActions )
	    mIliadTypeActions = MakeShareable(new FOdysseyFlipbookAssetTypeActions(category));

    if( UOdysseyFlipbookEditorSettings::Get()->IliadDefaultEditorEnabled )
    {
	    // Remove old AssetTypeAction from UE
        assetTools.UnregisterAssetTypeActions(mUETypeActions.ToSharedRef());

        //Register created Asset Type Actions
        assetTools.RegisterAssetTypeActions(mIliadTypeActions.ToSharedRef());

    }
    else
    {
	    // Remove old AssetTypeAction
	    assetTools.UnregisterAssetTypeActions(mIliadTypeActions.ToSharedRef());

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
    assetTools.UnregisterAssetTypeActions(mIliadTypeActions.ToSharedRef());
    assetTools.UnregisterAssetTypeActions(mUETypeActions.ToSharedRef());
}

void
FOdysseyFlipbookEditorModule::RegisterSettings()
{
    ISettingsModule* settingsModule = FModuleManager::GetModulePtr<ISettingsModule>( "Settings" );
    if( !settingsModule )
		return;

	settingsModule->RegisterSettings( "Editor", "Plugins", "ILIADFlipbookEditor"
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
    
	settingsModule->UnregisterSettings( "Editor", "Plugins", "OdysseyFlipbookEditor" );
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
