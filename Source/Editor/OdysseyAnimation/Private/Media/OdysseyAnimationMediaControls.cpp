// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Media/OdysseyAnimationMediaControls.h"

#include "Media/OdysseyAnimationMediaPlayer.h"

#define LOCTEXT_NAMESPACE "OdysseyAnimationMediaControls"

FOdysseyAnimationMediaControls::FOdysseyAnimationMediaControls()
	: mState(EMediaState::Closed)
	, mPlaybackIsBlocking(false)
	, mIsLooping(false)
	, mRate(0.0f)
{
}

void
FOdysseyAnimationMediaControls::Init(TWeakPtr<FOdysseyAnimationMediaPlayer> iPlayer)
{
    mPlayer = iPlayer;
}

void
FOdysseyAnimationMediaControls::OnOpen()
{
    mState = EMediaState::Stopped;
}

void
FOdysseyAnimationMediaControls::OnClose()
{
	mState = EMediaState::Closed;
	mPlaybackIsBlocking = false;
	mIsLooping = false;
	mRate = 0.f;
}

bool
FOdysseyAnimationMediaControls::CanControl(EMediaControl iControl) const
{
	return true; //All controls available
}

FTimespan
FOdysseyAnimationMediaControls::GetDuration() const
{
	TSharedPtr<FOdysseyAnimationMediaPlayer> player = mPlayer.Pin();
	if ( !player )
		return FTimespan(0);

	UOdysseyAnimation* animation = player->GetAnimation();
	if ( !animation )
		return FTimespan(0);

    return animation->GetDuration();
}

float
FOdysseyAnimationMediaControls::GetRate() const
{
	return 1.0;
}

EMediaState
FOdysseyAnimationMediaControls::GetState() const
{
	return mState;
}

EMediaStatus
FOdysseyAnimationMediaControls::GetStatus() const
{
	return EMediaStatus::None;
}

TRangeSet<float>
FOdysseyAnimationMediaControls::GetSupportedRates(EMediaRateThinning iThinning) const
{
	TRangeSet<float> result;
	result.Add(TRange<float>::Inclusive(-100000.0f, 100000.0f));
	return result;
}

FTimespan
FOdysseyAnimationMediaControls::GetTime() const
{
	return FTimespan(0);
}

bool
FOdysseyAnimationMediaControls::IsLooping() const
{
	return mIsLooping;
}

bool
FOdysseyAnimationMediaControls::Seek(const FTimespan& iTime)
{
	TSharedPtr<FOdysseyAnimationMediaPlayer> player = mPlayer.Pin();
	if ( !player )
		return false;

	player->GetOdysseySamples().SetTime(iTime);

	// scrub to desired time if needed
	if (mState == EMediaState::Stopped)
		mState = EMediaState::Paused;

	player->GetEventSink().ReceiveMediaEvent(EMediaEvent::SeekCompleted);

	return true;
}

bool
FOdysseyAnimationMediaControls::SetLooping(bool iLooping)
{
	mIsLooping = iLooping;
	return true;
}

bool
FOdysseyAnimationMediaControls::SetRate(float iRate)
{
	TSharedPtr<FOdysseyAnimationMediaPlayer> player = mPlayer.Pin();
	if ( !player )
		return false;

	if (GetDuration() == FTimespan::Zero())
		return false; // nothing to play

	if (mRate == 0.0f) // handle restarting
	{
		mState = EMediaState::Playing;
		player->GetEventSink().ReceiveMediaEvent(EMediaEvent::PlaybackResumed);
	}
	else if (iRate == 0.0f) // handle pausing
	{
		mState = EMediaState::Paused;
		player->GetEventSink().ReceiveMediaEvent(EMediaEvent::PlaybackSuspended);
	}

	mRate = iRate;

	return true;
}

void
FOdysseyAnimationMediaControls::SetBlockingPlaybackHint(bool iFacadeWillUseBlockingPlayback)
{
	mPlaybackIsBlocking = iFacadeWillUseBlockingPlayback;
}

void
FOdysseyAnimationMediaControls::SetState(EMediaState iState)
{
	mState = iState;
}

#undef LOCTEXT_NAMESPACE