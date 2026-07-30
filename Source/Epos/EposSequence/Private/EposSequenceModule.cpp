// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "EposSequenceModule.h"

#include "Board/BoardSequence.h"
#include "INamingFormatter.h"
#include "OdysseyTelemetryModule.h"
#include "Shot/ShotSequence.h"

#define LOCTEXT_NAMESPACE "FEposSequenceModule"

void
FEposSequenceModule::StartupModule()
{
    // This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

    RegisterTelemetry();
}

void
FEposSequenceModule::ShutdownModule()
{
    // This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
    // we call this function before unloading the module.

    UnregisterTelemetry();
}

//---

FEposSequenceModule::FOnNewActorTrackAdded& FEposSequenceModule::OnNewActorTrackAdded()
{
    return mNewActorTrackAdded;
}

//---

void
FEposSequenceModule::RegisterNamingFormatter( UNamingFormatter* iFormatter )
{
    mNamingFormatters.Add( iFormatter );
}

void
FEposSequenceModule::UnregisterNamingFormatter( UNamingFormatter* iFormatter )
{
    mNamingFormatters.Remove( iFormatter );
}

//---

void
FEposSequenceModule::RegisterTelemetry()
{
    FOdysseyTelemetryModule::Get().RegisterAssetClassToTrackForCreation( UBoardSequence::StaticClass() );
    FOdysseyTelemetryModule::Get().RegisterAssetClassToTrackForCreation( UShotSequence::StaticClass() );
}

void
FEposSequenceModule::UnregisterTelemetry()
{
    FOdysseyTelemetryModule::Get().UnregisterAssetClassToTrackForCreation( UBoardSequence::StaticClass() );
    FOdysseyTelemetryModule::Get().UnregisterAssetClassToTrackForCreation( UShotSequence::StaticClass() );
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE( FEposSequenceModule, EposSequence )
