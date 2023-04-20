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
	Animation = iAnimation;
	Texture = UTexture2D::CreateTransient(Animation->Width(), Animation->Height(), PF_B8G8R8A8);
	Texture->UpdateResource();
	FramesPerSecond = Animation->GetFramesPerSecond();

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
	int frameIndex = Animation->GetFrameIndexAtTime(mCurrentTime);
	if ( frameIndex == INDEX_NONE )
		return;

	TSharedPtr<IOdysseyAnimationImageRenderingAbility> imageRenderingAbility = Animation->GetAbility<IOdysseyAnimationImageRenderingAbility>();
	if ( !imageRenderingAbility )
		return;

	TArray<FGuid> imageRenderingComposition = imageRenderingAbility->GetComposition(frameIndex);
	if ( imageRenderingComposition != mImageRenderingComposition )
	{
		mImageRenderingComposition = imageRenderingComposition;
		mInvalidRects = { ::ULIS::FRectI::FromXYWH(0, 0, Animation->Width(), Animation->Height()) };
	}

	if (mInvalidRects.Num() > 0)
	{
		::ULIS::eFormat format = Animation->Format();
		::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(format);
		for ( const ::ULIS::FRectI& rect : mInvalidRects )
		{
			imageRenderingAbility->RenderInBlock(mBlock, frameIndex, rect, rect.Position(), {});
		}
		ctx.Finish();

		CopyBlockToTexture(mBlock, mInvalidRects);
		mInvalidRects.Empty();

		mOnTextureUpdated.Broadcast();
	}
}

void
UOdysseyAnimationPlayer::OnImageRenderingChanged(const FGuid& iId, const TArray<::ULIS::FRectI>& iRects)
{
	if (!mImageRenderingComposition.Contains(iId))
		return;

	mInvalidRects.Append(iRects);
	mInvalidRects = OdysseyRectUtils::MergeRects(mInvalidRects);
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

	TArray<FGuid> imageRenderingComposition = imageRenderingAbility->GetComposition(frameIndex);
	if ( imageRenderingComposition == mImageRenderingComposition )
		return;

	mInvalidRects.Add(::ULIS::FRectI::FromXYWH(0, 0, Animation->Width(), Animation->Height()));
	mInvalidRects = OdysseyRectUtils::MergeRects(mInvalidRects);
}

void
UOdysseyAnimationPlayer::CopyBlockToTexture(TSharedPtr<::ULIS::FBlock> iBlock, const TArray<::ULIS::FRectI>& iRects)
{
	//convert block to BGRA8 if needed
	if ( iBlock->Format() == ::ULIS::Format_BGRA8 )
	{
		TArray<FUpdateTextureRegion2D> regions;
		for (const ::ULIS::FRectI& rect : iRects)
		{
			regions.Emplace(rect.x, rect.y, rect.x, rect.y, rect.w, rect.h);
		}
		Texture->UpdateTextureRegions(0, regions.Num(), regions.GetData(), iBlock->BytesPerScanLine(), iBlock->BytesPerPixel(), iBlock->Bits());

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
		Texture->UpdateTextureRegions(
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