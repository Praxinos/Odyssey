// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Media/OdysseyAnimationMediaSamples.h"

#include "Media/OdysseyAnimationMediaPlayer.h"
#include "Media/OdysseyAnimationMediaTextureSample.h"

#define LOCTEXT_NAMESPACE "OdysseyAnimationMediaSamples"

FOdysseyAnimationMediaSamples::FOdysseyAnimationMediaSamples() :
	mCurrentFrameIndex(-1)
{
}

void
FOdysseyAnimationMediaSamples::Init(TWeakPtr<FOdysseyAnimationMediaPlayer> iPlayer)
{
    mPlayer = iPlayer;
}

void
FOdysseyAnimationMediaSamples::OnOpen()
{
	mCurrentFrameIndex = -1;
}

void
FOdysseyAnimationMediaSamples::OnClose()
{
	mCurrentFrameIndex = -1;
}

void
FOdysseyAnimationMediaSamples::SetTime(FTimespan iTime)
{
	TSharedPtr<FOdysseyAnimationMediaPlayer> player = mPlayer.Pin();
	if ( !player )
		return;

	UOdysseyAnimation* animation = player->GetAnimation();
	if ( !animation )
		return;

	mCurrentFrameIndex = animation->GetFrameIndexAtTime(iTime);
}

void
FOdysseyAnimationMediaSamples::FlushSamples()
{
	//No idea what to do here
}

FTimespan
FOdysseyAnimationMediaSamples::FindMaxOverlapingFrame(FTimespan iStartTime, FTimespan iEndTime, uint32* oIndex)
{
	TSharedPtr<FOdysseyAnimationMediaPlayer> player = mPlayer.Pin();
	if ( !player )
	{
		*oIndex = INDEX_NONE;
		return FTimespan(0);
	}

	UOdysseyAnimation* animation = player->GetAnimation();
	if ( !animation )
		return FTimespan(0);

	uint32 startFrameIndex = animation->GetFrameIndexAtTime(iStartTime);
	uint32 endFrameIndex = animation->GetFrameIndexAtTime(iEndTime);
	FTimespan bestOverlap(-1);
	uint32 bestFrameIndex = INDEX_NONE;
	for (uint32 frameIndex = startFrameIndex; frameIndex <= endFrameIndex; frameIndex++)
	{
		//Compute Overlap for frameIndex
		TRange<FTimespan> range = animation->GetFrameTimeRange(frameIndex);
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
	TSharedPtr<FOdysseyAnimationMediaPlayer> player = mPlayer.Pin();
	if ( !player )
		return false;

	TRange<FMediaTimeStamp>& timeRange = *oTimeRange;
	IMediaControls& controls = player->GetControls();
	UOdysseyAnimation* animation = player->GetAnimation();
	if ( !animation )
		return false;

	//Even if the media player does some kind of looping, we need to ensure the timerange bounds are looping correctly and are valid
	bool isLowerOutOfBound = timeRange.HasLowerBound() && timeRange.GetLowerBoundValue().Time < 0.0f;
	if (isLowerOutOfBound && controls.IsLooping())
	{
		FMediaTimeStamp timestamp = timeRange.GetLowerBoundValue();
		timestamp.Time += animation->GetDuration();
		timestamp.SequenceIndex--;
		timeRange.SetLowerBoundValue(timestamp);
		isLowerOutOfBound = false;
	}
	
	bool isUpperOutOfBound = timeRange.HasUpperBound() && timeRange.GetUpperBoundValue().Time >= animation->GetDuration();
	if (isUpperOutOfBound && controls.IsLooping())
	{
		FMediaTimeStamp timestamp = timeRange.GetUpperBoundValue();
		timestamp.Time -= animation->GetDuration();
		timestamp.SequenceIndex++;
		timeRange.SetUpperBoundValue(timestamp);
		isUpperOutOfBound = false;
	}

	//Clamp timerange just for security
	
	FMediaTimeStamp lowerBoundTimestamp = timeRange.GetLowerBoundValue();
	FMediaTimeStamp upperBoundTimestamp = timeRange.GetUpperBoundValue();
	lowerBoundTimestamp.Time = FMath::Max(FTimespan(0), lowerBoundTimestamp.Time);
	upperBoundTimestamp.Time = FMath::Min(animation->GetDuration(), upperBoundTimestamp.Time);
	timeRange.SetLowerBoundValue(lowerBoundTimestamp);
	timeRange.SetUpperBoundValue(upperBoundTimestamp);

	//End is different if we are playing forward or backward
	bool isAtEnd = controls.GetRate() >= 0 ? isUpperOutOfBound : isLowerOutOfBound;

	return isAtEnd;
}

IMediaSamples::EFetchBestSampleResult
FOdysseyAnimationMediaSamples::FetchBestVideoSampleForTimeRange(const TRange<FMediaTimeStamp>& iTimeRange, TSharedPtr<IMediaTextureSample, ESPMode::ThreadSafe>& OutSample, bool bReverse)
{
	/*
	 *	iTimeRange is always goes forward, never backward
	 *  which means LowerBoundValue is always <= UpperBoundValue
	 */

	TSharedPtr<FOdysseyAnimationMediaPlayer> player = mPlayer.Pin();
	if ( !player )
		return EFetchBestSampleResult::NoSample;

	FOdysseyAnimationMediaControls& controls = player->GetOdysseyControls();
	UOdysseyAnimation* animation = player->GetAnimation();
	if ( !animation )
		return EFetchBestSampleResult::NoSample;

	if (controls.GetState() == EMediaState::Stopped)
		return EFetchBestSampleResult::NoSample;

	//Sanitize the timeRange, to ensure looping and clamp it to animation duration
	//also detects if we reached the end of the animation
	TRange<FMediaTimeStamp> timeRange = iTimeRange;
	bool isAtEnd = SanitizeTimeRange(&timeRange);

	//Once time range is sanitized
	//Find which frame overlaps the timerange the most
	FTimespan startTime = timeRange.GetLowerBoundValue().Time;
	FTimespan endTime = timeRange.GetUpperBoundValue().Time;
	uint32 startFrameIndex = animation->GetFrameIndexAtTime(startTime);
	uint32 endFrameIndex = animation->GetFrameIndexAtTime(endTime);
	uint32 startSequenceIndex = timeRange.GetLowerBoundValue().SequenceIndex;
	uint32 endSequenceIndex = timeRange.GetUpperBoundValue().SequenceIndex;

	//Check if range is valid
	if (startSequenceIndex > endSequenceIndex || startSequenceIndex == endSequenceIndex && startTime > endTime)
		return EFetchBestSampleResult::NoSample;

	//range is valid
	//check overlap of each frame with the time range
	
	//Only a single frame overlaps the range
	if (startSequenceIndex == endSequenceIndex && startFrameIndex == endFrameIndex)
	{
		mCurrentFrameIndex = startFrameIndex;
	}
	//The time range is not looping, so we have a single range to check
	else if (startSequenceIndex == endSequenceIndex)
	{
		//search in [startFrame, endFrame]
		uint32 frameIndex = INDEX_NONE;
		FindMaxOverlapingFrame(startTime, endTime, &frameIndex);
		mCurrentFrameIndex = frameIndex;
	}
	//The time range is looping enough to cover the whole animation duration,
	//so check a single time range covering the whole animation duration.
	else if (  (startSequenceIndex < endSequenceIndex && startTime <= endTime)
			|| (endSequenceIndex - startSequenceIndex >= 2) )
	{
		uint32 frameIndex = INDEX_NONE;
		FindMaxOverlapingFrame(FTimespan(0), animation->GetDuration(), &frameIndex);
		mCurrentFrameIndex = frameIndex;
	}
	else
	{
		//The time range is looping, but not enough to cover the whole animation duration
		//so we have to check two ranges
		//search in [0, endFrame]
		//search in [startFrame, animation->lastFrame]
		uint32 frameIndex1 = INDEX_NONE;
		uint32 frameIndex2 = INDEX_NONE;
		FTimespan overlap1 = FindMaxOverlapingFrame(0, endTime, &frameIndex1);
		FTimespan overlap2 = FindMaxOverlapingFrame(startTime, animation->GetDuration(), &frameIndex2);
		mCurrentFrameIndex = overlap1 > overlap2 ? frameIndex1 : frameIndex2;
	}

	//TODO: Use Cache to retrieve the sample
	OutSample = MakeShared<FOdysseyAnimationMediaTextureSample>(animation, mCurrentFrameIndex);

	//It can sound weird, but this is also the place where we detect that the play needs to stop
	if (isAtEnd)
	{
		// Stop the player.
		player->GetEventSink().ReceiveMediaEvent(EMediaEvent::PlaybackEndReached);
		controls.SetState( EMediaState::Stopped );
		controls.SetRate( 0.0f );
		player->GetEventSink().ReceiveMediaEvent(EMediaEvent::PlaybackSuspended);
	}
			
	return EFetchBestSampleResult::Ok;
}

bool
FOdysseyAnimationMediaSamples::PeekVideoSampleTime(FMediaTimeStamp & TimeStamp)
{
	//ES: I don't know what is the actual purpose of this method
	// I just followed what FImgMediaLoader did
	// If someone knows better, please update this comment

	TSharedPtr<FOdysseyAnimationMediaPlayer> player = mPlayer.Pin();
	if ( !player )
		return false;

	UOdysseyAnimation* animation = player->GetAnimation();
	if ( !animation )
		return false;

	FOdysseyAnimationMediaControls& controls = player->GetOdysseyControls();
	uint32 frameIndex = INDEX_NONE;
	bool isNewSequence = false;

	if ( controls.GetRate() >= 0 )
	{
		frameIndex = mCurrentFrameIndex + 1;
		if (frameIndex >= animation->GetFrameCount())
		{
			if (!controls.IsLooping())
				return false;

			frameIndex = 0;
			isNewSequence = true;
		}
	}
	else
	{
		frameIndex = mCurrentFrameIndex - 1;
		if (frameIndex < 0)
		{
			if (!controls.IsLooping())
				return false;

			frameIndex = animation->GetFrameCount() - 1;
			isNewSequence = true;
		}
	}

	TimeStamp.Time = animation->GetFrameTimeRange(frameIndex).GetLowerBoundValue();
	TimeStamp.SequenceIndex = isNewSequence ? 1 : 0;
	return true;
}

#undef LOCTEXT_NAMESPACE