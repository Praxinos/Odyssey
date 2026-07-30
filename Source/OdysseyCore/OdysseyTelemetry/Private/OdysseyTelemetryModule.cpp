// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyTelemetryModule.h"

#include "OdysseyTelemetry.h"
#include "OdysseyTelemetryLog.h"

DEFINE_LOG_CATEGORY( LogOdysseyTelemetry );

IMPLEMENT_MODULE( FOdysseyTelemetryModule, OdysseyTelemetry );

//---

void
FOdysseyTelemetryModule::StartupModule()
{
#if !UE_BUILD_SHIPPING
    FOdysseyTelemetry::Get().StartSession();
#endif
}

void
FOdysseyTelemetryModule::ShutdownModule()
{
#if !UE_BUILD_SHIPPING
    FOdysseyTelemetry::Get().EndSession();
#endif
}

//---

//static
FOdysseyTelemetryModule&
FOdysseyTelemetryModule::Get()
{
    static const FName ModuleName = "OdysseyTelemetry";
    return FModuleManager::LoadModuleChecked<FOdysseyTelemetryModule>( ModuleName );
}

//static
bool
FOdysseyTelemetryModule::IsAvailable()
{
    static const FName ModuleName = "OdysseyTelemetry";
    return FModuleManager::Get().IsModuleLoaded( ModuleName );
}

//---

void
FOdysseyTelemetryModule::RegisterAssetClassToTrackForCreation( UClass* iClass )
{
    AssetClassToTrackForCreation.Add( iClass );
}

void
FOdysseyTelemetryModule::UnregisterAssetClassToTrackForCreation( UClass* iClass )
{
    AssetClassToTrackForCreation.Remove( iClass );
}

bool
FOdysseyTelemetryModule::IsAssetClassToTrackForCreation( UClass* iClass ) const
{
    return AssetClassToTrackForCreation.Contains( iClass );
}
