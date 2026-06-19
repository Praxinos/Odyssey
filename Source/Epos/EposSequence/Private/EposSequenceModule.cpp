// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "EposSequenceModule.h"

#include "INamingFormatter.h"

#define LOCTEXT_NAMESPACE "FEposSequenceModule"

void
FEposSequenceModule::StartupModule()
{
    // This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void
FEposSequenceModule::ShutdownModule()
{
    // This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
    // we call this function before unloading the module.
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

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE( FEposSequenceModule, EposSequence )
