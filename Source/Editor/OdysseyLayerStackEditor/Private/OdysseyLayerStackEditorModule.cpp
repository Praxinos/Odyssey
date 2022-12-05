// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyLayerStackEditorModule.h"
#include "Commands/OdysseyLayerStackEditorCommands.h"

#define LOCTEXT_NAMESPACE "OdysseyLayerStackEditorModule"

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

IMPLEMENT_MODULE(FOdysseyLayerStackEditorModule, OdysseyLayerStack);

#undef LOCTEXT_NAMESPACE
