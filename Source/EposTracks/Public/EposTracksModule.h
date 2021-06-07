// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

#include "TracksCustomizationManager.h"

class FEposTracksModule
    : public IModuleInterface
{
public:
    /** IModuleInterface implementation */
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

public:
    static EPOSTRACKS_API FTracksCustomizationManager& GetTracksCustomizationManager();

private:
    FTracksCustomizationManager mTracksCustomizationManager;
};
