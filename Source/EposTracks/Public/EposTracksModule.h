// Copyright © 2020 Praxinos, Inc. All Rights Reserved.
// IDDN 

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
