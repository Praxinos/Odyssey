// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

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
#include "OdysseyBrushAssetBase.h"
#include "Tools/RasterDrawingTool/OdysseyBrushOptionsOverrides.h"
#include "Tools/RasterDrawingTool/OdysseyBlendParametersOverrides.h"
#include "FreehandShape/OdysseyFreehandShapeOverrides.h"
#include <ULIS>

#define LOCTEXT_NAMESPACE "OdysseyPainterEditorModule"

/*-----------------------------------------------------------------------------
   FOdysseyPainterEditorModule
-----------------------------------------------------------------------------*/


void
FOdysseyPainterEditorModule::StartupModule()
{
    RegisterBrushOverrides(); //First thing to do, as it modifies the Brush CDO
    RegisterSettings();
    RegisterCommands();
}

void
FOdysseyPainterEditorModule::ShutdownModule()
{
    UnregisterBrushOverrides();
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

void
FOdysseyPainterEditorModule::RegisterBrushOverrides()
{
    FOdysseyBrushOverride::Register(UOdysseyBrushOptionsOverrides::StaticClass());
    FOdysseyBrushOverride::Register(UOdysseyBlendParametersOverrides::StaticClass());
    FOdysseyBrushOverride::Register(UOdysseyFreehandShapeOverrides::StaticClass());
}

void
FOdysseyPainterEditorModule::UnregisterBrushOverrides()
{

}

IMPLEMENT_MODULE( FOdysseyPainterEditorModule, OdysseyPainterEditor );

#undef LOCTEXT_NAMESPACE
