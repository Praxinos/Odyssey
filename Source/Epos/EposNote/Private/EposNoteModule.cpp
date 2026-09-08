// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "EposNoteModule.h"

#include "OdysseyTelemetryModule.h"
#include "StoryNote.h"

#define LOCTEXT_NAMESPACE "FEposNoteModule"

void FEposNoteModule::StartupModule()
{
    // This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

    FOdysseyTelemetryModule::Get().RegisterAssetClassToTrackForCreation( UStoryNote::StaticClass() );

}

void FEposNoteModule::ShutdownModule()
{
    // This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
    // we call this function before unloading the module.

    FOdysseyTelemetryModule::Get().UnregisterAssetClassToTrackForCreation( UStoryNote::StaticClass() );
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE( FEposNoteModule, EposNote )
