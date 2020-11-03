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

#include "OdysseyFlipbookEditorSettings.h"
#include "OdysseyFlipbookEditorToolkit.h"
#include "OdysseyFlipbook_AssetTypeActions.h"

#define LOCTEXT_NAMESPACE "OdysseyFlipbookEditorModule"
const FName OdysseyFlipbookEditorAppIdentifier = FName( TEXT( "OdysseyFlipbookEditorApp" ) );

/*-----------------------------------------------------------------------------
   FOdysseyFlipbookEditorModule
-----------------------------------------------------------------------------*/

TSharedRef<FOdysseyFlipbookEditorToolkit>
FOdysseyFlipbookEditorModule::CreateOdysseyFlipbookEditor(const EToolkitMode::Type iMode, const TSharedPtr< IToolkitHost >& iInitToolkitHost, UPaperFlipbook* iFlipbook )
{
    TSharedRef<FOdysseyFlipbookEditorToolkit> newOdysseyFlipbookEditor( new FOdysseyFlipbookEditorToolkit() );
    newOdysseyFlipbookEditor->Init( iMode, iInitToolkitHost, OdysseyFlipbookEditorAppIdentifier, iFlipbook );
    
    return newOdysseyFlipbookEditor;
}

TSharedPtr<FExtensibilityManager>
FOdysseyFlipbookEditorModule::GetMenuExtensibilityManager()
{
	return mMenuExtensibilityManager;
}

void
FOdysseyFlipbookEditorModule::RegisterAssetTypeAction(IAssetTools& ioAssetTools, TSharedRef<IAssetTypeActions> iAction)
{
	ioAssetTools.RegisterAssetTypeActions(iAction);
	mCreatedAssetTypeActions.Add(iAction);
}

void
FOdysseyFlipbookEditorModule::StartupModule()
{
	// Register asset types
	IAssetTools& assetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	mOdysseyPainterCategory = assetTools.RegisterAdvancedAssetCategory(FName(TEXT("ILIAD")), LOCTEXT("IliadPainterAssetCategory", "ILIAD"));
	RegisterAssetTypeAction(assetTools, MakeShareable(new FOdysseyFlipbookAssetTypeActions(mOdysseyPainterCategory)));
        
	// register menu extensions
	mMenuExtensibilityManager = MakeShareable(new FExtensibilityManager);
    
    // register settings
    ISettingsModule* settingsModule = FModuleManager::GetModulePtr<ISettingsModule>( "Settings" );
    
    if( settingsModule )
    {
        settingsModule->RegisterSettings( "Editor", "ContentEditors", "ILIADFlipbookEditor"
                                            , LOCTEXT( "OdysseyFlipbookEditorSettingsName", "ILIAD Flipbook Editor" )
                                            , LOCTEXT( "OdysseyFlipbookEditorSettingsDescription", "Configure the look and feel of the ILIAD Editor." )
                                            , GetMutableDefault<UOdysseyFlipbookEditorSettings>() );
    }
    
	if (!IsRunningCommandlet())
	{
		FOdysseyFlipbookContentBrowserExtensions::InstallHooks();
	}
}

void
FOdysseyFlipbookEditorModule::ShutdownModule()
{
    // unregister settings
    ISettingsModule* settingsModule = FModuleManager::GetModulePtr<ISettingsModule>( "Settings" );

    if( settingsModule )
    {
        settingsModule->UnregisterSettings( "Editor", "ContentEditors", "OdysseyFlipbookEditor" );
    }
    
	// unregister menu extensions
	mMenuExtensibilityManager.Reset();
    
	// Unregister all the asset types that we registered
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		IAssetTools& assetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
		for (int32 index = 0; index < mCreatedAssetTypeActions.Num(); ++index)
		{
			assetTools.UnregisterAssetTypeActions(mCreatedAssetTypeActions[index].ToSharedRef());
		}
	}

	FOdysseyFlipbookContentBrowserExtensions::RemoveHooks();
}

IMPLEMENT_MODULE( FOdysseyFlipbookEditorModule, OdysseyFlipbookEditor );

#undef LOCTEXT_NAMESPACE
