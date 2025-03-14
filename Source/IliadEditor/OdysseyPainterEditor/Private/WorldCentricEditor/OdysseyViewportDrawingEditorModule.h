// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"

/**
 * FOdysseyViewportDrawingEditor module interface
 */
class FOdysseyViewportDrawingEditorModule : public IModuleInterface
{
public:
    // IModuleInterface interface
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

private:
};

    //Commands
    void RegisterCommands();
    void UnregisterCommands();
