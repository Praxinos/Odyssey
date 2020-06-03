// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "AssetToolsModule.h"
#include "CoreMinimal.h"
#include "OdysseyTexture.h"
#include "ISettingsModule.h"
#include "LevelEditor.h"
#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"
#include "Settings/ContentBrowserSettings.h"
#include "Toolkits/AssetEditorToolkit.h"
#include "OdysseyTextureContentBrowserExtensions.h"


#include "IOdysseyPainterEditorModule.h"
#include "IOdysseyPainterEditorToolkit.h"
#include "OdysseyPainterEditorSettings.h"
#include "OdysseyPainterEditorToolkit.h"
#include "OdysseyTexture_AssetTypeActions.h"

#define LOCTEXT_NAMESPACE "OdysseyPainterEditorModule"

class SimpleTestFramework {
public:
    void RunTest() {
        const volatile int fenceA = 0;
        ::ul3::FPixelValue* p = new ::ul3::FPixelValue( ULIS3_FORMAT_RGBA8 );
        delete p; // No crash
        const volatile int fenceB = 0;
    }
};

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

    // From ...\UnrealEngine\Engine\Source\Editor\ContentBrowser\Private\SAssetView.cpp#3543
    void ShowPluginContentInContentBrowser()
    {
        if( GetDefault<UContentBrowserSettings>()->GetDisplayPluginFolders() )
            return;

        bool bDisplayPlugins = GetDefault<UContentBrowserSettings>()->GetDisplayPluginFolders();
        bool bRawDisplayPlugins = GetDefault<UContentBrowserSettings>()->GetDisplayPluginFolders( true );

        // Only if both these flags are false when toggling we want to enable the flag, otherwise we're toggling off
        if( !bDisplayPlugins && !bRawDisplayPlugins )
        {
            GetMutableDefault<UContentBrowserSettings>()->SetDisplayPluginFolders( true );
        }
        else
        {
            GetMutableDefault<UContentBrowserSettings>()->SetDisplayPluginFolders( false );
            GetMutableDefault<UContentBrowserSettings>()->SetDisplayPluginFolders( false, true );
        }
        GetMutableDefault<UContentBrowserSettings>()->PostEditChange();
    }

public:
    // IModuleInterface interface

    virtual void StartupModule() override
    {
        // Perform test
        SimpleTestFramework f;
        f.RunTest();

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

        //---

        ShowPluginContentInContentBrowser();
        
        if( !IsRunningCommandlet() )
        {
            FOdysseyTextureContentBrowserExtensions::InstallHooks();
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
        
        FOdysseyTextureContentBrowserExtensions::RemoveHooks();
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
