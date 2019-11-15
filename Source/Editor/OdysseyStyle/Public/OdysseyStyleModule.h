// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.

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
