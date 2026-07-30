// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "IMediaPlayerFactory.h"
#include "Modules/ModuleManager.h"
#include "Media/OdysseyAnimationMediaPlayerFactory.h"

/**
 * The Animation Asset module.
 */
class FOdysseyAnimationModule
    : public IModuleInterface
{
public:
    // IModuleInterface interface
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
    // End of IModuleInterface interface

private:
    void RegisterMedia();
    void UnregisterMedia();

    void RegisterTelemetry();
    void UnregisterTelemetry();

public:
    FOdysseyAnimationMediaPlayerFactory mAnimationMediaPlayerFactory;
};
