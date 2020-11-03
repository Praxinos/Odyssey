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

#include "OdysseyTextureEditorSettings.h"
#include "OdysseyTextureEditorToolkit.h"
#include "OdysseyTexture_AssetTypeActions.h"

#define LOCTEXT_NAMESPACE "OdysseyTextureEditorModule"
const FName OdysseyTextureEditorAppIdentifier = FName( TEXT( "OdysseyTextureEditorApp" ) );

/*-----------------------------------------------------------------------------
   FOdysseyTextureEditorModule
-----------------------------------------------------------------------------*/

TSharedRef<FOdysseyTextureEditorToolkit>
FOdysseyTextureEditorModule::CreateOdysseyTextureEditor(const EToolkitMode::Type iMode, const TSharedPtr< IToolkitHost >& iInitToolkitHost, UTexture2D* iTexture )
{
    TSharedRef<FOdysseyTextureEditorToolkit> newOdysseyTextureEditor( new FOdysseyTextureEditorToolkit() );
    newOdysseyTextureEditor->Init( iMode, iInitToolkitHost, OdysseyTextureEditorAppIdentifier, iTexture );
    
    return newOdysseyTextureEditor;
}

TSharedPtr<FExtensibilityManager>
FOdysseyTextureEditorModule::GetMenuExtensibilityManager()
{
	return mMenuExtensibilityManager;
}

void FOdysseyTextureEditorModule::RegisterAssetTypeAction(IAssetTools& ioAssetTools, TSharedRef<IAssetTypeActions> iAction)
{
	ioAssetTools.RegisterAssetTypeActions(iAction);
	mCreatedAssetTypeActions.Add(iAction);
}

void
FOdysseyTextureEditorModule::StartupModule()
{
    // Register asset types
	IAssetTools& assetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	mOdysseyPainterCategory = assetTools.RegisterAdvancedAssetCategory(FName(TEXT("ILIAD")), LOCTEXT("IliadPainterAssetCategory", "ILIAD"));
	RegisterAssetTypeAction(assetTools, MakeShareable(new FOdysseyTextureAssetTypeActions(mOdysseyPainterCategory)));

	// register menu extensions
	mMenuExtensibilityManager = MakeShareable(new FExtensibilityManager);
    
    // register settings
    ISettingsModule* settingsModule = FModuleManager::GetModulePtr<ISettingsModule>( "Settings" );

    if( settingsModule )
    {
        settingsModule->RegisterSettings( "Editor", "ContentEditors", "ILIADTextureEditor"
                                            , LOCTEXT( "OdysseyTextureEditorSettingsName", "ILIAD Texture Editor" )
                                            , LOCTEXT( "OdysseyTextureEditorSettingsDescription", "Configure the look and feel of the ILIAD Editor." )
                                            , GetMutableDefault<UOdysseyTextureEditorSettings>() );
    }
    
	if (!IsRunningCommandlet())
	{
		FOdysseyTextureContentBrowserExtensions::InstallHooks();
	}
}

void
FOdysseyTextureEditorModule::ShutdownModule()
{
    // unregister settings
    ISettingsModule* settingsModule = FModuleManager::GetModulePtr<ISettingsModule>( "Settings" );

    if( settingsModule )
    {
        settingsModule->UnregisterSettings( "Editor", "ContentEditors", "OdysseyTextureEditor" );
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

	FOdysseyTextureContentBrowserExtensions::RemoveHooks();
}

IMPLEMENT_MODULE( FOdysseyTextureEditorModule, OdysseyTextureEditor );

#undef LOCTEXT_NAMESPACE
