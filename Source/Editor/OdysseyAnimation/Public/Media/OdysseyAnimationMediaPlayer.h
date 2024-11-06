// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "OdysseyAnimation.h"

#include "Media/OdysseyAnimationMediaCache.h"
#include "Media/OdysseyAnimationMediaControls.h"
#include "Media/OdysseyAnimationMediaSamples.h"
#include "Media/OdysseyAnimationMediaTracks.h"
#include "Media/OdysseyAnimationMediaView.h"

#include "IMediaPlayer.h"
#include "IMediaEventSink.h"

class ODYSSEYANIMATION_API FOdysseyAnimationMediaPlayer
    : public IMediaPlayer
    , public TSharedFromThis<FOdysseyAnimationMediaPlayer>
{
public:
    /** Virtual destructor. */
    virtual ~FOdysseyAnimationMediaPlayer();

    /**
     * Create and initialize a new instance.
     *
     * @param InEventSink The object that receives media events from this player.
     */
    FOdysseyAnimationMediaPlayer(IMediaEventSink& InEventSink);

public:

    /** Initialization after construction */
    void Init();

    /** Returns the currently opened animation or nullptr */
    UOdysseyAnimation* GetAnimation();

    /** Returns the player's event sink */
    IMediaEventSink& GetEventSink();

    IOdysseyImageRenderer::eRenderType GetRenderType() const;
    void SetRenderType(IOdysseyImageRenderer::eRenderType iRenderType);

    void SetFrameToIncludeIntoDuration(int iFrame);
    void UnsetFrameToIncludeIntoDuration();

public:

    //~ IMediaPlayer interface
    virtual bool Open(const FString& Url, const IMediaOptions* Options) override;
    virtual bool Open(const TSharedRef<FArchive, ESPMode::ThreadSafe>& Archive, const FString& OriginalUrl, const IMediaOptions* Options) override;
    virtual void Close() override;

    virtual IMediaCache& GetCache() override;
    virtual IMediaControls& GetControls() override;
    virtual IMediaSamples& GetSamples() override;
    virtual IMediaTracks& GetTracks() override;
    virtual IMediaView& GetView() override;

    virtual FString GetInfo() const override;
    virtual FGuid GetPlayerPluginGUID() const override;
    virtual FString GetStats() const override;
    virtual FString GetUrl() const override;

    //There is several event sinks on which the user can bind to know what is happening with the player (see PlayerFacade)
    //Flush, means that any event in queue in those sinks are deleted
    //It also deletes all the player existing samples (I think)
    //Seeking is like starting over, that's why I think flushing when the seek starts is a good thing
    //as we don't care about the events in queue if we start over again in the next millisecond
    virtual bool FlushOnSeekStarted() const override;
    virtual bool FlushOnSeekCompleted() const override;
    virtual bool GetPlayerFeatureFlag(EFeatureFlag flag) const override;

public:
    TSharedPtr<FOdysseyAnimationMediaControls> GetAnimationControls() const { return mControls; }

private:
    TStrongObjectPtr<UOdysseyAnimation> mAnimation;
    FString mUrl;
    IMediaEventSink& mEventSink;

    TSharedPtr<FOdysseyAnimationMediaCache> mCache;
    TSharedPtr<FOdysseyAnimationMediaControls> mControls;
    TSharedPtr<FOdysseyAnimationMediaSamples> mSamples;
    TSharedPtr<FOdysseyAnimationMediaTracks> mTracks;
    TSharedPtr<FOdysseyAnimationMediaView> mView;
};
