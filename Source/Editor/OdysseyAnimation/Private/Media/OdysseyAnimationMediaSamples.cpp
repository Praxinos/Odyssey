// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Media/OdysseyAnimationMediaSamples.h"

#include "Media/OdysseyAnimationMediaPlayer.h"
#include "Media/OdysseyAnimationMediaTextureSample.h"

#define LOCTEXT_NAMESPACE "OdysseyAnimationMediaSamples"

FOdysseyAnimationMediaSamples::FOdysseyAnimationMediaSamples()
	: mAnimation(nullptr)
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
	mSample = MakeShared<FOdysseyAnimationMediaTextureSample>(mAnimation);
}

void
FOdysseyAnimationMediaSamples::OnClose()
{
	mAnimation = nullptr;
}

void
FOdysseyAnimationMediaSamples::FlushSamples()
{
	//ES:we should remove all the samples from the sample queue here
	//But we don't have a sample queue, so we do nothing
}

FTimespan
FOdysseyAnimationMediaSamples::FindMaxOverlapingFrame(FTimespan iStartTime, FTimespan iEndTime, int* oIndex)
{
    if (!mAnimation)
        return FTimespan();

	int startFrameIndex = mAnimation->GetFrameIndexAtTime(iStartTime);
	int endFrameIndex = mAnimation->GetFrameIndexAtTime(iEndTime);
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
FOdysseyAnimationMediaSamples::SanitizeTimeRange(TRange<FMediaTimeStamp>* oTimeRange)
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
		timestamp.Time += mAnimation->GetDuration();
		timestamp.SequenceIndex--;
		timeRange.SetLowerBoundValue(timestamp);
		isLowerOutOfBound = false;
	}
	
	bool isUpperOutOfBound = timeRange.HasUpperBound() && timeRange.GetUpperBoundValue().Time >= mAnimation->GetDuration();
	if (isUpperOutOfBound && controls->IsLooping())
	{
		FMediaTimeStamp timestamp = timeRange.GetUpperBoundValue();
		timestamp.Time -= mAnimation->GetDuration();
		timestamp.SequenceIndex++;
		timeRange.SetUpperBoundValue(timestamp);
		isUpperOutOfBound = false;
	}

	//Clamp timerange just for security
	
	FMediaTimeStamp lowerBoundTimestamp = timeRange.GetLowerBoundValue();
	FMediaTimeStamp upperBoundTimestamp = timeRange.GetUpperBoundValue();
	lowerBoundTimestamp.Time = FMath::Max(FTimespan(0), lowerBoundTimestamp.Time);
	upperBoundTimestamp.Time = FMath::Min(mAnimation->GetDuration(), upperBoundTimestamp.Time);
	timeRange.SetLowerBoundValue(lowerBoundTimestamp);
	timeRange.SetUpperBoundValue(upperBoundTimestamp);

	//End is different if we are playing forward or backward
	bool isAtEnd = controls->GetRate() >= 0 ? isUpperOutOfBound : isLowerOutOfBound;

	return isAtEnd;
}

IMediaSamples::EFetchBestSampleResult
FOdysseyAnimationMediaSamples::FetchBestVideoSampleForTimeRange(const TRange<FMediaTimeStamp>& iTimeRange, TSharedPtr<IMediaTextureSample, ESPMode::ThreadSafe>& OutSample, bool bReverse)
{
    if (!mAnimation)
		return EFetchBestSampleResult::NoSample;

	//	iTimeRange is always goes forward, never backward
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
	bool isAtEnd = SanitizeTimeRange(&timeRange);

	//Once time range is sanitized
	//Find which frame overlaps the timerange the most
	FTimespan startTime = timeRange.GetLowerBoundValue().Time;
	FTimespan endTime = timeRange.GetUpperBoundValue().Time;
	int startFrameIndex = mAnimation->GetFrameIndexAtTime(startTime);
	int endFrameIndex = mAnimation->GetFrameIndexAtTime(endTime);
	uint32 startSequenceIndex = timeRange.GetLowerBoundValue().SequenceIndex;
	uint32 endSequenceIndex = timeRange.GetUpperBoundValue().SequenceIndex;

	//Check if range is valid
	if (startSequenceIndex > endSequenceIndex || startSequenceIndex == endSequenceIndex && startTime > endTime)
		return EFetchBestSampleResult::NoSample;

	//range is valid
	//check overlap of each frame with the time range
	
	uint32 resultingSequenceIndex = startSequenceIndex;
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
	else if (  (startSequenceIndex < endSequenceIndex && startTime <= endTime)
			|| (endSequenceIndex - startSequenceIndex >= 2) )
	{
		frameIndex = INDEX_NONE;
		FindMaxOverlapingFrame(FTimespan(0), mAnimation->GetDuration(), &frameIndex);
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
		FTimespan overlap2 = FindMaxOverlapingFrame(startTime, mAnimation->GetDuration(), &frameIndex2);
		frameIndex = overlap1 > overlap2 ? frameIndex1 : frameIndex2;
		resultingSequenceIndex = overlap1 > overlap2 ? endSequenceIndex : startSequenceIndex;
	}

	//TODO: Use Cache to retrieve the sample
	//OutSample = MakeShared<FOdysseyAnimationMediaTextureSample>(mAnimation, frameIndex, resultingSequenceIndex);
	mSample->Update(frameIndex, resultingSequenceIndex);
	OutSample = mSample;
	controls->SetTime(mAnimation->GetFrameTimeRange(frameIndex).GetLowerBoundValue());

	//It can sound weird, but this is also the place where we detect that the play needs to stop
	if (isAtEnd)
	{
		// Stop the player.
		//TODO:This should not happen here ? Check this, it is so weird !
		//ES: I checked, I have no other place to do this...
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
	TimeStamp.SequenceIndex = 0;
	return true;
}

#undef LOCTEXT_NAMESPACE