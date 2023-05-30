// Copyright Epic Games, Inc. All Rights Reserved.

#include "Media/OdysseyAnimationMediaPlayer.h"

#include "UObject/UObjectGlobals.h"
#include "IMediaTextureSample.h"
#include "IMediaSamples.h"

#define LOCTEXT_NAMESPACE "OdysseyAnimationMediaPlayer"

FOdysseyAnimationMediaPlayer::~FOdysseyAnimationMediaPlayer()
{
	Close();
}

FOdysseyAnimationMediaPlayer::FOdysseyAnimationMediaPlayer(IMediaEventSink& iEventSink)
	: mEventSink(iEventSink)
	, mCache(MakeShared<FOdysseyAnimationMediaCache>())
	, mControls(MakeShared<FOdysseyAnimationMediaControls>())
	, mSamples(MakeShared<FOdysseyAnimationMediaSamples>())
	, mTracks(MakeShared<FOdysseyAnimationMediaTracks>())
	, mView(MakeShared<FOdysseyAnimationMediaView>())
{
}

void
FOdysseyAnimationMediaPlayer::Init()
{
	mCache->Init();
	mControls->Init(AsShared());
	mSamples->Init(AsShared(), mControls);
	mTracks->Init(AsShared());
	mView->Init(AsShared());
}

UOdysseyAnimation*
FOdysseyAnimationMediaPlayer::GetAnimation()
{
	return mAnimation.Get();
}

IMediaEventSink&
FOdysseyAnimationMediaPlayer::GetEventSink()
{
	return mEventSink;
}

//~ IMediaPlayer interface

bool
FOdysseyAnimationMediaPlayer::Open(const FString& iUrl, const IMediaOptions* iOptions)
{
	Close();

	if ( iUrl.IsEmpty() || !iUrl.StartsWith(TEXT("odysseyanimation://")))
		return false;

	mUrl = iUrl;

	//find the animation asset
	FString assetPath = iUrl;
	assetPath.RemoveFromStart(TEXT("odysseyanimation://"));

	mAnimation = TStrongObjectPtr<UOdysseyAnimation>(LoadObject< UOdysseyAnimation >( nullptr, *assetPath ));
	if (!mAnimation)
	{
		mControls->SetState(EMediaState::Error);
		mEventSink.ReceiveMediaEvent(EMediaEvent::MediaOpenFailed);
		return false;
	}

	mCurrentDuration = mAnimation->GetDuration();
	
	//succeeded
	mCache->OnOpen(mAnimation.Get());
	mControls->OnOpen(mAnimation.Get());
	mSamples->OnOpen(mAnimation.Get());
	mTracks->OnOpen();
	mView->OnOpen();
	
	mEventSink.ReceiveMediaEvent(EMediaEvent::MediaOpened);

	return true;
}

bool
FOdysseyAnimationMediaPlayer::Open(const TSharedRef<FArchive, ESPMode::ThreadSafe>& iArchive, const FString& iOriginalUrl, const IMediaOptions* iOptions)
{
	mControls->SetState(EMediaState::Error);
	return false;
}

void
FOdysseyAnimationMediaPlayer::Close()
{
	mCache->OnClose();
	mControls->OnClose();
	mSamples->OnClose();
	mTracks->OnClose();
	mView->OnClose();

	mAnimation = nullptr;
	mUrl.Empty();

	mEventSink.ReceiveMediaEvent(EMediaEvent::MediaClosed);
}

IMediaCache&
FOdysseyAnimationMediaPlayer::GetCache()
{
	return *mCache;
}

IMediaControls&
FOdysseyAnimationMediaPlayer::GetControls()
{
	return *mControls;
}

IMediaSamples&
FOdysseyAnimationMediaPlayer::GetSamples()
{
	return *mSamples;
}

IMediaTracks&
FOdysseyAnimationMediaPlayer::GetTracks()
{
	return *mTracks;
}

IMediaView&
FOdysseyAnimationMediaPlayer::GetView()
{
	return *mView;
}

FString
FOdysseyAnimationMediaPlayer::GetInfo() const
{
	/* Example
		Info = TEXT("Image Sequence\n");
		Info += FString::Printf(TEXT("    Dimension: %i x %i\n"), SequenceDim.X, SequenceDim.Y);
		Info += FString::Printf(TEXT("    Format: %s\n"), *FirstFrameInfo.FormatName);
		Info += FString::Printf(TEXT("    Compression: %s\n"), *FirstFrameInfo.CompressionName);
		Info += FString::Printf(TEXT("    Frames: %i\n"), GetNumImages());
		Info += FString::Printf(TEXT("    Frame Rate: %.2f (%i/%i)\n"), SequenceFrameRate.AsDecimal(), SequenceFrameRate.Numerator, SequenceFrameRate.Denominator);

		return Info;
	*/

	return FString();
}

FGuid
FOdysseyAnimationMediaPlayer::GetPlayerPluginGUID() const
{
    static FGuid PlayerPluginGUID(0xb3da1256, 0x7366410b, 0xacfdc6e3, 0x54ff7e5d);
	return PlayerPluginGUID;
}

FString
FOdysseyAnimationMediaPlayer::GetStats() const
{
	//From ImgMediaPlayer
	FString StatsString;
	{
		StatsString += TEXT("not implemented yet");
		StatsString += TEXT("\n");
	}

	return StatsString;
}

FString
FOdysseyAnimationMediaPlayer::GetUrl() const
{
	return mUrl;
}

bool
FOdysseyAnimationMediaPlayer::FlushOnSeekStarted() const
{
	//see .h for explanations
	return true;
}

bool
FOdysseyAnimationMediaPlayer::FlushOnSeekCompleted() const
{
	//see .h for explanations
	return false;
}

bool
FOdysseyAnimationMediaPlayer::GetPlayerFeatureFlag(EFeatureFlag iFlag) const
{
	switch (iFlag)
	{
		//PlaybackTimingV2 is the new way to manage Timing
		//Timing is now managed by the Media Framework instead of the player itself
		//V1 is meant to disappear in the future
		case EFeatureFlag::UsePlaybackTimingV2:
		return true;

		default:
		break;
	}

	return IMediaPlayer::GetPlayerFeatureFlag(iFlag);
}

void
FOdysseyAnimationMediaPlayer::Tick(float DeltaTime)
{
	if ( !mAnimation )
		return;

	FTimespan duration = mAnimation->GetDuration();
	if ( duration != mCurrentDuration )
	{
		mCurrentDuration = duration;
	}
}

#undef LOCTEXT_NAMESPACE

