// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Media/OdysseyAnimationMediaSamples.h"

#include "Media/OdysseyAnimationMediaPlayer.h"
#include "Media/OdysseyAnimationMediaTextureSample.h"
#include "Engine/TextureRenderTarget2D.h"
#include "UObject/StrongObjectPtr.h"
#include "ULISLoaderModule.h"

FOdysseyAnimationMediaSamples::FOdysseyAnimationMediaSamples()
    : mAnimation(nullptr)
    , mCurrentFrameIndex(INDEX_NONE)
    , mImageRenderingComposition()
    , mRenderTarget()
{
}

void
FOdysseyAnimationMediaSamples::Init(TSharedPtr<FOdysseyAnimationMediaPlayer> iPlayer, TSharedPtr<FOdysseyAnimationMediaControls> iControls)
{
    mPlayer = iPlayer;
    mControls = iControls;
}

void
FOdysseyAnimationMediaSamples::OnOpen(UOdysseyAnimation* iAnimation)
{
    mAnimation = iAnimation;
    FOdysseyRenderingAbility::OnRenderingChangedDelegate().AddRaw(this, &FOdysseyAnimationMediaSamples::OnRenderingChanged);
    //IOdysseyAnimationImageRenderingAbility::OnCompositionChanged().AddRaw(this, &FOdysseyAnimationMediaSamples::OnRenderingCompositionChanged);
    mRenderTarget = TStrongObjectPtr<UTextureRenderTarget2D>(NewObject<UTextureRenderTarget2D>());
    mRenderTarget->InitAutoFormat(mAnimation->GetWidth(), mAnimation->GetHeight());

    mSample = MakeShared<FOdysseyAnimationMediaTextureSample>(mAnimation->GetWidth(), mAnimation->GetHeight(), mRenderTarget.Get());
    mImageRenderingComposition.Empty();

    mInvalidTileMap = FOdysseyInvalidTileMap(64, mAnimation->GetWidth(), mAnimation->GetHeight());
}

EOdysseyRenderingType
FOdysseyAnimationMediaSamples::GetRenderType() const
{
    return mRenderType;
}

void
FOdysseyAnimationMediaSamples::SetRenderType(EOdysseyRenderingType iRenderType)
{
    if (!mAnimation)
        return;

    mRenderType = iRenderType;

    TArray<FGuid> imageRenderingComposition = mAnimation->GetRenderingComposition(mRenderType, mCurrentFrameIndex);
    if ( imageRenderingComposition == mImageRenderingComposition )
        return;

    mInvalidTileMap.Invalidate();
    mImageRenderingComposition = imageRenderingComposition;
}

void
FOdysseyAnimationMediaSamples::OnClose()
{
    SetRenderType(EOdysseyRenderingType::Render);
    Render();
    FOdysseyRenderingAbility::OnRenderingChangedDelegate().RemoveAll(this);

    mInvalidTileMap.Clear();

    mAnimation = nullptr;
    mSample = nullptr;
    mRenderTarget = nullptr;

    mImageRenderingComposition.Empty();
}

void
FOdysseyAnimationMediaSamples::FlushSamples()
{
    //ES:we should remove all the samples from the sample queue here
    //But we don't have a sample queue, so we do nothing
}

int
FOdysseyAnimationMediaSamples::GetFrameIndexAtTime(FTimespan iTime) const
{
    //Add 1 tick to be sure to retrieve the right frame in case the frame starts between iTime and iTime + 1 tick
    FTimespan time = iTime + FTimespan(1);
    return int(time.GetTotalSeconds() * mAnimation->GetFramesPerSecond());
}

FTimespan
FOdysseyAnimationMediaSamples::FindMaxOverlapingFrame(FTimespan iStartTime, FTimespan iEndTime, int* oIndex)
{
    if (!mAnimation)
        return FTimespan();

    int startFrameIndex = GetFrameIndexAtTime(iStartTime);
    int endFrameIndex = GetFrameIndexAtTime(iEndTime);
    FTimespan bestOverlap(-1);
    int bestFrameIndex = INDEX_NONE;
    for (int frameIndex = startFrameIndex; frameIndex <= endFrameIndex; frameIndex++)
    {
        //Compute Overlap for frameIndex
        TRange<FTimespan> range = mAnimation->GetFrameTimeRange(frameIndex);
        FTimespan frameStartTime = range.GetLowerBoundValue();
        FTimespan frameEndTime = range.GetUpperBoundValue();
        FTimespan overlap = FMath::Min(frameEndTime, iEndTime) - FMath::Max(frameStartTime, iStartTime);

        if(overlap > bestOverlap)
        {
            bestOverlap = overlap;
            bestFrameIndex = frameIndex;
        }
    }

    *oIndex = bestFrameIndex;
    return bestOverlap;
}

bool
FOdysseyAnimationMediaSamples::SanitizeTimeRange(TRange<FMediaTimeStamp>* oTimeRange, bool bReverse)
{
    if (!mAnimation)
        return false;

    TRange<FMediaTimeStamp>& timeRange = *oTimeRange;
    TSharedPtr<FOdysseyAnimationMediaControls> controls = mControls.Pin();
    if ( !controls )
        return false;

    //Even if the media player does some kind of looping, we need to ensure the timerange bounds are looping correctly and are valid
    bool isLowerOutOfBound = timeRange.HasLowerBound() && timeRange.GetLowerBoundValue().Time < 0.0f;
    if (isLowerOutOfBound && controls->IsLooping())
    {
        FMediaTimeStamp timestamp = timeRange.GetLowerBoundValue();
        timestamp.Time += controls->GetDuration();
        timestamp.SetSequenceIndex( timestamp.GetSequenceIndex( ) - 1 );
        timeRange.SetLowerBoundValue(timestamp);
        isLowerOutOfBound = false;
    }

    bool isUpperOutOfBound = timeRange.HasUpperBound() && timeRange.GetUpperBoundValue().Time >= controls->GetDuration();
    if (isUpperOutOfBound && controls->IsLooping())
    {
        FMediaTimeStamp timestamp = timeRange.GetUpperBoundValue();
        timestamp.Time -= controls->GetDuration();
        timestamp.SetSequenceIndex( timestamp.GetSequenceIndex() + 1 );
        timeRange.SetUpperBoundValue(timestamp);
        isUpperOutOfBound = false;
    }

    //Clamp timerange just for security

    FMediaTimeStamp lowerBoundTimestamp = timeRange.GetLowerBoundValue();
    FMediaTimeStamp upperBoundTimestamp = timeRange.GetUpperBoundValue();
    lowerBoundTimestamp.Time = FMath::Max(FTimespan(0), lowerBoundTimestamp.Time);
    upperBoundTimestamp.Time = FMath::Min(controls->GetDuration(), upperBoundTimestamp.Time);
    timeRange.SetLowerBoundValue(lowerBoundTimestamp);
    timeRange.SetUpperBoundValue(upperBoundTimestamp);

    //End is different if we are playing forward or backward
    bool isAtEnd = controls->GetRate() >= 0 ? isUpperOutOfBound : isLowerOutOfBound;

    return isAtEnd;
}

IMediaSamples::EFetchBestSampleResult
FOdysseyAnimationMediaSamples::FetchBestVideoSampleForTimeRange(const TRange<FMediaTimeStamp>& iTimeRange, TSharedPtr<IMediaTextureSample, ESPMode::ThreadSafe>& OutSample, bool bReverse, bool bConsistentResult)
{
    if (!mAnimation)
        return EFetchBestSampleResult::NoSample;

    //    iTimeRange is always goes forward, never backward
    //  which means LowerBoundValue is always <= UpperBoundValue

    TSharedPtr<FOdysseyAnimationMediaPlayer> player = mPlayer.Pin();
    if ( !player )
        return EFetchBestSampleResult::NoSample;

    TSharedPtr<FOdysseyAnimationMediaControls> controls = mControls.Pin();
    if (!controls)
        return EFetchBestSampleResult::NoSample;

    if ( controls->GetState() == EMediaState::Stopped )
        return EFetchBestSampleResult::NoSample;

    //Sanitize the timeRange, to ensure looping and clamp it to animation duration
    //also detects if we reached the end of the animation
    TRange<FMediaTimeStamp> timeRange = controls->GetState() == EMediaState::Paused ? TRange<FMediaTimeStamp>(iTimeRange.GetLowerBoundValue(), iTimeRange.GetLowerBoundValue()) : iTimeRange;
    bool isAtEnd = SanitizeTimeRange(&timeRange, bReverse);

    //Once time range is sanitized
    //Find which frame overlaps the timerange the most
    FTimespan startTime = timeRange.GetLowerBoundValue().Time;
    FTimespan endTime = timeRange.GetUpperBoundValue().Time;
    int startFrameIndex = FMath::Clamp(GetFrameIndexAtTime(startTime), 0, controls->GetFrameCount());
    int endFrameIndex = FMath::Clamp(GetFrameIndexAtTime(endTime), 0, controls->GetFrameCount());
    int64 startSequenceIndex = timeRange.GetLowerBoundValue().GetSequenceIndex();
    int64 endSequenceIndex = timeRange.GetUpperBoundValue().GetSequenceIndex();

    //Check if range is valid
    if ( startSequenceIndex > endSequenceIndex )
        return EFetchBestSampleResult::NoSample;

    if (startSequenceIndex == endSequenceIndex && startTime > endTime)
        return EFetchBestSampleResult::NoSample;

    //range is valid
    //check overlap of each frame with the time range

    int64 resultingSequenceIndex = startSequenceIndex;
    int frameIndex = 0;
    //Only a single frame overlaps the range
    if (startSequenceIndex == endSequenceIndex && startFrameIndex == endFrameIndex)
    {
        frameIndex = startFrameIndex;
    }
    //The time range is not looping, so we have a single range to check
    else if (startSequenceIndex == endSequenceIndex)
    {
        //search in [startFrame, endFrame]
        frameIndex = INDEX_NONE;
        FindMaxOverlapingFrame(startTime, endTime, &frameIndex);
    }
    //The time range is looping enough to cover the whole animation duration,
    //so check a single time range covering the whole animation duration.
    else if ( startTime <= endTime || endSequenceIndex - startSequenceIndex >= 2 )
    {
        frameIndex = INDEX_NONE;
        FindMaxOverlapingFrame(FTimespan(0), controls->GetDuration(), &frameIndex);
    }
    else
    {
        //The time range is looping, but not enough to cover the whole animation duration
        //so we have to check two ranges
        //search in [0, endFrame]
        //search in [startFrame, animation->lastFrame]
        int frameIndex1 = INDEX_NONE;
        int frameIndex2 = INDEX_NONE;
        FTimespan overlap1 = FindMaxOverlapingFrame(0, endTime, &frameIndex1);
        FTimespan overlap2 = FindMaxOverlapingFrame(startTime, controls->GetDuration(), &frameIndex2);
        frameIndex = overlap1 > overlap2 ? frameIndex1 : frameIndex2;
        resultingSequenceIndex = overlap1 > overlap2 ? endSequenceIndex : startSequenceIndex;
    }

    Update(frameIndex, resultingSequenceIndex);
    OutSample = mSample;
    controls->SetTime(mAnimation->GetFrameTimeRange(frameIndex).GetLowerBoundValue());

    //It can sound weird, but this is also the place where we detect that the play needs to stop
    if (isAtEnd)
    {
        // Stop the player.
        //So in the future, translate all this system in our own system with tracks, animated textures, players and everything
        player->GetEventSink().ReceiveMediaEvent(EMediaEvent::PlaybackEndReached);
        controls->Pause();
        controls->SetState(EMediaState::Stopped);
    }

    return EFetchBestSampleResult::Ok;
}

bool
FOdysseyAnimationMediaSamples::PeekVideoSampleTime(FMediaTimeStamp & TimeStamp)
{
    //ES: The purpose of this function is to provide a time to the playerfacade.
    //That time will be used as the start time when playing (i know it's weird)
    //It is called on the first play and after a seek.
    //It must represent the start time of a sample
    //The way it is usually done is by giving the start time of the first sample in the queue.
    //But we don't have a queue.
    //So we should just return the first frame which overlaps CurrentTime.
    //It does not need to be as precise as FetchBestVideoSampleForTimeRange.

    TSharedPtr<FOdysseyAnimationMediaControls> controls = mControls.Pin();

    TimeStamp.Time = controls->GetTime();
    TimeStamp.SetSequenceIndex( controls->GetSequenceIndex() );
    return true;
}

void
FOdysseyAnimationMediaSamples::Update(int iFrameIndex, int64 iSequenceIndex)
{
    TSharedPtr<FOdysseyAnimationMediaControls> controls = mControls.Pin();
    if (!controls)
        return;

    TRange<FTimespan> timeRange = mAnimation->GetFrameTimeRange(iFrameIndex);
    FMediaTimeStamp frameTime = FMediaTimeStamp(timeRange.GetLowerBoundValue(), iSequenceIndex);
    FTimespan frameDuration = timeRange.Size<FTimespan>();

    mSample->SetTime(frameTime);
    mSample->SetDuration(frameDuration);

    int controlsStartFrame = controls->GetFrameRange().GetLowerBoundValue();
    mCurrentFrameIndex = iFrameIndex + controlsStartFrame;
    TArray<FGuid> imageRenderingComposition = mAnimation->GetRenderingComposition(mRenderType, mCurrentFrameIndex);

    if ( imageRenderingComposition == mImageRenderingComposition )
        return;

    mImageRenderingComposition = imageRenderingComposition;
    mAnimation->RenderToTexture(mRenderTarget.Get(), FFrameNumber(mCurrentFrameIndex));
}

void
FOdysseyAnimationMediaSamples::OnRenderingChanged(const FOdysseyRenderingChangedEvent& iEvent)
{
    if ( !mAnimation )
        return;

    if (!mImageRenderingComposition.Contains(iEvent.GetId()) )
        return;

    if (iEvent.GetType() == FOdysseyRenderingChangedEvent::eEventType::kCompositionChange)
    {
        TArray<FGuid> imageRenderingComposition = mAnimation->GetRenderingComposition(mRenderType, mCurrentFrameIndex);
        if ( imageRenderingComposition == mImageRenderingComposition )
            return;

        mInvalidTileMap.Invalidate();
        mImageRenderingComposition = imageRenderingComposition;
    }
    else if (iEvent.GetType() == FOdysseyRenderingChangedEvent::eEventType::kValueChange)
    {
        //delay rects update to tick
        mInvalidTileMap.Invalidate(iEvent.GetRects());
    }
}

void
FOdysseyAnimationMediaSamples::Tick(float DeltaTime)
{
    Render();
}

void
FOdysseyAnimationMediaSamples::Render()
{
    if ( !mAnimation )
        return;

    if ( mInvalidTileMap.InvalidTiles().IsEmpty() )
        return;

    mAnimation->RenderToTexture(mRenderTarget.Get(), FFrameNumber(mCurrentFrameIndex), mInvalidTileMap.InvalidRects());
    mInvalidTileMap.Clear();
}
