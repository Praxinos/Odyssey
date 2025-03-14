// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Media/OdysseyAnimationMediaControls.h"

#include "Media/OdysseyAnimationMediaPlayer.h"
#include "OdysseyAnimation.h"

FOdysseyAnimationMediaControls::FOdysseyAnimationMediaControls()
    : mAnimation(nullptr)
    , mState(EMediaState::Closed)
    , mPlaybackIsBlocking(false)
    , mIsLooping(false)
    , mRate(0.0f)
{
}

void
FOdysseyAnimationMediaControls::Init(TSharedPtr<FOdysseyAnimationMediaPlayer> iPlayer)
{
    mPlayer = iPlayer;
}

void
FOdysseyAnimationMediaControls::OnOpen( UOdysseyAnimation* iAnimation )
{
    mAnimation = iAnimation;
    mState = EMediaState::Stopped;
    mPlaybackIsBlocking = false;
    mIsLooping = false;
    mRate = 0.f;
}

void
FOdysseyAnimationMediaControls::OnClose()
{
    mAnimation = nullptr;
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
    if (!mAnimation)
        return FTimespan();

    FInt32Range range = GetFrameRange();
    FTimespan duration = FTimespan::FromSeconds((range.GetUpperBoundValue() + 1) / mAnimation->GetFramesPerSecond()) - FTimespan(1);
    return duration;
}

int
FOdysseyAnimationMediaControls::GetFrameCount() const
{
    FInt32Range frameRange = GetFrameRange();
    int startFrame = frameRange.GetUpperBound().IsInclusive() ? frameRange.GetLowerBoundValue() : frameRange.GetLowerBoundValue() + 1;
    int endFrame = frameRange.GetUpperBound().IsInclusive() ? frameRange.GetUpperBoundValue() : frameRange.GetUpperBoundValue() - 1;
    return endFrame - startFrame + 1;
}

FInt32Range
FOdysseyAnimationMediaControls::GetFrameRange() const
{

    FInt32Range frameRange;
    frameRange.SetLowerBoundValue(mAnimation->GetLeftBoundValue());
    frameRange.SetUpperBoundValue(mAnimation->GetRightBoundValue());
    if (mFrameToIncludeIntoDuration.IsSet())
    {
        frameRange.SetLowerBoundValue(FMath::Min(frameRange.GetLowerBoundValue(), mFrameToIncludeIntoDuration.GetValue()));
        frameRange.SetUpperBoundValue(FMath::Max(frameRange.GetUpperBoundValue(), mFrameToIncludeIntoDuration.GetValue()));
    }
    return frameRange;
}

void
FOdysseyAnimationMediaControls::SetFrameToIncludeIntoDuration(int iFrame)
{
    mFrameToIncludeIntoDuration = iFrame;
}

void
FOdysseyAnimationMediaControls::UnsetFrameToIncludeIntoDuration()
{
    mFrameToIncludeIntoDuration = TOptional<int>();
}

float
FOdysseyAnimationMediaControls::GetRate() const
{
    return mRate;
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
    //deprecated: but needed to compile
    return mTime;
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

    mTime = iTime;

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

void
FOdysseyAnimationMediaControls::SetTime(FTimespan iTime)
{
    mTime = iTime;
}

bool
FOdysseyAnimationMediaControls::SetRate(float iRate)
{
    TSharedPtr<FOdysseyAnimationMediaPlayer> player = mPlayer.Pin();
    if ( !player )
        return false;

    if (GetDuration() == FTimespan::Zero())
        return false; // nothing to play

    if (mRate == 0.0f  && iRate != 0.0f) // handle restarting
    {
        mState = EMediaState::Playing;
        player->GetEventSink().ReceiveMediaEvent(EMediaEvent::PlaybackResumed);
    }
    else if ( mRate != 0.0f && iRate == 0.0f) // handle pausing
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
