// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"

/**
 * Implements the Editor style module, loaded by SlateApplication dynamically at startup.
 */
class FOdysseyStyleModule
    : public IModuleInterface
{
public:
    virtual void StartupModule() override;

    virtual void ShutdownModule() override;
};
