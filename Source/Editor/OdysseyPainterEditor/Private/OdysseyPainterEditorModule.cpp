// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyPainterEditorModule.h"

#include "AssetToolsModule.h"
#include "CoreMinimal.h"
#include "ISettingsModule.h"
#include "LevelEditor.h"
#include "PropertyEditorModule.h"
#include "Settings/ContentBrowserSettings.h"
#include "Toolkits/AssetEditorToolkit.h"
#include "OdysseyPainterEditorSettings.h"
#include "Models/OdysseyPainterEditorCommands.h"
#include <ULIS3>

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


void
FOdysseyPainterEditorModule::StartupModule()
{
    // Perform test
    SimpleTestFramework f;
    f.RunTest();

    RegisterSettings();
    RegisterCommands();
}

void
FOdysseyPainterEditorModule::ShutdownModule()
{
    UnregisterSettings();
    UnregisterCommands();
}

void
FOdysseyPainterEditorModule::RegisterSettings()
{
    ISettingsModule* settingsModule = FModuleManager::GetModulePtr<ISettingsModule>( "Settings" );

    if( !settingsModule )
        return;

    settingsModule->RegisterSettings( "Editor", "Plugins", "ILIADPainterEditor"
                                        , LOCTEXT( "OdysseyPainterEditorSettingsName", "ILIAD Painter Editor" )
                                        , LOCTEXT( "OdysseyPainterEditorSettingsDescription", "Configure the look and feel of the ILIAD Editor." )
                                        , GetMutableDefault<UOdysseyPainterEditorSettings>() );
}

void
FOdysseyPainterEditorModule::UnregisterSettings()
{
    ISettingsModule* settingsModule = FModuleManager::GetModulePtr<ISettingsModule>( "Settings" );

    if( !settingsModule )
        return;
        
    settingsModule->UnregisterSettings( "Editor", "Plugins", "OdysseyPainterEditor" );
}

void
FOdysseyPainterEditorModule::RegisterCommands()
{
    FOdysseyPainterEditorCommands::Register();
}

void
FOdysseyPainterEditorModule::UnregisterCommands()
{
    FOdysseyPainterEditorCommands::Unregister();
}

IMPLEMENT_MODULE( FOdysseyPainterEditorModule, OdysseyPainterEditor );

#undef LOCTEXT_NAMESPACE
