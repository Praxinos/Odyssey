// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"

#include "AnalyticsProviderETEventCache.h"
#include "IAnalyticsProviderET.h"
#include "Interfaces/IHttpResponse.h"
#include "Interfaces/IHttpRequest.h"

#include "OdysseyAnalyticsModule.h"

// Want to avoid putting the project name into the User-Agent, because for some apps (like the editor), the project name is private info.
// The analytics User-Agent uses the default User-Agent, but with project name removed.
class FOdysseyAnalyticsUserAgentCache
{
public:
    FOdysseyAnalyticsUserAgentCache();

    FString GetUserAgent();

private:
    void UpdateUserAgent();

    static TSet<FString> GetAllowedProjectComments();
    static TSet<FString> GetAllowedPlatformComments();

    static TSet<FString> ParseCommentSet( const FString& CommentBlob );

private:
    FString CachedUserAgent;
    uint32 CachedAgentVersion;
};

/**
 * Implementation of analytics for Odyssey Telemetry. (Greatly inspired by FAnalyticsProviderET)
 * Supports caching events and flushing them periodically (currently hardcoded limits).
 * Also supports a set of default attributes that will be added to every event.
 * For efficiency, this set of attributes is added directly into the set of cached events
 * with a special flag to indicate its purpose. This allows the set of cached events to be used like
 * a set of commands to be executed on flush, and allows us to inject the default attributes
 * efficiently into many events without copying the array at all.
 * If Config.APIServerOdyssey is empty, this will act as a NULL provider by forcing ShouldRecordEvent() to return false all the time.
 */
class FOdysseyAnalyticsProvider:
    public IAnalyticsProviderET,
    public FTSTickerObjectBase,
    public TSharedFromThis<FOdysseyAnalyticsProvider>
{
public:
    FOdysseyAnalyticsProvider( const FOdysseyAnalyticsModule::Config& ConfigValues );

    virtual ~FOdysseyAnalyticsProvider();

public:
    // FTSTickerObjectBase

    bool Tick( float DeltaSeconds ) override;

public:
    using IAnalyticsProvider::StartSession;
    using IAnalyticsProvider::RecordEvent;

    // IAnalyticsProvider

    virtual bool StartSession( FString InSessionID, const TArray<FAnalyticsEventAttribute>& Attributes ) override;
    virtual void EndSession() override;
    virtual void FlushEvents() override;

    virtual void SetAppID( FString&& AppID ) override;
    virtual void SetAppVersion( FString&& AppVersion ) override;
    virtual void SetAppVersionEngine( FString&& AppVersionEngine );

    virtual void SetUserID( const FString& InUserID ) override;
    virtual FString GetUserID() const override;

    virtual FString GetSessionID() const override;
    virtual bool SetSessionID( const FString& InSessionID ) override;

    virtual bool ShouldRecordEvent( const FString& EventName ) const override;
    virtual void RecordEvent( FString&& EventName, const TArray<FAnalyticsEventAttribute>& Attributes ) override;
    virtual void RecordEvent( FString&& EventName, const TArray<FAnalyticsEventAttribute>& Attributes, EAnalyticsRecordEventMode Mode ) override;

    virtual void SetDefaultEventAttributes( TArray<FAnalyticsEventAttribute>&& Attributes ) override;
    virtual TArray<FAnalyticsEventAttribute> GetDefaultEventAttributesSafe() const override;
    virtual int32 GetDefaultEventAttributeCount() const override;
    virtual FAnalyticsEventAttribute GetDefaultEventAttribute( int AttributeIndex ) const override;

    virtual void SetEventCallback( const OnEventRecorded& Callback ) override;

    virtual void SetUrlDomain( const FString& Domain, const TArray<FString>& AltDomains ) override;
    virtual void SetUrlPath( const FString& Path ) override;
    virtual void SetHeader( const FString& HeaderName, const FString& HeaderValue ) override;
    virtual void BlockUntilFlushed( float InTimeoutSec ) override;
    virtual void SetShouldRecordEventFunc( const ShouldRecordEventFunction& InShouldRecordEventFunc ) override;
    virtual FOnPreAnalyticsEventProcessed& OnPreAnalyticsEventProcessed() override
    {
        return OnPreAnalyticsEventProcessedDelegate;
    }
    virtual FOnAnalyticsEventQueued& OnAnalyticsEventQueued() override
    {
        return OnAnalyticsEventQueuedDelegate;
    }

    virtual const FAnalyticsET::Config& GetConfig() const override
    {
        checkNoEntry();
        static FAnalyticsET::Config c;
        return c;
    }
    //virtual const FOdysseyAnalyticsModule::Config& GetConfig() const override
    //{
    //    return Config;
    //}

private:
    void ExecuteRequest( TArray<uint8>& Payload, OUT int32& PayloadSize, OUT int32& EventCount, EAnalyticsRecordEventMode Mode );
    void SendImmediately( const FString& EventName, const TArray<FAnalyticsEventAttribute>& Attributes );
    void FlushEventsOnce();
    void FlushEventLegacy( const FString& EventName, const TArray<FAnalyticsEventAttribute>& Attributes );
    bool IsActingAsNullProvider() const
    {
        // if we don't have a primary APIKey then we are essentially acting as a NULL provider and will suppress all events.
        // Don't bother checking the retry domains because the primary domain being empty is enough to tell us we have nowhere to send as a primary destination.
        return Config.APIServerOdyssey.IsEmpty();
    }

    /** Create a request */
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> CreateRequest( EAnalyticsRecordEventMode Mode = EAnalyticsRecordEventMode::Cached );

    bool bSessionInProgress;
    /** The current configuration (might be updated with respect to the one provided at construction). */
    FOdysseyAnalyticsModule::Config Config;
    /** the unique UserID as passed to ET. */
    FString UserID;
    /** The session ID */
    FString SessionID;
    /** Default flush interval, when one is not explicitly given. */
    const float DefaultFlushIntervalSec = 60.0f;
    /** interval which to ensure events are flushed to the server. An event should not sit in the cache longer than this. It may be flushed sooner, but not longer (unless there is a hitch) */
    float FlushIntervalSec;
    /** Allows events to not be cached when -AnalyticsDisableCaching is used. This should only be used for debugging as caching significantly reduces bandwidth overhead per event. */
    bool bShouldCacheEvents;
    /** Current timer to keep track of FlushIntervalSec flushes */
    double NextEventFlushTime;
    /** Track destructing for unbinding callbacks when firing events at shutdown */
    bool bInDestructor;

    FAnalyticsProviderETEventCache EventCache;

    TArray<OnEventRecorded> EventRecordedCallbacks;

    /** Event filter function */
    ShouldRecordEventFunction ShouldRecordEventFunc;

    /**
    * Delegate called when an event Http request completes
    */
    void EventRequestComplete( FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded );

    /** Http headers to add to requests */
    TMap<FString, FString> HttpHeaders;

    FOdysseyAnalyticsUserAgentCache UserAgentCache;

    FOnPreAnalyticsEventProcessed OnPreAnalyticsEventProcessedDelegate;
    FOnAnalyticsEventQueued OnAnalyticsEventQueuedDelegate;
};
