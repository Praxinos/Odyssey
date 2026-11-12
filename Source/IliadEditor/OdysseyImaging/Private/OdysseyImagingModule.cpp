// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyImagingModule.h"

#include "Modules/ModuleManager.h"
#include "OdysseyTileManager.h"
#include "OdysseyTileManagerSettings.h"
#include "ISettingsModule.h"

#define LOCTEXT_NAMESPACE "Imaging"

#if WITH_DEV_AUTOMATION_TESTS
namespace Odyssey::TileCacheTests
{
    bool RunWorkerIfRequested();
}
#endif

void FOdysseyImagingModule::StartupModule()
{
#if WITH_DEV_AUTOMATION_TESTS
    if (Odyssey::TileCacheTests::RunWorkerIfRequested())
    {
        return;
    }
#endif
    RegisterSettings();
    FOdysseyTileManager::Get().Initialize();
}

void FOdysseyImagingModule::ShutdownModule()
{
    UnregisterSettings();
    FOdysseyTileManager::Get().Finalize();
}

void
FOdysseyImagingModule::RegisterSettings()
{
    ISettingsModule* settingsModule = FModuleManager::GetModulePtr<ISettingsModule>( "Settings" );

    if( !settingsModule )
        return;

    settingsModule->RegisterSettings( "Editor", "Plugins", "OdysseyTileManager"
        , LOCTEXT( "user-settings.tile-manager.name", "Odyssey - Tile Manager" )
        , LOCTEXT( "user-settings.tile-manager.tooltip", "Configure Odyssey Tile Manager." )
        , GetMutableDefault<UOdysseyTileManagerSettings>() );
}

void
FOdysseyImagingModule::UnregisterSettings()
{
    ISettingsModule* settingsModule = FModuleManager::GetModulePtr<ISettingsModule>( "Settings" );

    if( !settingsModule )
        return;

    settingsModule->UnregisterSettings( "Editor", "Plugins", "OdysseyTileManager" );
}

IMPLEMENT_MODULE(FOdysseyImagingModule, OdysseyImaging);

#undef LOCTEXT_NAMESPACE
