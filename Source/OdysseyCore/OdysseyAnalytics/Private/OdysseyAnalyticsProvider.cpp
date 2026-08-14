// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyAnalyticsProvider.h"

#include "Analytics.h" // LogAnalytics
//#include "AnalyticsPerfTracker.h" // Private
#include "GenericPlatform/GenericPlatform.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/TimeGuard.h"
#include "PlatformHttp.h"

//---

FOdysseyAnalyticsUserAgentCache::FOdysseyAnalyticsUserAgentCache()
    : CachedUserAgent()
    , CachedAgentVersion( 0 )
{
}

FString
FOdysseyAnalyticsUserAgentCache::GetUserAgent()
{
    if( CachedUserAgent.IsEmpty() || CachedAgentVersion != FPlatformHttp::GetDefaultUserAgentVersion() )
    {
        UpdateUserAgent();
    }

    return CachedUserAgent;
}

void
FOdysseyAnalyticsUserAgentCache::UpdateUserAgent()
{
    static TSet<FString> AllowedProjectComments( GetAllowedProjectComments() );
    static TSet<FString> AllowedPlatformComments( GetAllowedPlatformComments() );

    FDefaultUserAgentBuilder Builder = FPlatformHttp::GetDefaultUserAgentBuilder();
    Builder.SetProjectName( TEXT( "HIDDENPROJECTNAME" ) );
    CachedUserAgent = Builder.BuildUserAgentString( &AllowedProjectComments, &AllowedPlatformComments );
    CachedAgentVersion = Builder.GetAgentVersion();
}

//static
TSet<FString>
FOdysseyAnalyticsUserAgentCache::GetAllowedProjectComments()
{
    TArray<FString> AllowedProjectComments;
    //if( AnalyticsProviderETCvars::CVarDefaultUserAgentCommentsEnabled.GetValueOnAnyThread() )
    //{
    GConfig->GetArray( TEXT( "Analytics" ), TEXT( "AllowedUserAgentProjectComments" ), AllowedProjectComments, GEngineIni );
    //}
    return TSet<FString>( MoveTemp( AllowedProjectComments ) );
}

//static
TSet<FString>
FOdysseyAnalyticsUserAgentCache::GetAllowedPlatformComments()
{
    TArray<FString> AllowedPlatformComments;
    //if( AnalyticsProviderETCvars::CVarDefaultUserAgentCommentsEnabled.GetValueOnAnyThread() )
    //{
    GConfig->GetArray( TEXT( "Analytics" ), TEXT( "AllowedUserAgentPlatformComments" ), AllowedPlatformComments, GEngineIni );
    //}
    return TSet<FString>( MoveTemp( AllowedPlatformComments ) );
}

//static
TSet<FString>
FOdysseyAnalyticsUserAgentCache::ParseCommentSet( const FString& CommentBlob )
{
    TArray<FString> Comments;
    CommentBlob.ParseIntoArray( Comments, TEXT( ";" ) );
    return TSet<FString>( MoveTemp( Comments ) );
}

//---

TSharedPtr<FOdysseyAnalyticsProvider> FOdysseyAnalyticsModule::CreateAnalyticsProvider( const Config& ConfigValues ) const
{
#ifdef DISABLE_ANALYTICS_PROVIDER
    UE_LOGF( LogAnalytics, Warning, "CreateAnalyticsProvider is disabled in this configuration." );
    return NULL;
#else
    // If we didn't have a proper APIKey, return NULL
    if( ConfigValues.APIKeyOdyssey.IsEmpty() )
    {
        UE_LOGF( LogAnalytics, Warning, "CreateAnalyticsProvider config not contain required parameter %ls", *Config::GetKeyNameForAPIKey() );
        return nullptr;
    }
    LLM_SCOPE_BYNAME( TEXT( "OdysseyAnalytics" ) );
    return MakeShared<FOdysseyAnalyticsProvider>( ConfigValues );
#endif
}

//---

/**
 * Perform any initialization.
 */
FOdysseyAnalyticsProvider::FOdysseyAnalyticsProvider( const FOdysseyAnalyticsModule::Config& ConfigValues )
    : bSessionInProgress( false )
    , Config( ConfigValues )
    , FlushIntervalSec( ConfigValues.FlushIntervalSec < 0 ? DefaultFlushIntervalSec : ConfigValues.FlushIntervalSec )
    , bShouldCacheEvents( true )
    , NextEventFlushTime( FPlatformTime::Seconds() + FlushIntervalSec )
    , bInDestructor( false )
    // avoid preallocating space if we are using the legacy protocol.
    , EventCache( ConfigValues.MaximumPayloadSize, ConfigValues.PreallocatedPayloadSize )
{
    if( Config.APIKeyOdyssey.IsEmpty() )
    {
        UE_LOGF( LogAnalytics, Fatal, "OdysseyAnalyticsProvider: APIKey (%ls) cannot be empty!", *Config.APIKeyOdyssey );
    }

    if( Config.APIEndpointOdyssey.IsEmpty() )
    {
        UE_LOGF( LogAnalytics, Fatal, "OdysseyAnalyticsProvider: APIEndpointOdyssey is empty for APIKey (%ls)!", *Config.APIKeyOdyssey );
    }

    if( Config.APIServerOdyssey.IsEmpty() )
    {
        UE_LOGF( LogAnalytics, Fatal, "OdysseyAnalyticsProvider: APIServerOdyssey is empty for APIKey (%ls)!", *Config.APIKeyOdyssey );
        //UE_LOGF( LogAnalytics, Warning, "AnalyticsOdyssey: APIServerOdyssey is empty for APIKey (%ls), creating as a NULL provider!", *Config.APIKeyOdyssey );
    }

    if( Config.AppVersion.IsEmpty() )
    {
        TSharedPtr<IPlugin> plugin = IPluginManager::Get().FindPlugin( UE_PLUGIN_NAME );
        const FPluginDescriptor& pluginDescriptor = plugin->GetDescriptor();
        FString version = pluginDescriptor.VersionName;
        //if( pluginDescriptor.bIsBetaVersion )
        //    version += "-beta";
        Config.AppVersion = version;
    }

    UE_LOGF( LogAnalytics, Verbose, "[%ls] Initializing Odyssey Analytics provider", *Config.APIKeyOdyssey );
}

bool FOdysseyAnalyticsProvider::Tick( float DeltaSeconds )
{
    LLM_SCOPE_BYNAME( TEXT( "OdysseyAnalytics" ) );
    QUICK_SCOPE_CYCLE_COUNTER( STAT_FOdysseyAnalyticsProvider_Tick );

    // hold a lock the entire time here because we're making several calls to the event cache that we need to be consistent when we decide to flush.
    // With more care, we can likely avoid holding this lock the entire time.
    FAnalyticsProviderETEventCache::Lock EventCacheLock( EventCache );

    // Countdown to flush
    const double Now = FPlatformTime::Seconds();

    // Never tick-flush more than one provider in a single frame. There's non-trivial overhead to flushing events.
    // On servers where there may be dozens of provider instances, this will spread out the cost a bit.
    // If caching is disabled, we still want events to be flushed immediately, so we are only guarding the flush calls from tick,
    // any other calls to flush are allowed to happen in the same frame.
    static uint64 LastFrameCounterFlushed = 0;

    const bool bHadFlushesQueued = EventCache.HasFlushesQueued();
    const bool bShouldFlush = bHadFlushesQueued || ( EventCache.CanFlush() && Now >= NextEventFlushTime );

    if( bShouldFlush )
    {
        if( GFrameCounter == LastFrameCounterFlushed && true /* AnalyticsProviderETCvars::PreventMultipleFlushesInOneFrame */ )
        {
            UE_LOGF( LogAnalytics, Verbose, "[%ls] Tried to flush, but another analytics provider has already flushed this frame. Deferring until next frame.", *Config.APIKeyOdyssey );
        }
        else
        {
            // Just flush one payload, even if we may have more than one queued.
            FlushEventsOnce();
            LastFrameCounterFlushed = GFrameCounter;
            // If we aren't flushing up a previous queued payload, then this was a regular interval flush, so we need to reset the timer.
            // try to keep on the same cadence when flushing, since we could miss our window by several frames.
            if( !bHadFlushesQueued && Now >= NextEventFlushTime )
            {
                const double Multiplier = FMath::Floor( ( Now - NextEventFlushTime ) / FlushIntervalSec ) + 1.;
                NextEventFlushTime += Multiplier * FlushIntervalSec;
            }
        }
    }
    return true;
}

FOdysseyAnalyticsProvider::~FOdysseyAnalyticsProvider()
{
    bInDestructor = true;
    EndSession();
}

// MUST END with "_TelemetryFields" (to find all of them easily)
// MUST BE synchronized with web code
struct FProviderSessionStart_TelemetryFields
{
    static inline FString KeyName = TEXT( "OdysseyProvider.SessionStart" );

    // Attributes
    static inline FString Platform_KeyName_String = TEXT( "Platform" );
};

bool FOdysseyAnalyticsProvider::StartSession( FString InSessionID, const TArray<FAnalyticsEventAttribute>& Attributes )
{
    LLM_SCOPE_BYNAME( TEXT( "OdysseyAnalytics" ) );
    UE_LOGF( LogAnalytics, Display, "[%ls] OdysseyAnalyticsProvider::StartSession ( APIServer = %ls%ls. AppVersion = %ls )", *Config.APIKeyOdyssey, *Config.APIServerOdyssey, *Config.APIEndpointOdyssey, *Config.AppVersion );

    // end/flush previous session before staring new one
    if( bSessionInProgress )
    {
        EndSession();
    }
    SessionID = MoveTemp( InSessionID );

    using FProviderSessionStartFields = FProviderSessionStart_TelemetryFields;

    // always ensure we send a few specific attributes on session start.
    TArray<FAnalyticsEventAttribute> AttributesWithPlatform = Attributes;
    AttributesWithPlatform.Emplace( FProviderSessionStartFields::Platform_KeyName_String, FString( FPlatformProperties::IniPlatformName() ) );

    RecordEvent( FProviderSessionStartFields::KeyName, AttributesWithPlatform );

    bSessionInProgress = true;
    return bSessionInProgress;
}

// MUST END with "_TelemetryFields" (to find all of them easily)
// MUST BE synchronized with web code
struct FProviderSessionEnd_TelemetryFields
{
    static inline FString KeyName = TEXT( "OdysseyProvider.SessionEnd" );

    // Attributes
    // Nothing
};

/**
 * End capturing stats and queue the upload
 */
void FOdysseyAnalyticsProvider::EndSession()
{
    LLM_SCOPE_BYNAME( TEXT( "OdysseyAnalytics" ) );
    if( bSessionInProgress )
    {
        using FProviderSessionEndFields = FProviderSessionEnd_TelemetryFields;

        RecordEvent( FProviderSessionEndFields::KeyName, TArray<FAnalyticsEventAttribute>() );

        UE_LOGF( LogAnalytics, Display, "[%ls] OdysseyAnalyticsProvider::EndSession", *Config.APIKeyOdyssey );
    }

    FlushEvents();
    SessionID.Empty();

    bSessionInProgress = false;
}

TSharedRef<IHttpRequest, ESPMode::ThreadSafe> FOdysseyAnalyticsProvider::CreateRequest( EAnalyticsRecordEventMode Mode )
{
    LLM_SCOPE_BYNAME( TEXT( "OdysseyAnalytics" ) );
    if( !ensure( FModuleManager::Get().IsModuleLoaded( "HTTP" ) ) )
    {
        UE_LOGF( LogAnalytics, Display, "[%ls] Odyssey Analytics Provider tried to create a new HTTP request when HTTP was shutdown", *Config.APIKeyOdyssey );
    }

    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
    // TODO add config values for retries, for now, using default
    //TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = HttpRetryManager->CreateRequest( FHttpRetrySystem::FRetryLimitCountSetting(),
    //                                                                                             FHttpRetrySystem::FRetryTimeoutRelativeSecondsSetting(),
    //                                                                                             FHttpRetrySystem::FRetryResponseCodes(),
    //                                                                                             FHttpRetrySystem::FRetryVerbs(),
    //                                                                                             RetryServers,
    //                                                                                             FHttpRetrySystem::FRetryLimitCountSetting(),
    //                                                                                             FHttpRetrySystem::FExponentialBackoffCurve() );
    for( const TPair<FString, FString>& HttpHeader : HttpHeaders )
    {
        HttpRequest->SetHeader( HttpHeader.Key, HttpHeader.Value );
    }

    if( Mode == EAnalyticsRecordEventMode::Immediate )
    {
        HttpRequest->SetOption( HttpRequestOptions::RequestMode, LexToString( EHttpRequestMode::ImmediateRequest ) );
    }

    return HttpRequest;
}

void FOdysseyAnalyticsProvider::FlushEvents()
{
    UE_AUTORTFM_ONCOMMIT( this )
    {
        QUICK_SCOPE_CYCLE_COUNTER( STAT_FOdysseyAnalyticsProvider_FlushEvents );
        // Warn if this takes more than 2 ms
        SCOPE_TIME_GUARD_MS( TEXT( "FOdysseyAnalyticsProvider::FlushEvents" ), 2 );

        // keep flushing until the event cache has cleared its queue.
        while( EventCache.CanFlush() )
        {
            FlushEventsOnce();
        }
    };
}

void FOdysseyAnalyticsProvider::ExecuteRequest( TArray<uint8>& Payload, OUT int32& PayloadSize, OUT int32& EventCount, EAnalyticsRecordEventMode Mode )
{
    EventCount = 0;
    PayloadSize = 0;

    // UrlEncode NOTE: need to concatenate everything
    FString URLPath = Config.APIEndpointOdyssey;
    URLPath += TEXT( "?SessionID=" ) + FPlatformHttp::UrlEncode( SessionID );
    URLPath += TEXT( "&AppID=" ) + FPlatformHttp::UrlEncode( Config.APIKeyOdyssey );
    URLPath += TEXT( "&AppVersion=" ) + FPlatformHttp::UrlEncode( Config.AppVersion );
    URLPath += TEXT( "&UserID=" ) + FPlatformHttp::UrlEncode( UserID );
    PayloadSize = URLPath.Len() + Payload.Num();

    // Recreate the URLPath for logging because we do not want to escape the parameters when logging.
    // We cannot simply UrlEncode the entire Path after logging it because UrlEncode(Params) != UrlEncode(Param1) & UrlEncode(Param2) ...
    UE_LOGF( LogAnalytics, VeryVerbose, "[%ls] OdysseyAnalyticsProvider URL:%ls?SessionID=%ls&AppID=%ls&AppVersion=%ls&UserID=%ls. Payload:%.*s",
             *Config.APIKeyOdyssey,
             *Config.APIEndpointOdyssey,
             *SessionID,
             *Config.APIKeyOdyssey,
             *Config.AppVersion,
             *UserID,
             Payload.Num(), reinterpret_cast<FGenericPlatformTypes::UTF8CHAR*>( Payload.GetData() ) );

    {
        QUICK_SCOPE_CYCLE_COUNTER( STAT_FlushEventsHttpRequest );

        // Create/send Http request for an event
        TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = CreateRequest( Mode );
        HttpRequest->SetHeader( TEXT( "Content-Type" ), TEXT( "application/json; charset=utf-8" ) );
        // Want to avoid putting the project name into the User-Agent, because for some apps (like the editor), the project name is private info.
        // The analytics User-Agent uses the default User-Agent, but with project name removed.
        HttpRequest->SetHeader( TEXT( "User-Agent" ), UserAgentCache.GetUserAgent() );
        HttpRequest->SetURL( Config.APIServerOdyssey / URLPath );
        HttpRequest->SetVerb( TEXT( "POST" ) );
        HttpRequest->SetContent( MoveTemp( Payload ) );

        // Don't set a response callback if we are in our destructor, as the instance will no longer be there to call.
        if( !bInDestructor )
        {
            HttpRequest->OnProcessRequestComplete().BindSP( this, &FOdysseyAnalyticsProvider::EventRequestComplete );
        }

        OnPreAnalyticsEventProcessedDelegate.Broadcast( HttpRequest );

        HttpRequest->ProcessRequest();
    }
}

void FOdysseyAnalyticsProvider::SendImmediately( const FString& EventName, const TArray<FAnalyticsEventAttribute>& Attributes )
{
    LLM_SCOPE_BYNAME( TEXT( "OdysseyAnalytics" ) );

    if( ensure( FModuleManager::Get().IsModuleLoaded( "HTTP" ) ) )
    {
        TArray<uint8> Payload = EventCache.CreateImmediatePayload( EventName, Attributes );

        int32 PayloadSize = 0;
        int32 EventCount = 0;
        ExecuteRequest( Payload, PayloadSize, EventCount, EAnalyticsRecordEventMode::Immediate );
    }
    else
    {
        // If the HTTP module is not loaded yet for some reason, we fallback to the Cached system so we won't lose this event.
        // This should not happen, but better safe than sorry.
        RecordEvent( CopyTemp( EventName ), Attributes, EAnalyticsRecordEventMode::Cached );
    }
}

void FOdysseyAnalyticsProvider::FlushEventsOnce()
{
    // FlushEventsOnce cannot be rolled back, so it must not occur inside an AutoRTFM transaction.
    if( !ensure( !AutoRTFM::IsClosed() ) )
    {
        return;
    }

    LLM_SCOPE_BYNAME( TEXT( "OdysseyAnalytics" ) );
    // Make sure we don't try to flush too many times. When we are not caching events it's possible this can be called when there are no events in the array.
    if( !EventCache.CanFlush() )
    {
        return;
    }

    //ANALYTICS_FLUSH_TRACKING_BEGIN();
    int EventCount = 0;
    int PayloadSize = 0;

    if( ensure( FModuleManager::Get().IsModuleLoaded( "HTTP" ) ) )
    {
        TArray<uint8> Payload = EventCache.FlushCacheUTF8();

        ExecuteRequest( Payload, PayloadSize, EventCount, EAnalyticsRecordEventMode::Cached );
    }
    //ANALYTICS_FLUSH_TRACKING_END( PayloadSize, EventCount );
}

void FOdysseyAnalyticsProvider::SetAppID( FString&& InAppID )
{
    if( Config.APIKeyOdyssey != InAppID )
    {
        // Flush any cached events that would be using the old AppID.
        FlushEvents();
        Config.APIKeyOdyssey = MoveTemp( InAppID );
    }
}

void FOdysseyAnalyticsProvider::SetAppVersion( FString&& InAppVersion )
{
    if( InAppVersion.IsEmpty() )
    {
        TSharedPtr<IPlugin> plugin = IPluginManager::Get().FindPlugin( UE_PLUGIN_NAME );
        const FPluginDescriptor& pluginDescriptor = plugin->GetDescriptor();
        InAppVersion = pluginDescriptor.VersionName;
        //if( pluginDescriptor.bIsBetaVersion )
        //    InAppVersion += "-beta";
    }

    if( Config.AppVersion != InAppVersion )
    {
        UE_LOGF( LogAnalytics, Log, "[%ls] Updating AppVersion to %ls from old value of %ls", *Config.APIKeyOdyssey, *InAppVersion, *Config.AppVersion );
        // Flush any cached events that would be using the old AppVersion.
        FlushEvents();
        Config.AppVersion = MoveTemp( InAppVersion );
    }
}

void FOdysseyAnalyticsProvider::SetUserID( const FString& InUserID )
{
    //// command-line specified user ID overrides all attempts to reset it.
    //if( !FParse::Value( FCommandLine::Get(), TEXT( "ANALYTICSUSERID=" ), UserID, false ) )
    //{
        UE_LOGF( LogAnalytics, Log, "[%ls] SetUserId %ls", *Config.APIKeyOdyssey, *InUserID );
        // Flush any cached events that would be using the old UserID.
        FlushEvents();
        UserID = InUserID;
    //}
    //else if( UserID != InUserID )
    //{
    //    UE_LOGF( LogAnalytics, Log, "[%ls] Overriding SetUserId %ls with cmdline UserId of %ls.", *Config.APIKeyOdyssey, *InUserID, *UserID );
    //}
}

FString FOdysseyAnalyticsProvider::GetUserID() const
{
    return UserID;
}

FString FOdysseyAnalyticsProvider::GetSessionID() const
{
    return SessionID;
}

bool FOdysseyAnalyticsProvider::SetSessionID( const FString& InSessionID )
{
    if( SessionID != InSessionID )
    {
        // Flush any cached events that would be using the old SessionID.
        FlushEvents();
        SessionID = InSessionID;
        UE_LOGF( LogAnalytics, Log, "[%ls] Forcing SessionID to %ls.", *Config.APIKeyOdyssey, *SessionID );
    }
    return true;
}

bool FOdysseyAnalyticsProvider::ShouldRecordEvent( const FString& EventName ) const
{
    return !IsActingAsNullProvider() && ( !ShouldRecordEventFunc || ShouldRecordEventFunc( *this, EventName ) );
}

void FOdysseyAnalyticsProvider::RecordEvent( FString&& EventName, const TArray<FAnalyticsEventAttribute>& Attributes )
{
    LLM_SCOPE_BYNAME( TEXT( "OdysseyAnalytics" ) );

    RecordEvent( MoveTemp( EventName ), Attributes, EAnalyticsRecordEventMode::Cached );
}

void FOdysseyAnalyticsProvider::RecordEvent( FString&& EventName, const TArray<FAnalyticsEventAttribute>& Attributes, EAnalyticsRecordEventMode Mode )
{
    // let higher level code filter the decision of whether to send the event
    if( ShouldRecordEvent( EventName ) )
    {
        bool bQueueEvent = true;
        const FAnalyticEventQueuedInfo AnalyticEventInfo =
        {
            Mode,
            EventName,
            Attributes
        };

        OnAnalyticsEventQueuedDelegate.Broadcast( bQueueEvent, AnalyticEventInfo );

        if( bQueueEvent )
        {
            switch( Mode )
            {
                case EAnalyticsRecordEventMode::Cached:
                {
                    // fire any callbacks
                    for( const auto& Cb : EventRecordedCallbacks )
                    {
                        // we no longer track if the event was Json, each attribute does.
                        Cb( EventName, Attributes, false );
                    }

                    EventCache.AddToCache( MoveTemp( EventName ), Attributes );

                    // if we aren't caching events, flush immediately. This is really only for debugging as it will significantly affect bandwidth.
                    if( !bShouldCacheEvents )
                    {
                        FlushEvents();
                    }
                }
                break;

                case EAnalyticsRecordEventMode::Immediate:
                {
                    SendImmediately( MoveTemp( EventName ), Attributes );
                }
                break;

                default:
                    checkf( false, TEXT( "Please, implement new modes here" ) );
                    break;
            }
        }
        else
        {
            UE_LOGF( LogAnalytics, Verbose, "Dropping Event %ls due to OnAnalyticsEventQueuedDelegate request.", *EventName );
        }
    }
    else
    {
        UE_LOGF( LogAnalytics, Verbose, "Ignoring event named '%ls' due to ShouldRecordEvent check", *EventName );
    }
}

void FOdysseyAnalyticsProvider::SetDefaultEventAttributes( TArray<FAnalyticsEventAttribute>&& Attributes )
{
    //FAnalyticsEventAttribute* attribute = Attributes.FindByPredicate( []( const FAnalyticsEventAttribute& iAttribute )
    //                                                                  {
    //                                                                      return iAttribute.GetName() == TEXT( "ProjectName" );
    //                                                                  } );
    //if( attribute )
    //    attribute->SetValue( TEXT( "*****" ) );

    EventCache.SetDefaultAttributes( MoveTemp( Attributes ) );
}

TArray<FAnalyticsEventAttribute> FOdysseyAnalyticsProvider::GetDefaultEventAttributesSafe() const
{
    return EventCache.GetDefaultAttributes();
}

int32 FOdysseyAnalyticsProvider::GetDefaultEventAttributeCount() const
{
    return EventCache.GetDefaultAttributeCount();
}


FAnalyticsEventAttribute FOdysseyAnalyticsProvider::GetDefaultEventAttribute( int AttributeIndex ) const
{
    return EventCache.GetDefaultAttribute( AttributeIndex );
}

void FOdysseyAnalyticsProvider::SetEventCallback( const OnEventRecorded& Callback )
{
    EventRecordedCallbacks.Add( Callback );
}

void FOdysseyAnalyticsProvider::EventRequestComplete( FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool )
{
    // process responses
    bool bEventsDelivered = false;
    if( HttpResponse.IsValid() )
    {
        UE_LOGF( LogAnalytics, VeryVerbose, "[%ls] OdysseyAnalyticsProvider response for [%ls]. Code: %d. Payload: %ls", *Config.APIKeyOdyssey, *HttpRequest->GetURL(), HttpResponse->GetResponseCode(), *HttpResponse->GetContentAsString() );
        if( EHttpResponseCodes::IsOk( HttpResponse->GetResponseCode() ) )
        {
            bEventsDelivered = true;
        }
    }
    else
    {
        UE_LOGF( LogAnalytics, VeryVerbose, "[%ls] OdysseyAnalyticsProvider response for [%ls]. No response", *Config.APIKeyOdyssey, *HttpRequest->GetURL() );
    }
}

void FOdysseyAnalyticsProvider::SetUrlDomain( const FString& Domain, const TArray<FString>& AltDomains )
{
    // See if anything is actually changing before going through the work to flush and reset the URLs.
    if( Config.APIServerOdyssey == Domain )
    {
        return;
    }
    LLM_SCOPE_BYNAME( TEXT( "OdysseyAnalytics" ) );

    // flush existing events before changing URL domains.
    FlushEvents();

    Config.APIServerOdyssey = Domain;

    //// Set the number of retries to the number of retry URLs that have been passed in.
    //uint32 RetryLimitCount = AltDomains.Num();

    //HttpRetryManager->SetDefaultRetryLimit( RetryLimitCount );

    //TArray<FString> TmpAltAPIServers = AltDomains;

    //// If we have retry domains defined, insert the default domain into the list
    //if( RetryLimitCount > 0 )
    //{
    //    FString DefaultUrlDomain = FPlatformHttp::GetUrlDomain( Config.APIServerET );
    //    if( !TmpAltAPIServers.Contains( DefaultUrlDomain ) )
    //    {
    //        TmpAltAPIServers.Insert( DefaultUrlDomain, 0 );
    //    }

    //    RetryServers = MakeShared<FHttpRetrySystem::FRetryDomains, ESPMode::ThreadSafe>( MoveTemp( TmpAltAPIServers ) );
    //}
    //else
    //{
    //    RetryServers.Reset();
    //}

    if( Config.APIServerOdyssey.IsEmpty() )
    {
        UE_LOGF( LogAnalytics, Fatal, "OdysseyAnalyticsProvider: APIServerOdyssey is empty for APIKey (%ls)!", *Config.APIKeyOdyssey );
        //UE_LOGF( LogAnalytics, Warning, "OdysseyAnalyticsProvider: APIServerOdyssey is empty for APIKey (%ls), converting to a NULL provider!", *Config.APIKeyOdyssey );
    }
    else
    {
        UE_LOGF( LogAnalytics, Log, "OdysseyAnalyticsProvider: Set APIServerOdyssey to %ls", *Config.APIServerOdyssey );
    }
}

void FOdysseyAnalyticsProvider::SetUrlPath( const FString& Path )
{
    // See if anything is actually changing before going through the work to flush and reset the URLs.
    if( Config.APIEndpointOdyssey == Path )
    {
        return;
    }
    LLM_SCOPE_BYNAME( TEXT( "OdysseyAnalytics" ) );

    // flush existing events before changing URL path.
    FlushEvents();

    Config.APIEndpointOdyssey = Path;
}

void FOdysseyAnalyticsProvider::SetHeader( const FString& HeaderName, const FString& HeaderValue )
{
    LLM_SCOPE_BYNAME( TEXT( "OdysseyAnalytics" ) );
    if( HeaderValue.IsEmpty() )
    {
        HttpHeaders.Remove( HeaderName );
    }
    else
    {
        HttpHeaders.Emplace( HeaderName, HeaderValue );
    }
}

void FOdysseyAnalyticsProvider::BlockUntilFlushed( float InTimeoutSec )
{
    LLM_SCOPE_BYNAME( TEXT( "OdysseyAnalytics" ) );
    FlushEvents();
    //HttpRetryManager->BlockUntilFlushed( InTimeoutSec );

    checkNoEntry(); //TODO: to see if it go here ?! to know what to do with HttpRetryManager->BlockUntilFlushed() as HttpRetryManager is not more used
}

void FOdysseyAnalyticsProvider::SetShouldRecordEventFunc( const ShouldRecordEventFunction& InShouldRecordEventFunc )
{
    ShouldRecordEventFunc = InShouldRecordEventFunc;
}
