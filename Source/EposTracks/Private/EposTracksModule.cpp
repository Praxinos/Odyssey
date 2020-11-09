// IDDN FR.001.250001.004.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "EposTracksModule.h"

#define LOCTEXT_NAMESPACE "FEposTracksModule"

void FEposTracksModule::StartupModule()
{
    // This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FEposTracksModule::ShutdownModule()
{
    // This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
    // we call this function before unloading the module.
}

//---

//static
FTracksCustomizationManager&
FEposTracksModule::GetTracksCustomizationManager()
{
    return FModuleManager::LoadModuleChecked<FEposTracksModule>( "EposTracks" ).mTracksCustomizationManager;
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE( FEposTracksModule, EposTracks )
