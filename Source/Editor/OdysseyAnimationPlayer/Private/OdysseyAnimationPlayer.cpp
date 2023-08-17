// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyAnimationPlayer.h"
#include "OdysseyAnimation.h"
#include "OdysseyRectUtils.h"
#include "Abilities/IOdysseyAnimationImageRenderingAbility.h"

#include "ULISLoaderModule.h"

void
UOdysseyAnimationPlayer::PostInitProperties()
{
    Super::PostInitProperties();

	IOdysseyAnimationImageRenderingAbility::OnChanged().AddUObject(this, &UOdysseyAnimationPlayer::OnImageRenderingChanged);
	IOdysseyAnimationImageRenderingAbility::OnCompositionChanged().AddUObject(this, &UOdysseyAnimationPlayer::OnImageRenderingCompositionChanged);
}

FSimpleMulticastDelegate&
UOdysseyAnimationPlayer::OnAnimationChanged()
{
	return mOnAnimationChanged;
}

FSimpleMulticastDelegate&
UOdysseyAnimationPlayer::OnTextureChanged()
{
	return mOnTextureChanged;
}

FSimpleMulticastDelegate&
UOdysseyAnimationPlayer::OnTextureUpdated()
{
	return mOnTextureUpdated;
}

FSimpleMulticastDelegate&
UOdysseyAnimationPlayer::OnFramesPerSecondChanged()
{
	return mOnFramesPerSecondChanged;
}

FSimpleMulticastDelegate&
UOdysseyAnimationPlayer::OnIsLoopingChanged()
{
	return mOnIsLoopingChanged;
}

FSimpleMulticastDelegate&
UOdysseyAnimationPlayer::OnCurrentTimeChanged()
{
	return mOnCurrentTimeChanged;
}

FSimpleMulticastDelegate&
UOdysseyAnimationPlayer::OnPlay()
{
	return mOnPlay;
}

FSimpleMulticastDelegate&
UOdysseyAnimationPlayer::OnPause()
{
	return mOnPause;
}

FSimpleMulticastDelegate&
UOdysseyAnimationPlayer::OnStop()
{
	return mOnStop;
}


void
UOdysseyAnimationPlayer::SetAnimation(UOdysseyAnimation* iAnimation)
{
	if (!iAnimation)
	{
		Animation = nullptr;
		Texture = nullptr;
		mOnAnimationChanged.Broadcast();
		mOnTextureChanged.Broadcast();
		return;
	}

	Animation = iAnimation;
	Texture = UTexture2D::CreateTransient(Animation->Width(), Animation->Height(), PF_B8G8R8A8);
	Texture->UpdateResource();
	FramesPerSecond = Animation->GetFramesPerSecond();
	mInvalidTileMap = FULISInvalidTileMap(64, Animation->Width(), Animation->Height());

	mOnAnimationChanged.Broadcast();
	mOnTextureChanged.Broadcast();
	mOnFramesPerSecondChanged.Broadcast();
}

UOdysseyAnimation*
UOdysseyAnimationPlayer::GetAnimation() const
{
	return Animation;
}

UTexture2D*
UOdysseyAnimationPlayer::GetTexture() const
{
	return Texture;
}

void
UOdysseyAnimationPlayer::Play(bool iBackward)
{
	mIsBackward = iBackward;
	Status = EOdysseyAnimationPlayerStatus::Playing;

	mOnPlay.Broadcast();
}

void
UOdysseyAnimationPlayer::Pause()
{
	Status = EOdysseyAnimationPlayerStatus::Paused;
	mOnPause.Broadcast();
}

void
UOdysseyAnimationPlayer::Stop()
{
	Status = EOdysseyAnimationPlayerStatus::Stopped;
	mOnStop.Broadcast();
}

void
UOdysseyAnimationPlayer::SeekToTime(FTimespan iTime)
{
	mCurrentTime = iTime;
	mOnCurrentTimeChanged.Broadcast();
}

void
UOdysseyAnimationPlayer::SeekToFrame(int iFrameIndex)
{
	mCurrentTime = FTimespan::FromSeconds((Animation->CurrentFrame) / Animation->GetFramesPerSecond());
	mOnCurrentTimeChanged.Broadcast();
}

void
UOdysseyAnimationPlayer::SetFramesPerSecond(double iFramesPerSecond)
{
	FramesPerSecond = iFramesPerSecond;
	mOnFramesPerSecondChanged.Broadcast();
}

double
UOdysseyAnimationPlayer::GetFramesPerSecond() const
{
	return FramesPerSecond;
}

void
UOdysseyAnimationPlayer::SetIsLooping(bool iIsLooping)
{
	IsLooping = iIsLooping;
	mOnIsLoopingChanged.Broadcast();
}

bool
UOdysseyAnimationPlayer::GetIsLooping() const
{
	return IsLooping;
}

EOdysseyAnimationPlayerStatus
UOdysseyAnimationPlayer::GetStatus() const
{
	return Status;
}

FTimespan
UOdysseyAnimationPlayer::GetCurrentTime() const
{
	return mCurrentTime;
}

bool
UOdysseyAnimationPlayer::IsBackward() const
{
	return mIsBackward;
}

void
UOdysseyAnimationPlayer::SetRenderType(IOdysseyImageRenderer::eRenderType iRenderType)
{
	mRenderType = iRenderType;
}

IOdysseyImageRenderer::eRenderType
UOdysseyAnimationPlayer::GetRenderType() const
{
	return mRenderType;
}

void
UOdysseyAnimationPlayer::Tick(float iDeltaTime)
{
	if (!Animation)
		return;

	if (Status == EOdysseyAnimationPlayerStatus::Playing)
	{
		bool bStop = false;
		FTimespan newTime = mCurrentTime;
		if ( mIsBackward )
		{
			newTime -= FTimespan::FromSeconds(iDeltaTime * FramesPerSecond / Animation->GetFramesPerSecond());
			if ( newTime < FTimespan::Zero() )
			{
				if ( IsLooping )
				{
					while ( newTime < FTimespan::Zero() )
					{
						newTime += Animation->GetDuration();
					}
				}
				else
				{
					newTime = FTimespan::Zero();
					bStop = true;
				}
			}
		}
		else
		{
			newTime += FTimespan::FromSeconds(iDeltaTime * FramesPerSecond / Animation->GetFramesPerSecond());
			FTimespan duration = Animation->GetDuration();
			if ( newTime > Animation->GetDuration() )
			{
				if ( IsLooping )
				{
					while ( newTime > Animation->GetDuration() )
					{
						newTime -= Animation->GetDuration();
					}
				}
				else
				{
					newTime = Animation->GetDuration();
					bStop = true;
				}
			}
		}

		SeekToTime(newTime);
		if (bStop)
			Stop();
	}

	//If the frame has changed, render the whole frame
	UpdateTexture();
}

void
UOdysseyAnimationPlayer::UpdateTexture()
{
	TRACE_CPUPROFILER_EVENT_SCOPE(UOdysseyAnimationPlayer::UpdateTexture);
	int frameIndex = Animation->GetFrameIndexAtTime(mCurrentTime);
	if ( frameIndex == INDEX_NONE )
		return;

	TSharedPtr<IOdysseyAnimationImageRenderingAbility> imageRenderingAbility = Animation->GetAbility<IOdysseyAnimationImageRenderingAbility>();
	if ( !imageRenderingAbility )
		return;

	TArray<FGuid> imageRenderingComposition = imageRenderingAbility->GetComposition(frameIndex, mRenderType);
	if ( imageRenderingComposition != mImageRenderingComposition )
	{
		mImageRenderingComposition = imageRenderingComposition;
		mAnimationHandle = imageRenderingAbility->Preload(frameIndex, mRenderType);
		TSharedPtr<IOdysseyImageRenderer> renderer = imageRenderingAbility->BuildRenderer(frameIndex, mRenderType);

		::ULIS::FRectI rect = ::ULIS::FRectI::FromXYWH(0, 0, Animation->Width(), Animation->Height());
		TSharedPtr<::ULIS::FBlock> block = MakeShared<::ULIS::FBlock>(Animation->Width(), Animation->Height(), Animation->Format());

		{
			TRACE_CPUPROFILER_EVENT_SCOPE(UOdysseyAnimationPlayer::UpdateTexture);
			renderer->Copy(block, rect, {});

			::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(Animation->Format());
			ctx.Finish();
		}

		CopyBlocksToTexture({ block }, { rect });
		mInvalidTileMap.Clear();
		mOnTextureUpdated.Broadcast();
		return;
	}

	if (!mInvalidTileMap.InvalidTiles().IsEmpty())
	{
		TSharedPtr<IOdysseyImageRenderer> renderer = imageRenderingAbility->BuildRenderer(frameIndex, mRenderType);

		TArray<TSharedPtr<::ULIS::FBlock>> blocks;
		TArray<::ULIS::FRectI> invalidRects = mInvalidTileMap.InvalidRects();
		for ( const ::ULIS::FRectI& rect : invalidRects )
		{
			TSharedPtr<::ULIS::FBlock> block = MakeShared<::ULIS::FBlock>(rect.w, rect.h, Animation->Format());
			renderer->Copy(block, rect, ::ULIS::FVec2I(0), {});
			blocks.Add(block);
		}
		
		::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(Animation->Format());
		ctx.Finish();

		CopyBlocksToTexture(blocks, invalidRects);
		mInvalidTileMap.Clear();
		mOnTextureUpdated.Broadcast();
	}
}

void
UOdysseyAnimationPlayer::OnImageRenderingChanged(const FGuid& iId, const TArray<::ULIS::FRectI>& iRects)
{
	if (!mImageRenderingComposition.Contains(iId))
		return;

	mInvalidTileMap.Invalidate(iRects);
}

void
UOdysseyAnimationPlayer::OnImageRenderingCompositionChanged(const FGuid& iId)
{
	if ( !mImageRenderingComposition.Contains(iId) )
		return;

	int frameIndex = Animation->GetFrameIndexAtTime(mCurrentTime);
	if (frameIndex == INDEX_NONE)
		return;

	TSharedPtr<IOdysseyAnimationImageRenderingAbility> imageRenderingAbility = Animation->GetAbility<IOdysseyAnimationImageRenderingAbility>();
	if ( !imageRenderingAbility )
		return;

	TArray<FGuid> imageRenderingComposition = imageRenderingAbility->GetComposition(frameIndex, mRenderType);
	if ( imageRenderingComposition == mImageRenderingComposition )
		return;

	mInvalidTileMap.Invalidate(::ULIS::FRectI::FromXYWH(0, 0, Animation->Width(), Animation->Height()));
}

void
UOdysseyAnimationPlayer::CopyBlocksToTexture(const TArray<TSharedPtr<::ULIS::FBlock>>& iBlocks, const TArray<::ULIS::FRectI>& iRects)
{
	if ( iBlocks.IsEmpty() )
		return;

	::ULIS::eFormat format = iBlocks[0]->Format();

	//convert block to BGRA8 if needed*
	if ( format == ::ULIS::Format_BGRA8 )
	{
		TArray<TSharedPtr<FUpdateTextureRegion2D>> regions; //Keeps region object alive until fence.Wait()
		for ( int i = 0; i < iBlocks.Num(); i++ )
		{
			const TSharedPtr<::ULIS::FBlock>& block = iBlocks[i];
			const ::ULIS::FRectI& rect = iRects[i];
			regions.Add(MakeShared<FUpdateTextureRegion2D>(rect.x, rect.y, 0, 0, block->Rect().w, block->Rect().h));
			Texture->UpdateTextureRegions(0, 1, regions.Last().Get(), block->BytesPerScanLine(), block->BytesPerPixel(), block->Bits());
		}

		FRenderCommandFence fence;
		fence.BeginFence();
		fence.Wait();

		return;
	}

	::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(::ULIS::Format_BGRA8);
	TArray<TSharedPtr<::ULIS::FBlock>> convBlocks;
	TArray<TSharedPtr<FUpdateTextureRegion2D>> regions;
	for ( int i = 0; i < iBlocks.Num(); i++ )
	{
		const TSharedPtr<::ULIS::FBlock>& block = iBlocks[i];
		const ::ULIS::FRectI& rect = iRects[i];

		TSharedPtr<::ULIS::FBlock> convBlock = MakeShared<::ULIS::FBlock>(rect.w, rect.h, ::ULIS::Format_BGRA8);
		ctx.ConvertFormat(*block, *convBlock, block->Rect(), ::ULIS::FVec2I(0), ::ULIS::FSchedulePolicy::AsyncCacheEfficient);
		convBlocks.Add(convBlock);
		regions.Add(MakeShared<FUpdateTextureRegion2D>(rect.x, rect.y, 0, 0, rect.w, rect.h));
	}
	
	ctx.Finish();

	for ( int i = 0; i < regions.Num(); i++ )
	{
		Texture->UpdateTextureRegions(
			0,
			1,
			regions[i].Get(),
			convBlocks[i]->BytesPerScanLine(),
			convBlocks[i]->BytesPerPixel(),
			convBlocks[i]->Bits()
		);
	}

	FRenderCommandFence fence;
	fence.BeginFence();
	fence.Wait();
}