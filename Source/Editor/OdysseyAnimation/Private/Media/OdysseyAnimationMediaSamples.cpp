// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Media/OdysseyAnimationMediaSamples.h"

#include "Media/OdysseyAnimationMediaPlayer.h"
#include "Media/OdysseyAnimationMediaTextureSample.h"
#include "Engine/Texture2DDynamic.h"

#define LOCTEXT_NAMESPACE "OdysseyAnimationMediaSamples"

FOdysseyAnimationMediaSamples::FOdysseyAnimationMediaSamples()
	: mAnimation(nullptr)
    , mCurrentFrameIndex(INDEX_NONE)
    , mFrameId()
    , mTexture1()
    , mTexture2()
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
    mAnimation->OnRenderImageChanged().AddRaw(this, &FOdysseyAnimationMediaSamples::OnRenderImageChanged);
	mTexture1 = TStrongObjectPtr<UTexture2DDynamic>(UTexture2DDynamic::Create(mAnimation->Width(), mAnimation->Height(), FTexture2DDynamicCreateInfo(PF_B8G8R8A8)));
	mTexture2 = TStrongObjectPtr<UTexture2DDynamic>(UTexture2DDynamic::Create(mAnimation->Width(), mAnimation->Height(), FTexture2DDynamicCreateInfo(PF_B8G8R8A8)));
	mSample = MakeShared<FOdysseyAnimationMediaTextureSample>(mAnimation->Width(), mAnimation->Height(), mTexture1.Get(), mTexture2.Get());
}

void
FOdysseyAnimationMediaSamples::OnClose()
{
    mAnimation->OnRenderImageChanged().RemoveAll(this);
	mAnimation = nullptr;
	mSample = nullptr;
	mTexture1 = nullptr;
	mTexture2 = nullptr;
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
	int startFrameIndex = FMath::Clamp(mAnimation->GetFrameIndexAtTime(startTime), 0, mAnimation->GetFrameCount());
	int endFrameIndex = FMath::Clamp(mAnimation->GetFrameIndexAtTime(endTime), 0, mAnimation->GetFrameCount());
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
FOdysseyAnimationMediaSamples::Update(int iFrameIndex, uint32 iSequenceIndex)
{
    mCurrentFrameIndex = iFrameIndex;

    FString frameId = mAnimation->GetFrameId(mCurrentFrameIndex);
    if ( frameId == mFrameId )
        return;

    mFrameId = frameId;

    TRange<FTimespan> timeRange = mAnimation->GetFrameTimeRange(mCurrentFrameIndex);
	FMediaTimeStamp frameTime = FMediaTimeStamp(timeRange.GetLowerBoundValue(), iSequenceIndex);
    FTimespan frameDuration = timeRange.Size<FTimespan>();

	mSample->SetTime(frameTime);
	mSample->SetDuration(frameDuration);

    CopyRects({ ::ULIS::FRectI::FromXYWH(0, 0, mAnimation->Width(), mAnimation->Height())});
}

void
FOdysseyAnimationMediaSamples::CopyRects(const TArray<::ULIS::FRectI>& iRects)
{

    TSharedPtr<::ULIS::FBlock> srcBlock = mAnimation->GetBlockFromId(mFrameId);
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(srcBlock->Format());
    ENQUEUE_RENDER_COMMAND(FWriteRawDataToTexture)(
        [this, srcBlock, iRects](FRHICommandListImmediate& RHICmdList)
        {
            FTexture2DDynamicResource* resource1 = static_cast<FTexture2DDynamicResource*>(mTexture1->GetResource());
            if ( !resource1 )
                return;

            FTexture2DDynamicResource* resource2 = static_cast<FTexture2DDynamicResource*>(mTexture2->GetResource());
            if ( !resource2 )
                return;

            CopyRects_RenderThread(resource1, srcBlock, iRects);
            CopyRects_RenderThread(resource2, srcBlock, iRects);
        }
    );

    FRenderCommandFence fence1;
    fence1.BeginFence();
    fence1.Wait();

    ctx.Finish();

    ENQUEUE_RENDER_COMMAND(FWriteRawDataToTexture2)(
        [this](FRHICommandListImmediate& RHICmdList)
        {
            FTexture2DDynamicResource* resource1 = static_cast<FTexture2DDynamicResource*>(mTexture1->GetResource());
            if ( !resource1 )
                return;

            FTexture2DDynamicResource* resource2 = static_cast<FTexture2DDynamicResource*>(mTexture2->GetResource());
            if ( !resource2 )
                return;

            FTexture2DRHIRef rhi1 = resource1->GetTexture2DRHI();
            FTexture2DRHIRef rhi2 = resource2->GetTexture2DRHI();
            RHIUnlockTexture2D(rhi1, 0, false, false);
            RHIUnlockTexture2D(rhi2, 0, false, false);
        }
    );
    
    FRenderCommandFence fence;
    fence.BeginFence();
    fence.Wait();
}

void
FOdysseyAnimationMediaSamples::CopyRects_RenderThread(FTexture2DDynamicResource* iResource, TSharedPtr<::ULIS::FBlock> iSrc, const TArray<::ULIS::FRectI>& iRects)
{
    check(IsInRenderingThread());

    FTexture2DRHIRef rhi = iResource->GetTexture2DRHI();

	const int32 w = rhi->GetSizeX();
	const int32 h = rhi->GetSizeY();

    //8bits version
	uint32 stride = 0;
	uint8* data = reinterpret_cast<uint8*>(RHILockTexture2D(rhi, 0, RLM_WriteOnly, stride, false, false));
    TSharedPtr<::ULIS::FBlock> dstBlock = MakeShared<::ULIS::FBlock>(data, w, h, ::ULIS::eFormat::Format_BGRA8);

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(iSrc->Format());

    // ES: Instead we copy the whole texture 
    ::ULIS::FEvent eventConvert = FULISEventBuilder().RetainBlock(dstBlock).Build();
    ctx.ConvertFormat(
        *iSrc,
        *dstBlock,
        dstBlock->Rect(),
        ::ULIS::FVec2I(0),
        ::ULIS::FSchedulePolicy::AsyncCacheEfficient,
        0, 
        nullptr,
        &eventConvert
    );
}

void
FOdysseyAnimationMediaSamples::OnRenderImageChanged(UOdysseyAnimation* iAnimation, const TRange<int>& iRange, const TArray<::ULIS::FRectI>& iRects, bool iIsInteractive)
{
	if (iAnimation != mAnimation)
		return;

    if ( iRange.Contains(mCurrentFrameIndex) )
    {
        //delay rects update to tick
        mInvalidRects.Append(iRects);
        mInvalidRects = OdysseyRectUtils::MergeRects(mInvalidRects);
    }
}

void
FOdysseyAnimationMediaSamples::Tick(float DeltaTime)
{
    if ( mInvalidRects.IsEmpty() )
        return;

    CopyRects(mInvalidRects);
    mInvalidRects.Empty();
}

#undef LOCTEXT_NAMESPACE