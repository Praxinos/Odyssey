// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyLayerStackEditorModule.h"

#include "Modules/ModuleManager.h"

#include "Commands/OdysseyLayerStackEditorCommands.h"
#include "OdysseyLayerStackSelection.h"

void FOdysseyLayerStackEditorModule::StartupModule()
{
    RegisterCommands();

    // Load the dependent TypedElementFramework module (holding TypedElementRegistry)
    // Otherwise, OdysseyLayerStackSelection::Initialize() tries to access TypedElementRegistry and randomly crashes
    // This also done in some Unreal Engines Plugins / Modules
    FModuleManager::Get().LoadModule(TEXT("TypedElementFramework"));
    OdysseyLayerStackSelection::Initialize();
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
