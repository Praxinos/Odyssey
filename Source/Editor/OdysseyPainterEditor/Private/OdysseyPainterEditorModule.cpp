// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "AssetToolsModule.h"
#include "CoreMinimal.h"
#include "Engine/Texture.h"
#include "ISettingsModule.h"
#include "LevelEditor.h"
#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"
#include "Toolkits/AssetEditorToolkit.h"

#include "IOdysseyPainterEditorModule.h"
#include "IOdysseyPainterEditorToolkit.h"
#include "OdysseyPainterEditorSettings.h"
#include "OdysseyPainterEditorToolkit.h"
#include "OdysseyTextureDummy/OdysseyTextureDummy_AssetTypeActions.h"
#include "OdysseyTextureDummy/OdysseyTextureDummy_ContentBrowserExtensions.h"

#define LOCTEXT_NAMESPACE "OdysseyPainterEditorModule"

/*-----------------------------------------------------------------------------
   FOdysseyPainterEditorModule
-----------------------------------------------------------------------------*/

class FOdysseyPainterEditorModule
    : public IOdysseyPainterEditorModule
{
public:

    // IOdysseyPainterEditorModule interface
    virtual TSharedRef<IOdysseyPainterEditorToolkit> CreateOdysseyPainterEditor( const EToolkitMode::Type iMode, const TSharedPtr< IToolkitHost >& iInitToolkitHost, UTexture2D* iTexture ) override
    {
        TSharedRef<FOdysseyPainterEditorToolkit> newOdysseyPainterEditor( new FOdysseyPainterEditorToolkit() );
        newOdysseyPainterEditor->InitOdysseyPainterEditor( iMode, iInitToolkitHost, iTexture );

        return newOdysseyPainterEditor;
    }

    virtual TSharedPtr<FExtensibilityManager> GetMenuExtensibilityManager() override
    {
        return mMenuExtensibilityManager;
    }

private:
    void RegisterAssetTypeAction( IAssetTools& ioAssetTools, TSharedRef<IAssetTypeActions> iAction )
    {
        ioAssetTools.RegisterAssetTypeActions( iAction );
        mCreatedAssetTypeActions.Add( iAction );
    }

public:
    // IModuleInterface interface

    virtual void StartupModule() override
    {
        // Register asset types

        IAssetTools& assetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>( "AssetTools" ).Get();
        mOdysseyPainterCategory = assetTools.RegisterAdvancedAssetCategory( FName( TEXT( "ILIAD" ) ), LOCTEXT( "IliadPainterAssetCategory", "ILIAD" ) );
        RegisterAssetTypeAction( assetTools, MakeShareable( new FOdysseyTextureAssetTypeActions( mOdysseyPainterCategory ) ) );

        // register menu extensions
        mMenuExtensibilityManager = MakeShareable( new FExtensibilityManager );

        // register settings
        ISettingsModule* settingsModule = FModuleManager::GetModulePtr<ISettingsModule>( "Settings" );

        if( settingsModule )
        {
            settingsModule->RegisterSettings( "Editor", "ContentEditors", "ILIADPainterEditor"
                                              , LOCTEXT( "OdysseyPainterEditorSettingsName", "ILIAD Painter Editor" )
                                              , LOCTEXT( "OdysseyPainterEditorSettingsDescription", "Configure the look and feel of the ILIAD Editor." )
                                              , GetMutableDefault<UOdysseyPainterEditorSettings>() );
        }

        if( !IsRunningCommandlet() )
        {
            FOdysseyPainterContentBrowserExtensions::InstallHooks();
        }
    }

    virtual void ShutdownModule() override
    {
        // unregister settings
        ISettingsModule* settingsModule = FModuleManager::GetModulePtr<ISettingsModule>( "Settings" );

        if( settingsModule )
        {
            settingsModule->UnregisterSettings( "Editor", "ContentEditors", "OdysseyPainterEditor" );
        }

        // unregister menu extensions
        mMenuExtensibilityManager.Reset();

        // Unregister all the asset types that we registered
        if( FModuleManager::Get().IsModuleLoaded( "AssetTools" ) )
        {
            IAssetTools& assetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>( "AssetTools" ).Get();
            for( int32 index = 0; index < mCreatedAssetTypeActions.Num(); ++index )
            {
                assetTools.UnregisterAssetTypeActions( mCreatedAssetTypeActions[index].ToSharedRef() );
            }
        }
    }

private:
    /** All created asset type actions. Cached here so that we can unregister them during shutdown. */
    TArray< TSharedPtr<IAssetTypeActions> > mCreatedAssetTypeActions;

    EAssetTypeCategories::Type mOdysseyPainterCategory;

    // Holds the menu extensibility manager.
    TSharedPtr<FExtensibilityManager> mMenuExtensibilityManager;
};

IMPLEMENT_MODULE( FOdysseyPainterEditorModule, OdysseyPainterEditor );

#undef LOCTEXT_NAMESPACE
