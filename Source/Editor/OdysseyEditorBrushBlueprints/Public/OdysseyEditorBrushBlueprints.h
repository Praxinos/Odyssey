// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"

/**
 * The Brush Asset module.
 */
class FOdysseyEditorBrushBlueprints :
	public IModuleInterface
{
public:
    // IModuleInterface interface
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
	// End of IModuleInterface interface
};
