// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyFlipbookEditorModule.h"

#include "TextureEditor/OdysseyTextureEditorExtension.h"
#include "FlipbookEditor/OdysseyFlipbookEditorExtension.h"
#include "OdysseyFlipbookEditorToolkit.h"
#include "ISettingsModule.h"
#include "FlipbookEditor/OdysseyFlipbookEditorGUI.h"
#include "LevelEditor.h"

#define LOCTEXT_NAMESPACE "FlipbookEditor"

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

	RegisterLevelEditorLayoutExtensions();
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

	UnregisterLevelEditorLayoutExtensions();
}

void
FOdysseyFlipbookEditorModule::RegisterAssetTypeActions()
{
	IAssetTools& assetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
    // Create Asset Categories
	EAssetTypeCategories::Type category = assetTools.RegisterAdvancedAssetCategory(FName(TEXT("ILIAD")), LOCTEXT("asset-category.name", "ILIAD"));

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
										, LOCTEXT( "settings.name", "ILIAD Flipbook Editor" )
										, LOCTEXT( "settings.tooltip", "Configure the look and feel of the ILIAD Editor." )
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

void
FOdysseyFlipbookEditorModule::CreateOdysseyFlipbookEditor( TArray<UPaperFlipbook*> iFlipbooks )
{
	UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();
    bool warningDisplayed = false;
    for( auto FlipbookIt = iFlipbooks.CreateConstIterator(); FlipbookIt; ++FlipbookIt )
    {
		UPaperFlipbook* Flipbook = *FlipbookIt;

		//PATCH: To avoid opening ILIAD when another editor for this asset is opened
		// To make it right, we should use AssetEditorSubsystem->OpenEditorForAsset, but for now it would call the default editor instead of ILIAD
        if (AssetEditorSubsystem->FindEditorForAsset(Flipbook, true) != nullptr)
            continue;

        bool editorFound = false;

        for (int i = 0; i < Flipbook->GetNumKeyFrames(); i++)
        {
            UPaperSprite* sprite = Flipbook->GetKeyFrameChecked(i).Sprite;
            if (!sprite)
                continue;

            if (AssetEditorSubsystem->FindEditorForAsset(sprite, true) != nullptr)
            {
                editorFound = true;
                break;
            }

            UTexture2D* texture = sprite->GetSourceTexture();
            if (!texture)
                continue;

            if (AssetEditorSubsystem->FindEditorForAsset(texture, true) != nullptr)
            {
                editorFound = true;
                break;
            }
        }

        if (editorFound)
        {
            if (!warningDisplayed)
            {
                EditFlipbooksWarning();
                warningDisplayed = true;
            }
            continue;
        }

		TSharedPtr<FOdysseyPainterEditor> editor = MakeShared<FOdysseyPainterEditor>(
			TEXT("OdysseyFlipbookEditor"),
			LOCTEXT("main-menu.category", "Odyssey Flipbook Editor"),
			Flipbook,
			"OdysseyFlipbookEditor_Layout"
		);

		TSharedRef<FOdysseyTextureEditorExtension> textureExtension = MakeShared<FOdysseyTextureEditorExtension>(editor.Get());
		TSharedRef<FOdysseyFlipbookEditorExtension> flipbookExtension = MakeShared<FOdysseyFlipbookEditorExtension>(editor.Get());

		editor->AddExtension(textureExtension);
		editor->AddExtension(flipbookExtension);

		TSharedPtr<FOdysseyFlipbookEditorToolkit> toolkit = MakeShared<FOdysseyFlipbookEditorToolkit>();
		toolkit->Initialize(Flipbook, editor);

		flipbookExtension->SetFlipbook(Flipbook);
	}
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

void
FOdysseyFlipbookEditorModule::RegisterLevelEditorLayoutExtensions()
{
    FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>("LevelEditor");
	mExtendLevelEditorLayout = LevelEditorModule.OnRegisterLayoutExtensions().AddStatic(&FOdysseyFlipbookEditorGUI::ExtendLevelEditorLayout);
}

void
FOdysseyFlipbookEditorModule::UnregisterLevelEditorLayoutExtensions()
{
    FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>("LevelEditor");
	LevelEditorModule.OnRegisterLayoutExtensions().Remove(mExtendLevelEditorLayout);
}

IMPLEMENT_MODULE( FOdysseyFlipbookEditorModule, OdysseyFlipbookEditor );

#undef LOCTEXT_NAMESPACE
