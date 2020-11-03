// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "AssetToolsModule.h"
#include "CoreMinimal.h"
#include "ISettingsModule.h"
#include "LevelEditor.h"
#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"
#include "Settings/ContentBrowserSettings.h"
#include "Toolkits/AssetEditorToolkit.h"

#include "OdysseyPainterEditorSettings.h"
#include "OdysseyPainterEditorToolkit.h"

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
    : public IModuleInterface
{
public:
    // IModuleInterface interface

    virtual void StartupModule() override
    {
        // Perform test
        SimpleTestFramework f;
        f.RunTest();

        // register settings
        ISettingsModule* settingsModule = FModuleManager::GetModulePtr<ISettingsModule>( "Settings" );

        if( settingsModule )
        {
            settingsModule->RegisterSettings( "Editor", "ContentEditors", "ILIADPainterEditor"
                                              , LOCTEXT( "OdysseyPainterEditorSettingsName", "ILIAD Painter Editor" )
                                              , LOCTEXT( "OdysseyPainterEditorSettingsDescription", "Configure the look and feel of the ILIAD Editor." )
                                              , GetMutableDefault<UOdysseyPainterEditorSettings>() );
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
    }
};

IMPLEMENT_MODULE( FOdysseyPainterEditorModule, OdysseyPainterEditor );

#undef LOCTEXT_NAMESPACE
