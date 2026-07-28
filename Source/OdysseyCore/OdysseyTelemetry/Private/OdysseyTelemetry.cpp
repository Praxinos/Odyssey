// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyTelemetry.h"

#include "Analytics.h"
#include "AnalyticsProviderETEventCache.h"
#include "AnalyticsTracer.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "BuildSettings.h"
#include "HttpModule.h"
#include "IAnalyticsProviderET.h"
#include "Interfaces/IPluginManager.h"
#include "Internationalization/Culture.h"
#include "Internationalization/Internationalization.h"
#include "Misc/EngineVersion.h"

#include "OdysseyAnalyticsProvider.h"
#include "OdysseyTelemetryLog.h"

//---

FOdysseyTelemetry&
FOdysseyTelemetry::Get()
{
    static FOdysseyTelemetry OdysseyTelemetryInstance;
    return OdysseyTelemetryInstance;
}

//---

static
FString
CreateAnalyticsUserId( const FString& EpicAccountId )
{
    // EpicAccountId is a parameter as it seems it can change during a session
    // that's why it's done like this in UE_5.8\Engine\Source\Runtime\Engine\Private\EngineAnalytics.cpp
    // but here, for the moment, no delegate used for this behavior
    return FString::Printf( TEXT( "%s|%s|%s" ), *FPlatformMisc::GetLoginId(), *EpicAccountId, *FPlatformMisc::GetOperatingSystemId() );
}

static
TSharedPtr<FOdysseyAnalyticsProvider>
CreatePraxinosAnalyticsProvider()
{
    FString Section( "OdysseyTelemetry.Provider" );
    if( FApp::GetBuildConfiguration() != EBuildConfiguration::Development )
        Section += TEXT( "-Debug" );

    FString ConfigPathfile = GConfig->GetConfigFilename( TEXT( UE_PLUGIN_NAME ) );

    FOdysseyAnalyticsModule::Config Config;

    bool Enabled = true;
    Enabled = GConfig->GetBoolOrDefault( *Section, TEXT( "Enabled" ), true, ConfigPathfile );
    if( !Enabled )
        return nullptr;

    // Validate the usage type is for this build type
    FString UsageType;
    if( !ensure( GConfig->GetString( *Section, TEXT( "UsageType" ), UsageType, ConfigPathfile ) ) )
        return nullptr;

    bool IsValidUseCase = false;
#if WITH_EDITOR
    // Must specify a Editor usage type for this type build
    if( UsageType.Find( TEXT( "Editor" ) ) != INDEX_NONE )
        IsValidUseCase |= true;
#endif

    // For the moment, it's just usable in the editor
//#if WITH_EDITOR
//    // Must specify a Editor usage type for this type build
//    if( UsageType.Find( TEXT( "Editor" ) ) != INDEX_NONE )
//        IsValidUseCase |= true;
//#else
//    // Must specify a Runtime usage type for all non Editor builds
//    if( UsageType.Find( TEXT( "Runtime" ) ) != INDEX_NONE )
//        IsValidUseCase |= true;
//#endif
//
//#if WITH_SERVER_CODE
//    // Must specify a Server usage type for this type build
//    if( UsageType.Find( TEXT( "Server" ) ) != INDEX_NONE )
//        IsValidUseCase |= true;
//#endif
//
//#if WITH_CLIENT_CODE
//    // Must specify a Client usage type for this type build
//    if( UsageType.Find( TEXT( "Client" ) ) != INDEX_NONE )
//        IsValidUseCase |= true;
//#endif

    if( IsValidUseCase == false )
        // This provider is not valid for this use case
        return nullptr;

    //---

    bool found;

    FString APIKey;
    found = GConfig->GetString( *Section, TEXT( "APIKeyOdyssey" ), APIKey, ConfigPathfile );
    check( found );
    Config.APIKeyOdyssey = APIKey;

    FString APIServer;
    found = GConfig->GetString( *Section, TEXT( "APIServerOdyssey" ), APIServer, ConfigPathfile );
    check( found );
    Config.APIServerOdyssey = APIServer;

    FString APIEndpoint;
    found = GConfig->GetString( *Section, TEXT( "APIEndpointOdyssey" ), APIEndpoint, ConfigPathfile );
    check( found );
    Config.APIEndpointOdyssey = APIEndpoint;

    //Config.AppEnvironment = TEXT( "datacollector-binary" );

    // Connect the engine analytics provider (if there is a configuration delegate installed)
    return FOdysseyAnalyticsModule::Get().CreateAnalyticsProvider( Config );
}

// MUST END with "_TelemetryFields"
// MUST BE synchronized with web code
struct SessionStart_TelemetryFields
{
    static inline FString KeyName = TEXT( "OdysseyTelemetry.SessionStart" );

    //---

    // Attributes
    // - added in the SessionStart request
    static inline FString SchemaVersion_KeyName_AsInt32 = TEXT( "SchemaVersion" );

    // DefaultAttributes
    // - added in ALL future requests (then not in SessionStart request)
    static inline FString SessionId_KeyName_AsString = TEXT( "Session_ID" );
    static inline FString SessionStartUTC_KeyName_AsDouble = TEXT( "Session_StartUTC" );

    // SessionAttributes
    // - may be added in DefaultAttributes (in ALL future requests)
    // - or may be added a single time in Attributes only for the SessionStart request
    //static inline FString ProjectName_KeyName_AsString = TEXT( "ProjectName" );
    static inline FString ProjectId_KeyName_AsString = TEXT( "ProjectID" );

    static inline FString BuildConfiguration_KeyName_AsEBuildConfiguration = TEXT( "Build_Configuration" );
    static inline FString BuildBranchName_KeyName_AsString = TEXT( "Build_BranchName" );
    static inline FString BuildChangelist_KeyName_AsInt = TEXT( "Build_Changelist" );

    static inline FString ConfigIsEditor_KeyName_AsBool = TEXT( "Config_IsEditor" );
    static inline FString ConfigIsBuildMachine_KeyName_AsBool = TEXT( "Config_IsBuildMachine" );
    static inline FString ConfigIsRunningCommandlet_KeyName_AsBool = TEXT( "Config_IsRunningCommandlet" );
    static inline FString ConfigIsDebuggerPresent_KeyName_AsBool = TEXT( "Config_IsDebuggerPresent" );

    static inline FString UserId_KeyName_AsString = TEXT( "User_ID" );
    static inline FString UserLanguage_KeyName_AsString = TEXT( "User_Language" );
    static inline FString UserRegion_KeyName_AsString = TEXT( "User_Region" );
    //static inline FString ApplicationCommandline_KeyName_AsString = TEXT( "Application_Commandline" );

    static inline FString HardwarePlatform_KeyName_AsString = TEXT( "Hardware_Platform" );
    static inline FString HardwareCPU_KeyName_AsString = TEXT( "Hardware_CPU" );
    static inline FString HardwareCPUCoresPhysical_KeyName_AsInt32 = TEXT( "Hardware_CPU_Cores_Physical" );
    static inline FString HardwareCPUCoresLogical_KeyName_AsInt32 = TEXT( "Hardware_CPU_Cores_Logical" );
    static inline FString HardwareRAM_KeyName_AsUint64 = TEXT( "Hardware_RAM" );
    //static inline FString HardwareComputerName_KeyName_AsString = TEXT( "Hardware_ComputerName" );

    static inline FString OSVersion_KeyName_AsString = TEXT( "OS_Version" );
    static inline FString OSVersionLabel_KeyName_AsString = TEXT( "OS_VersionLabel" );
    static inline FString OSVersionSubLabel_KeyName_AsString = TEXT( "OS_VersionSubLabel" );
    static inline FString OSVersionId_KeyName_AsString = TEXT( "OS_ID" );
};

void
FOdysseyTelemetry::StartSession()
{
    checkf( !bIsInitialized, TEXT( "FOdysseyTelemetry::Initialize called more than once." ) );

    // Load the configuration
    LoadConfiguration();

#if WITH_EDITOR
    // this will only be true for builds that have editor support (desktop platforms)
    // The idea here is to only send editor events for actual editor runs, not for things like -game runs of the editor.
    bool bIsEditorRun = GIsEditor && !IsRunningCommandlet();
#else
    bool bIsEditorRun = false;
#endif

    // Outside of the editor, the only engine analytics usage is the hardware survey
    const bool bShouldInitAnalytics = bIsEditorRun && Config.bSendTelemetry;
    if( !bShouldInitAnalytics )
        return;

    OdysseyAnalyticsProvider = CreatePraxinosAnalyticsProvider();
    if( !OdysseyAnalyticsProvider.IsValid() )
        return;

    //---

    TArray<FAnalyticsEventAttribute> DefaultAttributes;
    TArray<FAnalyticsEventAttribute> SessionContextAttributes;

    //const FString UserID = FPlatformProcess::UserName( false );
    const FString UserID = CreateAnalyticsUserId( FPlatformMisc::GetEpicAccountId() );

    //FCultureRef Culture = FInternationalization::Get().GetCurrentCulture(); // Region is empty
    FCultureRef OSCulture = FInternationalization::Get().GetDefaultCulture();
    FString UserName = OSCulture->GetName();                          // fr-FR
    FString UserLanguage = OSCulture->GetTwoLetterISOLanguageName();  // fr
    FString UserRegion = OSCulture->GetRegion();                      // FR

    //FString ComputerName = FPlatformProcess::ComputerName();

    //const FString ProjectName = FApp::GetProjectName();

    FString ProjectIDString;
    GConfig->GetString( TEXT( "/Script/EngineSettings.GeneralProjectSettings" ), TEXT( "ProjectID" ), ProjectIDString, GGameIni );

    FGuid ProjectID;
    if( !ProjectIDString.IsEmpty() )
    {
        TArray<FString> Elements;
        if( ProjectIDString.ParseIntoArray( Elements, TEXT( "=" ) ) == 5 )
        {
            ProjectID = FGuid( FCString::Atoi( *( Elements[1] ) ), FCString::Atoi( *( Elements[2] ) ), FCString::Atoi( *( Elements[3] ) ), FCString::Atoi( *( Elements[4] ) ) );
        }
        else
        {
            ProjectID = FGuid( ProjectIDString );
        }
    }

    FGuid SessionID = FApp::GetInstanceId();

    //---

    using SessionStartFields = SessionStart_TelemetryFields;

    // Build the default attributes. These are sent with ALL events
    DefaultAttributes.Emplace( SessionStartFields::SessionId_KeyName_AsString, SessionID.ToString( EGuidFormats::DigitsWithHyphensInBraces ) );
    DefaultAttributes.Emplace( SessionStartFields::SessionStartUTC_KeyName_AsDouble, FDateTime::UtcNow().ToUnixTimestampDecimal() );

    //

    // Build the session context attributes. These will always be sent with the SessionStart event but can also be appended to default attributes to send with all ALL events.
    //SessionContextAttributes.Emplace( SessionStartFields::ProjectName_KeyName_AsString, ProjectName );
    SessionContextAttributes.Emplace( SessionStartFields::ProjectId_KeyName_AsString, ProjectID );

    SessionContextAttributes.Emplace( SessionStartFields::BuildConfiguration_KeyName_AsEBuildConfiguration, FApp::GetBuildConfiguration() );
    SessionContextAttributes.Emplace( SessionStartFields::BuildBranchName_KeyName_AsString, FApp::GetBranchName().ToLower() );
    SessionContextAttributes.Emplace( SessionStartFields::BuildChangelist_KeyName_AsInt, BuildSettings::GetCurrentChangelist() );

    SessionContextAttributes.Emplace( SessionStartFields::ConfigIsEditor_KeyName_AsBool, GIsEditor );
    SessionContextAttributes.Emplace( SessionStartFields::ConfigIsBuildMachine_KeyName_AsBool, GIsBuildMachine );
    SessionContextAttributes.Emplace( SessionStartFields::ConfigIsRunningCommandlet_KeyName_AsBool, IsRunningCommandlet() );
    SessionContextAttributes.Emplace( SessionStartFields::ConfigIsDebuggerPresent_KeyName_AsBool, FPlatformMisc::IsDebuggerPresent() );

    //// Only send user data if requested
    //if( Config.bSendUserData == true )
    //{
        SessionContextAttributes.Emplace( SessionStartFields::UserId_KeyName_AsString, UserID );
        SessionContextAttributes.Emplace( SessionStartFields::UserLanguage_KeyName_AsString, UserLanguage );
        SessionContextAttributes.Emplace( SessionStartFields::UserRegion_KeyName_AsString, UserRegion );
    //    SessionContextAttributes.Emplace( SessionStartFields::ApplicationCommandline_KeyName_AsString, FCommandLine::Get() );
    //}

    // ALways send the platform
    SessionContextAttributes.Emplace( SessionStartFields::HardwarePlatform_KeyName_AsString, FString( FPlatformProperties::IniPlatformName() ) );

    // Only send detailed hardware data if requested
    //if( Config.bSendHardwareData == true )
    {
        SessionContextAttributes.Emplace( SessionStartFields::HardwareCPU_KeyName_AsString, FPlatformMisc::GetCPUBrand() );
        SessionContextAttributes.Emplace( SessionStartFields::HardwareCPUCoresPhysical_KeyName_AsInt32, FPlatformMisc::NumberOfCores() );
        SessionContextAttributes.Emplace( SessionStartFields::HardwareCPUCoresLogical_KeyName_AsInt32, FPlatformMisc::NumberOfCoresIncludingHyperthreads() );
        SessionContextAttributes.Emplace( SessionStartFields::HardwareRAM_KeyName_AsUint64, FPlatformMemory::GetStats().TotalPhysical );
        //SessionContextAttributes.Emplace( SessionStartFields::HardwareComputerName_KeyName_AsString, ComputerName );
    }

    // Only send OS data if requested
    //if( Config.bSendOSData == true )
    {
        FString OSVersionLabel;
        FString OSSubVersionLabel;
        FPlatformMisc::GetOSVersions( OSVersionLabel, OSSubVersionLabel );

        SessionContextAttributes.Emplace( SessionStartFields::OSVersion_KeyName_AsString, FPlatformMisc::GetOSVersion() );
        SessionContextAttributes.Emplace( SessionStartFields::OSVersionLabel_KeyName_AsString, OSVersionLabel );
        SessionContextAttributes.Emplace( SessionStartFields::OSVersionSubLabel_KeyName_AsString, OSSubVersionLabel );
        SessionContextAttributes.Emplace( SessionStartFields::OSVersionId_KeyName_AsString, FPlatformMisc::GetOperatingSystemId() );
    }

    // Send the Session Context attributes with ALL events by appending to the default attributes if required
    if( Config.bSendSessionContext == true )
    {
        DefaultAttributes.Append( SessionContextAttributes );
    }

    // Set up the analytics provider
    OdysseyAnalyticsProvider->SetUserID( UserID );
    //OdysseyAnalyticsProvider->SetSessionID( SessionID.ToString( EGuidFormats::DigitsWithHyphensInBraces ) ); // Set when using StartSession(...)
    OdysseyAnalyticsProvider->SetDefaultEventAttributes( MoveTemp( DefaultAttributes ) );
    //OdysseyAnalyticsProvider->SetRecordEventCallback( RecordEventCallback );

    //---

    // StartSession() without parameter will regenerate a new SessionId
    // So, use StartSession(...) with the same SessionId as the events
    OdysseyAnalyticsProvider->StartSession( SessionID.ToString( EGuidFormats::DigitsWithHyphensInBraces ), TArray<FAnalyticsEventAttribute>() );

    // Create the IAnalyticsTracer interface
    OdysseyAnalyticsTracer = FAnalytics::Get().CreateAnalyticsTracer();
    OdysseyAnalyticsTracer->SetProvider( OdysseyAnalyticsProvider );
    OdysseyAnalyticsTracer->StartSession();

    // Bind the pre-exit callback
    FCoreDelegates::OnEnginePreExit.AddRaw( &FOdysseyTelemetry::Get(), &FOdysseyTelemetry::EndSession );

    SessionStartTime = FDateTime::UtcNow();

    TArray<FAnalyticsEventAttribute> Attributes;
    if( Config.bSendSessionContext == false )
    {
        // We always send the session context with the SessionStart event so if we have opted out of sending it with the DefaultAttributes to ALL events we need append it here
        Attributes = SessionContextAttributes;
    }

    const int32 SessionContextSchemaVersion = 1;
    Attributes.Emplace( SessionStartFields::SchemaVersion_KeyName_AsInt32, SessionContextSchemaVersion );

    OdysseyAnalyticsProvider->RecordEvent( SessionStartFields::KeyName, Attributes );

    //---

    UE_LOGF( LogOdysseyTelemetry, Log, "Started OdysseyTelemetry Session" );

    bIsInitialized = true;

    //---

    RegisterOnAssetCreation();
}

// MUST END with "_TelemetryFields"
// MUST BE synchronized with web code
struct SessionEnd_TelemetryFields
{
    static inline FString KeyName = TEXT( "OdysseyTelemetry.SessionEnd" );

    // Attributes
    static inline FString SchemaVersion_KeyName_AsInt32 = TEXT( "SchemaVersion" );
    static inline FString Duration_KeyName_AsDouble = TEXT( "Duration" );
};

void
FOdysseyTelemetry::EndSession()
{
    if( !bIsInitialized )
        return;

    bIsInitialized = false;

    // End session for the tracer and the provider
    if( OdysseyAnalyticsTracer.IsValid() )
    {
        OdysseyAnalyticsTracer->EndSession();
        OdysseyAnalyticsTracer.Reset();
    }

    if( OdysseyAnalyticsProvider.IsValid() )
    {
        const FTimespan SessionDuration = FDateTime::UtcNow() - SessionStartTime;

        using SessionEndFields = SessionEnd_TelemetryFields;

        TArray<FAnalyticsEventAttribute> Attributes;
        Attributes.Emplace( SessionEndFields::SchemaVersion_KeyName_AsInt32, 1 );
        Attributes.Emplace( SessionEndFields::Duration_KeyName_AsDouble, SessionDuration.GetTotalSeconds() );
        OdysseyAnalyticsProvider->RecordEvent( SessionEndFields::KeyName, Attributes );

        OdysseyAnalyticsProvider->FlushEvents();
        OdysseyAnalyticsProvider->EndSession();
        ensure( OdysseyAnalyticsProvider.IsUnique() );
        OdysseyAnalyticsProvider.Reset();
    }

    UE_LOGF( LogOdysseyTelemetry, Log, "Ended OdysseyTelemetry Session" );
}

//---

void
FOdysseyTelemetry::LoadConfiguration()
{
    const FString SectionName( "OdysseyTelemetry.Config" );

    FString ConfigPathfile = GConfig->GetConfigFilename( TEXT( UE_PLUGIN_NAME ) );

    GConfig->GetBool( *SectionName, TEXT( "SendTelemetry" ), Config.bSendTelemetry, ConfigPathfile );
    //GConfig->GetBool( *SectionName, TEXT( "SendUserData" ), Config.bSendUserData, ConfigPathfile );
    //GConfig->GetBool( *SectionName, TEXT( "SendHardwareData" ), Config.bSendHardwareData, ConfigPathfile );
    //GConfig->GetBool( *SectionName, TEXT( "SendOSData" ), Config.bSendOSData, ConfigPathfile );
    GConfig->GetBool( *SectionName, TEXT( "SendSessionContext" ), Config.bSendSessionContext, ConfigPathfile );
}

//---

TWeakPtr<IAnalyticsProvider>
FOdysseyTelemetry::GetProvider()
{
    return OdysseyAnalyticsProvider;
}

//---

TWeakPtr<IAnalyticsTracer>
FOdysseyTelemetry::GetTracer()
{
    return OdysseyAnalyticsTracer;
}

TSharedPtr<IAnalyticsSpan>
FOdysseyTelemetry::StartSpan( const FName Name, const TArray<FAnalyticsEventAttribute>& AdditionalAttributes )
{
    return OdysseyAnalyticsTracer.IsValid() ? OdysseyAnalyticsTracer->StartSpan( Name, AdditionalAttributes ) : TSharedPtr<IAnalyticsSpan>();
}

TSharedPtr<IAnalyticsSpan>
FOdysseyTelemetry::StartSpan( const FName Name, TSharedPtr<IAnalyticsSpan> ParentSpan, const TArray<FAnalyticsEventAttribute>& AdditionalAttributes )
{
    return OdysseyAnalyticsTracer.IsValid() ? OdysseyAnalyticsTracer->StartSpan( Name, ParentSpan, AdditionalAttributes ) : TSharedPtr<IAnalyticsSpan>();
}

bool
FOdysseyTelemetry::EndSpan( TSharedPtr<IAnalyticsSpan> Span, const TArray<FAnalyticsEventAttribute>& AdditionalAttributes )
{
    return OdysseyAnalyticsTracer.IsValid() ? OdysseyAnalyticsTracer->EndSpan( Span, AdditionalAttributes ) : false;
}

bool
FOdysseyTelemetry::EndSpan( const FName Name, const TArray<FAnalyticsEventAttribute>& AdditionalAttributes )
{
    return OdysseyAnalyticsTracer.IsValid() ? OdysseyAnalyticsTracer->EndSpan( Name, AdditionalAttributes ) : false;
}

TSharedPtr<IAnalyticsSpan>
FOdysseyTelemetry::GetSpan( const FName Name )
{
    return OdysseyAnalyticsTracer.IsValid() ? OdysseyAnalyticsTracer->GetSpan( Name ) : TSharedPtr<IAnalyticsSpan>();
}

TSharedPtr<IAnalyticsSpan>
FOdysseyTelemetry::GetSessionSpan() const
{
    return OdysseyAnalyticsTracer.IsValid() ? OdysseyAnalyticsTracer->GetSessionSpan() : TSharedPtr<IAnalyticsSpan>();
}

//---

// MUST END with "_TelemetryFields"
// MUST BE synchronized with web code
struct AssetAdded_TelemetryFields
{
    static inline FString KeyName = TEXT( "AssetRegistry.AssetAdded" );

    // Attributes
    static inline FString AssetClassPath_KeyName_AsString = TEXT( "AssetClassPath" );
    static inline FString AssetFullName_KeyName_AsString = TEXT( "AssetFullName" );
};

void
FOdysseyTelemetry::RegisterOnAssetCreation()
{
    FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>( AssetRegistryConstants::ModuleName );

    AssetRegistryModule.Get().OnAssetAdded().AddLambda( [this]( const FAssetData& iAssetData )
                                                        {
                                                            FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>( AssetRegistryConstants::ModuleName );
                                                            // To ignore all assets loading at startup and just care when new assets are created
                                                            if( AssetRegistryModule.Get().IsLoadingAssets() )
                                                                return;

                                                            using AssetAddedFields = AssetAdded_TelemetryFields;

                                                            // Not ideal, but can't use UOdysseyAnimation (or any other custom) classes to avoid dependency problems ...
                                                            // Maybe record events in each custom factory (?)
                                                            if( iAssetData.GetClass()->GetName() == TEXT( "OdysseyAnimation" )
                                                                || iAssetData.GetClass()->GetName() == TEXT( "BoardSequence" )
                                                                || iAssetData.GetClass()->GetName() == TEXT( "ShotSequence" ) )
                                                            //if( iAssetData.GetClass() == UTexture2D::StaticClass() )
                                                            {
                                                                TArray<FAnalyticsEventAttribute> Attributes;
                                                                Attributes.Emplace( AssetAddedFields::AssetClassPath_KeyName_AsString, iAssetData.AssetClassPath.ToString() );
                                                                Attributes.Emplace( AssetAddedFields::AssetFullName_KeyName_AsString, iAssetData.GetFullName() );

                                                                OdysseyAnalyticsProvider->RecordEvent( TEXT( "AssetRegistry.AssetAdded" ), Attributes );
                                                            }
                                                        } );
}
