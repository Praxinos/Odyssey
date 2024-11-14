// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyPainterEditorModule.h"

#include "FOdysseyVectorBrushCustomization.h"
#include "FOdysseyVectorObjectViewPaletteCustomization.h"

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
#include "Tools/RasterDrawingTool/OdysseyPainterEditorRasterDrawingToolOverrides.h"

#include "PainterEditor/OdysseyPainterEditorGUI.h"
#include <ULIS>

#define LOCTEXT_NAMESPACE "PainterEditor"

/*-----------------------------------------------------------------------------
   FOdysseyPainterEditorModule
-----------------------------------------------------------------------------*/


void
FOdysseyPainterEditorModule::StartupModule()
{
    RegisterBrushOverrides(); //First thing to do, as it modifies the Brush CDO
    RegisterSettings();
    RegisterCommands();
    RegisterLevelEditorLayoutExtensions();

    FOdysseyVectorBrushCustomization::Register();
    FOdysseyVectorObjectViewPaletteCustomization::Register();
}

void
FOdysseyPainterEditorModule::ShutdownModule()
{
    UnregisterBrushOverrides();
    UnregisterSettings();
    UnregisterCommands();
    UnregisterLevelEditorLayoutExtensions();

    FOdysseyVectorBrushCustomization::Unregister();
    FOdysseyVectorObjectViewPaletteCustomization::Unregister();
}

void
FOdysseyPainterEditorModule::RegisterSettings()
{
    ISettingsModule* settingsModule = FModuleManager::GetModulePtr<ISettingsModule>( "Settings" );

    if( !settingsModule )
        return;

    settingsModule->RegisterSettings( "Editor", "Plugins", "OdysseyPainterEditor"
                                        , LOCTEXT( "settings.name", "Odyssey Painter Editor" )
                                        , LOCTEXT( "settings.tooltip", "Configure the look and feel of the Odyssey Editor." )
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
    FOdysseyBrushOverride::Register(UOdysseyPainterEditorRasterDrawingToolOverrides::StaticClass());
}

void
FOdysseyPainterEditorModule::UnregisterBrushOverrides()
{

}

void
FOdysseyPainterEditorModule::RegisterLevelEditorLayoutExtensions()
{
    FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>("LevelEditor");
    mExtendLevelEditorLayout = LevelEditorModule.OnRegisterLayoutExtensions().AddStatic(&FOdysseyPainterEditorGUI::ExtendLevelEditorLayout);
}

void
FOdysseyPainterEditorModule::UnregisterLevelEditorLayoutExtensions()
{
    FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>("LevelEditor");
    LevelEditorModule.OnRegisterLayoutExtensions().Remove(mExtendLevelEditorLayout);
}

IMPLEMENT_MODULE( FOdysseyPainterEditorModule, OdysseyPainterEditor );

#undef LOCTEXT_NAMESPACE
