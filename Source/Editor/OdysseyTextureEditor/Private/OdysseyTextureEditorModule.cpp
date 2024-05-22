// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyTextureEditorModule.h"

#include "AssetToolsModule.h"
#include "CoreMinimal.h"
#include "ISettingsModule.h"
#include "LevelEditor.h"
#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"
#include "Settings/ContentBrowserSettings.h"
#include "Toolkits/AssetEditorToolkit.h"

#include "PainterEditor/OdysseyPainterEditor.h"
#include "TextureEditor/OdysseyTextureEditorExtension.h"
#include "OdysseyTextureEditorSettings.h"
#include "OdysseyTextureEditorToolkit.h"
#include "OdysseyTextureAssetTypeActions.h"
#include "OdysseyTextureAssetTypeActions.h"
#include "TextureEditor/OdysseyTextureEditorCommands.h"
#include "TextureEditor/OdysseyTextureEditorSource.h"
#include "TextureEditor/OdysseyTextureEditorGUI.h"
#include "Extensions/OdysseyTextureContentBrowserExtensions.h"

#define LOCTEXT_NAMESPACE "TextureEditor"

/*-----------------------------------------------------------------------------
   FOdysseyTextureEditorModule
-----------------------------------------------------------------------------*/

void
FOdysseyTextureEditorModule::CreateOdysseyTextureEditor( TArray<UTexture2D*> iTextures )
{
	UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem< UAssetEditorSubsystem >();
	bool warningDisplayed = false;
	for( UTexture2D* texture : iTextures )
	{
		//PATCH: To avoid opening ILIAD when another editor for this asset is opened
		// To make it right, we should use AssetEditorSubsystem->OpenEditorForAsset, but for now it would call the default editor instead of ILIAD
		if (AssetEditorSubsystem->FindEditorForAsset(texture, true) != nullptr)
		{
			if (!warningDisplayed)
			{
				FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("content-browser-extension.texture-already-opened-dialog.message", "The texture is already opened in an other editor. Please close the editor before opening the texture with ILIAD."), LOCTEXT("content-browser-extension.edit-texture.texture-already-opened-dialog.title", "Texture Already Opened"));
				warningDisplayed = true;
			}
			continue;
		}

		TSharedPtr<FOdysseyPainterEditor> editor = MakeShared<FOdysseyPainterEditor>(
			TEXT("OdysseyTextureEditor"),
			LOCTEXT("main-menu.category", "Odyssey Texture2D Editor"),
			texture,
			"OdysseyTextureEditor_Layout"
		);

		TSharedRef<FOdysseyTextureEditorExtension> textureExtension = MakeShared<FOdysseyTextureEditorExtension>(editor.Get());
		editor->AddExtension(textureExtension);

		TSharedPtr<FOdysseyTextureEditorToolkit> toolkit = MakeShared<FOdysseyTextureEditorToolkit>();
		toolkit->Initialize(texture, editor);

		//----- Extend the Edit menu using the FMenuBuilder API
		TSharedPtr<FExtender> menuExtender = MakeShareable(new FExtender());
		// Extend the Edit menu after the "Configuration" category
		menuExtender->AddMenuExtension(
			"Configuration",
			EExtensionHook::After,
			NULL,
			FMenuExtensionDelegate::CreateRaw( editor.Get(), &FOdysseyPainterEditor::AddEditMenuEntry )
		);
		toolkit->AddMenuExtender( menuExtender );
		// Rebuild the the menu bar and take into account the above extender. 
		// FOdysseyPainterEditor::AddEditMenuEntry() will then be called each time the Edit menu needs to be shown.
		toolkit->RegenerateMenusAndToolbars();
		//-----

		TSharedPtr<FOdysseyTextureEditorSource> source = MakeShared<FOdysseyTextureEditorSource>(texture);
		editor->SetSource(source);
	}
}

void
FOdysseyTextureEditorModule::StartupModule()
{
    // Register Assets Types Actions once the main loop is initialized
    // see here : https://udn.unrealengine.com/s/question/0D54z00007DVU5KCAX/two-assettypeactions-for-the-same-type-force-priority-
    FCoreDelegates::OnFEngineLoopInitComplete.AddRaw(this, &FOdysseyTextureEditorModule::RegisterAssetTypeActions);

	// Register Commands
	RegisterCommands();

	// Register Settings
    RegisterSettings();

	// Install Content Browser Extionsion Hooks
	if (!IsRunningCommandlet())
	{
		FOdysseyTextureContentBrowserExtensions::InstallHooks();
	}

	RegisterLevelEditorLayoutExtensions();
}

void
FOdysseyTextureEditorModule::ShutdownModule()
{
    // Unregister Assets Types Actions
    FCoreDelegates::OnFEngineLoopInitComplete.RemoveAll(this);

	// Uninstall Content Browser Extionsion Hooks
	FOdysseyTextureContentBrowserExtensions::RemoveHooks();

	// Unregister Settings
    UnregisterSettings();

	// Unregister Commands
	UnregisterCommands();

	// Unregister Assets Type Actions
	UnregisterAssetTypeActions();

	UnregisterLevelEditorLayoutExtensions();
}

void
FOdysseyTextureEditorModule::RegisterAssetTypeActions()
{
	IAssetTools& assetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();

	// Create Asset Categories
	EAssetTypeCategories::Type category = assetTools.RegisterAdvancedAssetCategory(FName(TEXT("ILIAD")), LOCTEXT("asset-category.name", "ILIAD"));

	//Create Asset Types Actions
	mIliadTypeActions = MakeShareable(new FOdysseyTextureAssetTypeActions(category));

	//Register created Asset Type Actions
	assetTools.RegisterAssetTypeActions(mIliadTypeActions.ToSharedRef());
}

void
FOdysseyTextureEditorModule::UnregisterAssetTypeActions()
{
	if (!FModuleManager::Get().IsModuleLoaded("AssetTools"))
		return;
	
	IAssetTools& assetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
    assetTools.UnregisterAssetTypeActions(mIliadTypeActions.ToSharedRef());
}

void
FOdysseyTextureEditorModule::RegisterSettings()
{
    ISettingsModule* settingsModule = FModuleManager::GetModulePtr<ISettingsModule>( "Settings" );
    if( !settingsModule )
		return;

	settingsModule->RegisterSettings( "Editor", "Plugins", "ILIADTexture2DEditor"
										, LOCTEXT( "settings.name", "ILIAD Texture2D Editor" )
										, LOCTEXT( "settings.tooltip", "Configure the look and feel of the ILIAD Editor." )
										, GetMutableDefault<UOdysseyTextureEditorSettings>() );
}

void
FOdysseyTextureEditorModule::UnregisterSettings()
{
    ISettingsModule* settingsModule = FModuleManager::GetModulePtr<ISettingsModule>( "Settings" );

    if( !settingsModule )
		return;
    
	settingsModule->UnregisterSettings( "Editor", "Plugins", "ILIADTexture2DEditor" );
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

void
FOdysseyTextureEditorModule::RegisterLevelEditorLayoutExtensions()
{
    FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>("LevelEditor");
	mExtendLevelEditorLayout = LevelEditorModule.OnRegisterLayoutExtensions().AddStatic(&FOdysseyTextureEditorGUI::ExtendLevelEditorLayout);
}

void
FOdysseyTextureEditorModule::UnregisterLevelEditorLayoutExtensions()
{
    FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>("LevelEditor");
	LevelEditorModule.OnRegisterLayoutExtensions().Remove(mExtendLevelEditorLayout);
}

IMPLEMENT_MODULE( FOdysseyTextureEditorModule, OdysseyTextureEditor );

#undef LOCTEXT_NAMESPACE
