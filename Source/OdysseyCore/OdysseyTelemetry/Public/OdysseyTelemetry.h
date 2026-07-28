// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"

#include "AnalyticsEventAttribute.h"
#include "Modules/ModuleManager.h"

class IAnalyticsProvider;
class IAnalyticsSpan;
class IAnalyticsTracer;
class FOdysseyAnalyticsProvider;

/**
 *  Public implementation of Odyssey provider module
 */
class FOdysseyTelemetry : FNoncopyable
{
public:
    /** Access to the module singleton*/
    static ODYSSEYTELEMETRY_API FOdysseyTelemetry& Get();

public:
    /** Helper function to determine if the provider is valid. */
    ODYSSEYTELEMETRY_API bool IsAvailable()
    {
        return OdysseyAnalyticsProvider.IsValid();
    }

    /** Access to the analytics provider for the system*/
    ODYSSEYTELEMETRY_API TWeakPtr<IAnalyticsProvider> GetProvider();

    /** Access to the tracer for the system*/
    ODYSSEYTELEMETRY_API TWeakPtr<IAnalyticsTracer> GetTracer();

    /** Called to initialize the singleton. */
    ODYSSEYTELEMETRY_API void StartSession();

    /** Called to shut down the singleton */
    ODYSSEYTELEMETRY_API void EndSession();

public:
    /** Start a new span specifying the parent*/
    ODYSSEYTELEMETRY_API TSharedPtr<IAnalyticsSpan> StartSpan( const FName Name, const TArray<FAnalyticsEventAttribute>& AdditionalAttributes = {} );

    /** Start a new span specifying the parent*/
    ODYSSEYTELEMETRY_API TSharedPtr<IAnalyticsSpan> StartSpan( const FName Name, TSharedPtr<IAnalyticsSpan> ParentSpan, const TArray<FAnalyticsEventAttribute>& AdditionalAttributes = {} );

    /** End an existing span*/
    ODYSSEYTELEMETRY_API bool EndSpan( TSharedPtr<IAnalyticsSpan> Span, const TArray<FAnalyticsEventAttribute>& AdditionalAttributes = {} );

    /** End an existing span by name*/
    ODYSSEYTELEMETRY_API bool EndSpan( const FName Name, const TArray<FAnalyticsEventAttribute>& AdditionalAttributes = {} );

    /** Get an active span by name, non active spans will not be available*/
    ODYSSEYTELEMETRY_API TSharedPtr<IAnalyticsSpan> GetSpan( const FName Name );

    /** Get the root session span*/
    ODYSSEYTELEMETRY_API TSharedPtr<IAnalyticsSpan> GetSessionSpan() const;

private:
    void RegisterOnAssetCreation();

private:
    /** Configure the plugin*/
    void LoadConfiguration();

    struct FConfig
    {
        bool bSendTelemetry = true; // Only send telemetry data if we have been requested to
        //bool bSendUserData = false;  // Never send user data unless specifically asked to
        //bool bSendHardwareData = false; // Never send hardware data unless specifically asked to
        //bool bSendOSData = false; // Never send operating system data unless specifically asked to
        bool bSendSessionContext = false; // Send the session context data with every event
    };

private:
    bool                                    bIsInitialized = false;
    FConfig                                 Config;
    TSharedPtr<FOdysseyAnalyticsProvider>   OdysseyAnalyticsProvider;
    TSharedPtr<IAnalyticsTracer>            OdysseyAnalyticsTracer;

    //FCriticalSection                        CriticalSection; // useful ???

    FDateTime                               SessionStartTime;
};
