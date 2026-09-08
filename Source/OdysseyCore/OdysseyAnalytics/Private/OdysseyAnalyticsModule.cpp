// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyAnalyticsModule.h"
#include "IAnalyticsProviderET.h"

#include "Analytics.h"
//#include "AnalyticsPerfTracker.h" // Private
#include "AnalyticsProviderETEventCache.h"
#include "HttpModule.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/EngineVersion.h"

#include "OdysseyAnalyticsProvider.h"

IMPLEMENT_MODULE( FOdysseyAnalyticsModule, OdysseyAnalytics );

void FOdysseyAnalyticsModule::StartupModule()
{
    // Make sure http is loaded so that we can flush events during module shutdown
    FModuleManager::LoadModuleChecked<FHttpModule>( "HTTP" );
    FAnalyticsProviderETEventCache::OnStartupModule();
}

void FOdysseyAnalyticsModule::ShutdownModule()
{
//#if ANALYTICS_PERF_TRACKING_ENABLED
//    TearDownAnalyticsPerfTracker();
//#endif
}

TSharedPtr<IAnalyticsProvider> FOdysseyAnalyticsModule::CreateAnalyticsProvider( const FAnalyticsProviderConfigurationDelegate& GetConfigValue ) const
{
    checkNoEntry();
    return nullptr;
    //return CreateOdysseyAnalyticsProvider( GetConfigValue );
}

// Only useful when dynamically loaded (?)
// But in our case, directly use CreateAnalyticsProvider() with a real config parameter
//TSharedPtr<FOdysseyAnalyticsProvider> FOdysseyAnalyticsModule::CreateOdysseyAnalyticsProvider( const FAnalyticsProviderConfigurationDelegate& GetConfigValue ) const
//{
//#ifdef DISABLE_ANALYTICS_PROVIDER
//    UE_LOGF( LogAnalytics, Warning, "CreateOdysseyAnalyticsProvider is disabled in this configuration" );
//    return nullptr;
//#else
//    if( GetConfigValue.IsBound() )
//    {
//        Config ConfigValues;
//        ConfigValues.APIKeyOdyssey = GetConfigValue.Execute( Config::GetKeyNameForAPIKey(), true );
//        ConfigValues.APIServerOdyssey = GetConfigValue.Execute( Config::GetKeyNameForAPIServer(), true );
//        ConfigValues.APIEndpointOdyssey = GetConfigValue.Execute( Config::GetKeyNameForAPIEndpoint(), true );
//        ConfigValues.AppVersion = GetConfigValue.Execute( Config::GetKeyNameForAppVersion(), false );
//
//        if( ConfigValues.AppVersion.IsEmpty() )
//        {
//            TSharedPtr<IPlugin> plugin = IPluginManager::Get().FindPlugin( UE_PLUGIN_NAME );
//            const FPluginDescriptor& pluginDescriptor = plugin->GetDescriptor();
//            FString version = pluginDescriptor.VersionName;
//            //if( pluginDescriptor.bIsBetaVersion )
//            //    version += "-beta";
//            ConfigValues.AppVersion = version;
//        }
//
//        return CreateAnalyticsProvider( ConfigValues );
//    }
//    else
//    {
//        UE_LOGF( LogAnalytics, Warning, "CreateOdysseyAnalyticsProvider called with an unbound delegate" );
//    }
//
//    return nullptr;
//#endif
//}
