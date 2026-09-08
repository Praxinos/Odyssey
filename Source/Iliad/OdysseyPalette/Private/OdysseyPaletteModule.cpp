// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyPaletteModule.h"

#include "OdysseyTelemetryModule.h"
#include "Palette/OdysseyPalette.h"

void FOdysseyPaletteModule::StartupModule()
{
    FOdysseyTelemetryModule::Get().RegisterAssetClassToTrackForCreation( UOdysseyPalette::StaticClass() );
}

void FOdysseyPaletteModule::ShutdownModule()
{
    FOdysseyTelemetryModule::Get().UnregisterAssetClassToTrackForCreation( UOdysseyPalette::StaticClass() );
}

IMPLEMENT_MODULE(FOdysseyPaletteModule, OdysseyPalette);
