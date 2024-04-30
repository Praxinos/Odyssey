// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Media/OdysseyAnimationMediaSamples.h"

#include "Media/OdysseyAnimationMediaPlayer.h"
#include "Media/OdysseyAnimationMediaTextureSample.h"
#include "Engine/Texture2D.h"

FOdysseyAnimationMediaSamples::FOdysseyAnimationMediaSamples()
	: mAnimation(nullptr)
    , mCurrentFrameIndex(INDEX_NONE)
    , mImageRenderingComposition()
    , mTexture()
    //, mTexture2()
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
	FOdysseyImageRenderingAbility::OnImageRenderingChangedDelegate().AddRaw(this, &FOdysseyAnimationMediaSamples::OnImageRenderingChanged);
	//IOdysseyAnimationImageRenderingAbility::OnCompositionChanged().AddRaw(this, &FOdysseyAnimationMediaSamples::OnImageRenderingCompositionChanged);
	mTexture = TStrongObjectPtr<UTexture2D>(UTexture2D::CreateTransient(mAnimation->Width(), mAnimation->Height(), PF_B8G8R8A8));
	//mTexture2 = TStrongObjectPtr<UTexture2D>(UTexture2D::CreateTransient(mAnimation->Width(), mAnimation->Height(), PF_B8G8R8A8));
	mSample = MakeShared<FOdysseyAnimationMediaTextureSample>(mAnimation->Width(), mAnimation->Height(), mTexture.Get());
	mImageRenderingComposition.Empty();

	mInvalidTileMap = FULISInvalidTileMap(64, mAnimation->Width(), mAnimation->Height());

	mTexture->UpdateResource();
}

IOdysseyImageRenderer::eRenderType
FOdysseyAnimationMediaSamples::GetRenderType() const
{
	return mRenderType;
}

void
FOdysseyAnimationMediaSamples::SetRenderType(IOdysseyImageRenderer::eRenderType iRenderType)
{
	if (!mAnimation)
		return;
		
	mRenderType = iRenderType;

	TArray<FGuid> imageRenderingComposition = mAnimation->GetImageRenderingComposition(mRenderType, mCurrentFrameIndex);
	if ( imageRenderingComposition == mImageRenderingComposition )
		return;

	mInvalidTileMap.Invalidate();
	mImageRenderingComposition = imageRenderingComposition;
}

void
FOdysseyAnimationMediaSamples::OnClose()
{
	SetRenderType(IOdysseyImageRenderer::eRenderType::Render);
	Render();
	FOdysseyImageRenderingAbility::OnImageRenderingChangedDelegate().RemoveAll(this);
	
	mInvalidTileMap.Clear();

	mAnimation = nullptr;
	mSample = nullptr;
	mTexture = nullptr;

	mImageRenderingComposition.Empty();
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
		timestamp.SequenceIndex--;
		timeRange.SetLowerBoundValue(timestamp);
		isLowerOutOfBound = false;
	}
	
	bool isUpperOutOfBound = timeRange.HasUpperBound() && timeRange.GetUpperBoundValue().Time >= controls->GetDuration();
	if (isUpperOutOfBound && controls->IsLooping())
	{
		FMediaTimeStamp timestamp = timeRange.GetUpperBoundValue();
		timestamp.Time -= controls->GetDuration();
		timestamp.SequenceIndex++;
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
	//TODO: DOes anyone understand what should the bConsistentResult parameter do ?

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
	bool isAtEnd = SanitizeTimeRange(&timeRange, bReverse);

	//Once time range is sanitized
	//Find which frame overlaps the timerange the most
	FTimespan startTime = timeRange.GetLowerBoundValue().Time;
	FTimespan endTime = timeRange.GetUpperBoundValue().Time;
	int startFrameIndex = FMath::Clamp(mAnimation->GetFrameIndexAtTime(startTime), 0, controls->GetFrameCount());
	int endFrameIndex = FMath::Clamp(mAnimation->GetFrameIndexAtTime(endTime), 0, controls->GetFrameCount());
	int64 startSequenceIndex = timeRange.GetLowerBoundValue().SequenceIndex;
	int64 endSequenceIndex = timeRange.GetUpperBoundValue().SequenceIndex;

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
	TArray<FGuid> imageRenderingComposition = mAnimation->GetImageRenderingComposition(mRenderType, mCurrentFrameIndex);

	if ( imageRenderingComposition == mImageRenderingComposition )
		return;

	mImageRenderingComposition = imageRenderingComposition;

	::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(mAnimation->Format());
	::ULIS::FRectI rect = ::ULIS::FRectI::FromXYWH(0, 0, mAnimation->Width(), mAnimation->Height());
	TArray<::ULIS::FEvent> events;
	TSharedPtr<IOdysseyImageRenderer> renderer = mAnimation->BuildImageRenderer(mRenderType, mCurrentFrameIndex);
	renderer->Init();

	TSharedPtr<::ULIS::FBlock> block = MakeShared<::ULIS::FBlock>(mAnimation->Width(), mAnimation->Height(), mAnimation->Format());
	FOdysseyImageRendererBlendParams params(block, {block->Rect()});
	renderer->Copy(params, {});
	ctx.Finish();

	CopyBlockToTexture(block, { rect });
}

void
FOdysseyAnimationMediaSamples::CopyBlockToTexture(TSharedPtr<::ULIS::FBlock> iBlock, const TArray<::ULIS::FRectI>& iRects)
{
	if ( !iBlock )
		return;

	//convert block to BGRA8 if needed
	if ( iBlock->Format() == ::ULIS::Format_BGRA8 )
	{
		TArray<FUpdateTextureRegion2D> regions;
		for (const ::ULIS::FRectI& rect : iRects)
		{
			regions.Emplace(rect.x, rect.y, rect.x, rect.y, rect.w, rect.h);
		}
		mTexture->UpdateTextureRegions(0, regions.Num(), regions.GetData(), iBlock->BytesPerScanLine(), iBlock->BytesPerPixel(), iBlock->Bits());

		FRenderCommandFence fence;
		fence.BeginFence();
		fence.Wait();

		return;
	}

	//Here block has not the expected format
	//But instead of converting the whole block, we will convert only the parts of the block we need
	::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(::ULIS::Format_BGRA8);
	TArray<FUpdateTextureRegion2D> regions;
	TArray<TSharedPtr<::ULIS::FBlock>> blocks;
	for (const ::ULIS::FRectI& rect : iRects)
	{
		TSharedPtr<::ULIS::FBlock> block = MakeShared<::ULIS::FBlock>(rect.w, rect.h, ::ULIS::Format_BGRA8);
		ctx.ConvertFormat(*iBlock, *block, rect, ::ULIS::FVec2I(0), ::ULIS::FSchedulePolicy::AsyncCacheEfficient);
		regions.Emplace(rect.x, rect.y, 0, 0, rect.w, rect.h);
		blocks.Add(block);
	}
	ctx.Finish();

	for (int i = 0; i < regions.Num(); i++)
	{
		mTexture->UpdateTextureRegions(
			0,
			1,
			&regions[i],
			blocks[i]->BytesPerScanLine(),
			blocks[i]->BytesPerPixel(),
			blocks[i]->Bits()
		);
	}

	FRenderCommandFence fence;
	fence.BeginFence();
	fence.Wait();
}

void
FOdysseyAnimationMediaSamples::OnImageRenderingChanged(const FOdysseyImageRenderingChangedEvent& iEvent)
{
	if ( !mAnimation )
		return;

	if (!mImageRenderingComposition.Contains(iEvent.GetId()) )
		return;

	if (iEvent.GetType() == FOdysseyImageRenderingChangedEvent::eEventType::kCompositionChange)
	{
		TArray<FGuid> imageRenderingComposition = mAnimation->GetImageRenderingComposition(mRenderType, mCurrentFrameIndex);
		if ( imageRenderingComposition == mImageRenderingComposition )
			return;

		mInvalidTileMap.Invalidate();
		mImageRenderingComposition = imageRenderingComposition;
	}
	else if (iEvent.GetType() == FOdysseyImageRenderingChangedEvent::eEventType::kValueChange)
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

	TSharedPtr<IOdysseyImageRenderer> renderer = mAnimation->BuildImageRenderer(mRenderType, mCurrentFrameIndex);
	renderer->Init();
	TSharedPtr<::ULIS::FBlock> block = MakeShared<::ULIS::FBlock>(mAnimation->Width(), mAnimation->Height(), mAnimation->Format());

	FOdysseyImageRendererBlendParams params(block, mInvalidTileMap.InvalidRects());
	renderer->Copy(params, {});

	::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(mAnimation->Format());
	ctx.Finish();

	CopyBlockToTexture(block, mInvalidTileMap.InvalidRects());
    mInvalidTileMap.Clear();
}
