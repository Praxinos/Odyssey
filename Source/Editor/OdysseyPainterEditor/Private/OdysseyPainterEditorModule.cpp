// IDDN FR.001.250001.005.S.P.2019.000.00000
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
#include <ULIS>

#define LOCTEXT_NAMESPACE "OdysseyPainterEditorModule"

class SimpleTestFramework {
public:
    void RunTest() {
        const volatile int fenceA = 0;
        ::ULIS::FColor* p = new ::ULIS::FColor( ::ULIS::Format_RGBA8 );
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

    settingsModule->RegisterSettings( "Editor", "Plugins", "OdysseyPainterEditor"
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
