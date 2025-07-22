// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"

/**
 * The Brush Asset module.
 */
class FOdysseyBrushModule :
    public IModuleInterface
{
public:
    // IModuleInterface interface
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
    // End of IModuleInterface interface

private:
    //BrushOverrides Customizations
    void RegisterBrushCustomizations();
    void UnregisterBrushCustomizations();
};
