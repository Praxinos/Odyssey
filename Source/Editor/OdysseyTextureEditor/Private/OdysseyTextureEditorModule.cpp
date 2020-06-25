// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

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

void
FOdysseyTextureEditorModule::StartupModule()
{
    // register settings
    ISettingsModule* settingsModule = FModuleManager::GetModulePtr<ISettingsModule>( "Settings" );

    if( settingsModule )
    {
        settingsModule->RegisterSettings( "Editor", "ContentEditors", "ILIADTextureEditor"
                                            , LOCTEXT( "OdysseyTextureEditorSettingsName", "ILIAD Texture Editor" )
                                            , LOCTEXT( "OdysseyTextureEditorSettingsDescription", "Configure the look and feel of the ILIAD Editor." )
                                            , GetMutableDefault<UOdysseyTextureEditorSettings>() );
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
}

IMPLEMENT_MODULE( FOdysseyTextureEditorModule, OdysseyTextureEditor );

#undef LOCTEXT_NAMESPACE
