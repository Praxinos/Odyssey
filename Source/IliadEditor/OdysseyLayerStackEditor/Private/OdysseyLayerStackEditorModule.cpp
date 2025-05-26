// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyLayerStackEditorModule.h"

#include "Commands/OdysseyLayerStackEditorCommands.h"
#include "Modules/ModuleManager.h"

void FOdysseyLayerStackEditorModule::StartupModule()
{
    RegisterCommands();
}

void FOdysseyLayerStackEditorModule::ShutdownModule()
{
    UnregisterCommands();
}

void FOdysseyLayerStackEditorModule::RegisterCommands()
{
    FOdysseyLayerStackEditorCommands::Register();
}

void FOdysseyLayerStackEditorModule::UnregisterCommands()
{
    FOdysseyLayerStackEditorCommands::Unregister();
}

IMPLEMENT_MODULE(FOdysseyLayerStackEditorModule, OdysseyLayerStackEditor);
