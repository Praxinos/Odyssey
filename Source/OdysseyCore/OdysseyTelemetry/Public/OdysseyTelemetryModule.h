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
    /**
     * Singleton-like access to this module's interface.  This is just for convenience!
     * Beware of calling this during the shutdown phase, though.  Your module might have been unloaded already.
     *
     * @return Returns singleton instance, loading the module on demand if needed
     */
    ODYSSEYTELEMETRY_API static FOdysseyTelemetryModule& Get();

    /**
     * Checks to see if this module is loaded and ready.  It is only valid to call Get() if IsAvailable() returns true.
     *
     * @return True if the module is loaded and ready to use
     */
    ODYSSEYTELEMETRY_API static bool IsAvailable();

public:
    ODYSSEYTELEMETRY_API virtual void StartupModule() override;
    ODYSSEYTELEMETRY_API virtual void ShutdownModule() override;

public:
    ODYSSEYTELEMETRY_API void RegisterAssetClassToTrackForCreation( UClass* iClass );
    ODYSSEYTELEMETRY_API void UnregisterAssetClassToTrackForCreation( UClass* iClass );

    ODYSSEYTELEMETRY_API bool IsAssetClassToTrackForCreation( UClass* iClass ) const;

private:
    TSet<UClass*> AssetClassToTrackForCreation;
};
