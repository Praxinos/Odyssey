// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "AnalyticsProviderConfigurationDelegate.h"
#include "Containers/Array.h"
#include "Containers/UnrealString.h"
#include "CoreMinimal.h"
#include "HAL/Platform.h"
#include "HAL/PlatformCrt.h"
#include "Interfaces/IAnalyticsProviderModule.h"
#include "Modules/ModuleManager.h"
#include "Templates/SharedPointer.h"
#include "Templates/UnrealTemplate.h"

class IAnalyticsProvider;
class FOdysseyAnalyticsProvider;

/**
 *  Public implementation of Odyssey provider module
 */
class FOdysseyAnalyticsModule : public IAnalyticsProviderModule
{
    //--------------------------------------------------------------------------
    // Module functionality
    //--------------------------------------------------------------------------
public:
    /**
     * Singleton-like access to this module's interface.  This is just for convenience!
     * Beware of calling this during the shutdown phase, though.  Your module might have been unloaded already.
     *
     * @return Returns singleton instance, loading the module on demand if needed
     */
    static inline FOdysseyAnalyticsModule& Get()
    {
        return FModuleManager::LoadModuleChecked< FOdysseyAnalyticsModule >( "OdysseyAnalytics" );
    }

    //--------------------------------------------------------------------------
    // Configuration functionality
    //--------------------------------------------------------------------------
public:
    /**
     * Defines required configuration values for Odyssey analytics provider.
     * APIKeyOdyssey MUST be set.
     * Set APIServerOdyssey to an empty string to create a "NULL" analytics provider that will be a valid instance but will suppress sending any events.
    */
    struct Config
    {
        /** Odyssey APIKey - Get from your account manager */
        FString APIKeyOdyssey;
        /** Odyssey API Server - Base URL to send events. Set this to an empty string to essentially create a NULL analytics provider that will be non-null, but won't actually send events. */
        FString APIServerOdyssey;
        /** Odyssey API Endpoint - This is the API endpoint for the provider. */
        FString APIEndpointOdyssey;
        /**
         * AppVersion - defines the app version of the plugin passed to the provider.
         * By default this will be the one in uplugin.
         */
        FString AppVersion;
        /** When true (default), events are dropped if flush fails */
        bool bDropEventsOnFlushFailure = true;
        /** Maximum number of retries to attempt. */
        uint32 RetryLimitCount = 0;
        /** Maximum time to elapse before forcing events to be flushed. Use a negative value to use the defaults (60 sec). */
        float FlushIntervalSec = -1.f;
        /** Maximum size a payload can reach before we force a flush of the payload. Use a negative value to use the defaults. See FAnalyticsProviderETEventCache. */
        int32 MaximumPayloadSize = -1;
        /** We preallocate a payload. It defaults to the Maximum configured payload size (see FAnalyticsProviderETEventCache). Use a negative value use the default. See FAnalyticsProviderETEventCache. */
        int32 PreallocatedPayloadSize = -1;

        /** Default ctor to ensure all values have their proper default. */
        Config() = default;
        /** Ctor exposing common configurables . */
        Config(FString InAPIKeyOdyssey, FString InAPIServerOdyssey, FString InAppVersion = FString(), float InFlushIntervalSec = -1.f, int32 InMaximumPayloadSize = -1, int32 InPreallocatedPayloadSize = -1)
            : APIKeyOdyssey(MoveTemp(InAPIKeyOdyssey ))
            , APIServerOdyssey(MoveTemp(InAPIServerOdyssey ))
            , AppVersion(MoveTemp( InAppVersion ))
            , FlushIntervalSec(InFlushIntervalSec)
            , MaximumPayloadSize(InMaximumPayloadSize)
            , PreallocatedPayloadSize(InPreallocatedPayloadSize)
        {}

        /** KeyName required for APIKey configuration. */
        static FString GetKeyNameForAPIKey() { return TEXT("APIKeyOdyssey"); }
        /** KeyName required for APIServer configuration. */
        static FString GetKeyNameForAPIServer() { return TEXT("APIServerOdyssey"); }
        /** KeyName required for APIEndpoint configuration. */
        static FString GetKeyNameForAPIEndpoint() { return TEXT("APIEndpointOdyssey"); }
        /** KeyName required for AppVersionOdyssey configuration. */
        static FString GetKeyNameForAppVersionOdyssey() { return TEXT("AppVersionOdyssey"); }
        /** KeyName required for AppVersionEngine configuration. */
        static FString GetKeyNameForAppVersionEngine() { return TEXT("AppVersionEngine"); }
        /** Default value if no APIEndpoint is given. */
        static FString GetDefaultAPIEndpoint() { return TEXT("api/v1/public/data"); }
    };

    //--------------------------------------------------------------------------
    // provider factory functions
    //--------------------------------------------------------------------------
public:
    /**
     * IAnalyticsProviderModule interface.
     * Creates the analytics provider given a configuration delegate.
     * The keys required exactly match the field names in the Config object.
     */
    ODYSSEYANALYTICS_API virtual TSharedPtr<IAnalyticsProvider> CreateAnalyticsProvider(const FAnalyticsProviderConfigurationDelegate& GetConfigValue) const override;

    /**
     * Construct an Odyssey analytics provider given a configuration delegate.
     * The keys required exactly match the field names in the Config object.
     */
    //ODYSSEYANALYTICS_API virtual TSharedPtr<FOdysseyAnalyticsProvider> CreateOdysseyAnalyticsProvider(const FAnalyticsProviderConfigurationDelegate& GetConfigValue) const;

    /**
     * Construct an Odyssey analytics provider directly from a config object.
     */
    ODYSSEYANALYTICS_API virtual TSharedPtr<FOdysseyAnalyticsProvider> CreateAnalyticsProvider(const Config& ConfigValues) const;

private:
    ODYSSEYANALYTICS_API virtual void StartupModule() override;
    ODYSSEYANALYTICS_API virtual void ShutdownModule() override;
};
