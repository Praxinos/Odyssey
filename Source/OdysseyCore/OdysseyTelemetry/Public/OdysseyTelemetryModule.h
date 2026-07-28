// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"

#include "Modules/ModuleManager.h"

/**
 *  Public implementation of Odyssey provider module
 */
class FOdysseyTelemetryModule : public IModuleInterface
{
public:
    ODYSSEYTELEMETRY_API virtual void StartupModule() override;
    ODYSSEYTELEMETRY_API virtual void ShutdownModule() override;
};
