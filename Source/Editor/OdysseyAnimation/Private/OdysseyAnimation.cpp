// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyAnimation.h"

#include "LayerStack/Layers/LayerImageRaster/OdysseyAnimationLayerImageRaster.h"
#include "LayerStack/OdysseyAnimationLayerStack.h"

#include "Misc/TransactionObjectEvent.h"
#include "OdysseyAnimationImageRenderingAbility.h"

#include <ULIS>
#include "ULISLoaderModule.h"

UOdysseyAnimation::FOnCurrentFrameChanged&
UOdysseyAnimation::OnCurrentFrameChanged()
{
    static FOnCurrentFrameChanged onCurrentFrameChanged;
    return onCurrentFrameChanged;
}

UOdysseyAnimation::FOnFramesPerSecondChanged&
UOdysseyAnimation::OnFramesPerSecondChanged()
{
    static FOnFramesPerSecondChanged onFramesPerSecondChanged;
    return onFramesPerSecondChanged;
}

void UOdysseyAnimation::Init(const FOdysseyAnimationConfiguration& iConfiguration)
{
	mWidth = iConfiguration.Width;
	mHeight = iConfiguration.Height;
	mFormat = iConfiguration.ULISFormat();
	FramesPerSecond = iConfiguration.FramesPerSecond;

	mLayerStack = NewObject<UOdysseyAnimationLayerStack>(this, "LayerStack", RF_Public | RF_Transactional);
	UOdysseyLayer* layer = mLayerStack->AddLayer(UOdysseyAnimationLayerImageRaster::StaticClass()); //Move in OdysseyAnimationFactor
    mLayerStack->CurrentLayer = TSoftObjectPtr<UOdysseyLayer>(layer);
}

uint32
UOdysseyAnimation::Width() const
{
	return mWidth;
}

uint32
UOdysseyAnimation::Height() const
{
	return mHeight;
}

::ULIS::eFormat
UOdysseyAnimation::Format() const
{
	return ::ULIS::eFormat(mFormat);
}

FTimespan
UOdysseyAnimation::GetDuration() const
{
	return GetFrameTimeRange(GetFrameCount() - 1).GetUpperBoundValue();
}

FInt32Range
UOdysseyAnimation::GetFrameRange() const
{
	//TODO: deduce frame count from :
	// - startPoint / endPoint
	return mLayerStack->GetFrameRange();
}

uint32
UOdysseyAnimation::GetFrameCount() const
{
	FInt32Range frameRange = mLayerStack->GetFrameRange();
	int startFrame = frameRange.GetUpperBound().IsInclusive() ? frameRange.GetLowerBoundValue() : frameRange.GetLowerBoundValue() + 1;
	int endFrame = frameRange.GetUpperBound().IsInclusive() ? frameRange.GetUpperBoundValue() : frameRange.GetUpperBoundValue() - 1;
	return endFrame - startFrame + 1;
}

double
UOdysseyAnimation::GetFramesPerSecond() const
{
	return FramesPerSecond;
}

int
UOdysseyAnimation::GetFrameIndexAtTime(FTimespan iTime) const
{
	if ( iTime < 0 || iTime > GetDuration() )
		return INDEX_NONE;

	//Add 1 tick to be sure to retrieve the right frame in case the frame starts between iTime and iTime + 1 tick
	FTimespan time = iTime + FTimespan(1); 
	return int(time.GetTotalSeconds() * GetFramesPerSecond());
}

TRange<FTimespan>
UOdysseyAnimation::GetFrameTimeRange(int iFrameIndex) const
{
	FTimespan start = FTimespan::FromSeconds(iFrameIndex / GetFramesPerSecond());

	//Remove one tick because end timespan is included in the range
	//That way we never have two frame with overlapping timeranges
	FTimespan end = FTimespan::FromSeconds((iFrameIndex + 1) / GetFramesPerSecond()) - FTimespan(1);
	return TRange<FTimespan>(start, end);
}

/*
TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe>
UOdysseyAnimation::GetBlockAtIndex(int iIndex)
{
	if (iIndex < 0 || iIndex >= (int)GetFrameCount())
	{
		::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext((ULIS::eFormat)mFormat);
		TSharedPtr< ::ULIS::FBlock > block = MakeShared<::ULIS::FBlock>(mWidth, mHeight, (ULIS::eFormat)mFormat);
		ctx.Finish();

		return block;
	}

	return GetBlockFromId(mFrameIds[iIndex]);
}

TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe>
UOdysseyAnimation::GetBlockAtTime(FTimespan iTime)
{
	int frameIndex = GetFrameIndexAtTime(iTime);
	return GetBlockAtIndex(frameIndex);
}

TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe>
UOdysseyAnimation::GetBlockFromRenderImageIdComposition(const TArray<FGuid>& iIdComposition)
{
	if (!mFrameBlocks.Contains(iIdComposition))
		return nullptr;

	if ( mFrameBlocks[iIdComposition].mInvalidRects.Num() > 0 )
		GenerateFrameBlock(iIdComposition);

	return mFrameBlocks[iIdComposition].mRasterBlock->GetBlock();
}

void
UOdysseyAnimation::WaitForBlockUpdate(const TArray<FGuid>& iIdComposition)
{
	if ( !mFrameBlocks.Contains(iIdComposition) )
		return;

	if ( mFrameBlocks[iIdComposition].mInvalidRects.Num() > 0 )
		GenerateFrameBlock(iIdComposition);
} */


UOdysseyAnimationLayerStack*
UOdysseyAnimation::GetLayerStack() const
{
	return mLayerStack;
}

void
UOdysseyAnimation::Serialize(FArchive& Ar)
{
	/* Proxy Specific
	//Update frame blocks and Ids before any saving/loading
	if ( Ar.IsSaving() && Ar.IsPersistent() && !Ar.IsTransacting() )
	{
		UpdateFrameBlocks();
	} */

	//
	Super::Serialize(Ar);

	/* Proxy Specific

	int frameBlocksCount = mFrameBlocks.Num();
	Ar << frameBlocksCount;

	if ( Ar.IsSaving() )
	{
		for ( auto& element : mFrameBlocks )
		{
			FString& id = element.Key;
			FFrameBlock& frameBlock = element.Value;
			Ar << id;
			Ar << *frameBlock.mRasterBlock;
			Ar << frameBlock.mFrameIndexes;
		}
	}
	else if ( Ar.IsLoading() )
	{
		for ( int i = 0; i < frameBlocksCount; i++ )
		{
			FFrameBlock frameBlock;
			frameBlock.mRasterBlock = MakeShared<FOdysseyRasterBlock>(this);
			FString id;

			Ar << id;
			Ar << *frameBlock.mRasterBlock;
			Ar << frameBlock.mFrameIndexes;

			mFrameBlocks.Add(id, frameBlock);
		}
	} */

	//TODO: Save frame 

	//TODO: On Save (when asynchrounous):
	//TODO: Force cleanup of mFrameBlocks
	//TODO: Force block generation of mFrameBlocks (

}


void
UOdysseyAnimation::PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    if (PropertyChangedEvent.ChangeType & EPropertyChangeType::Interactive)
        return;

    PropertyChanged(PropertyChangedEvent.GetPropertyName());
}

void
UOdysseyAnimation::PostTransacted(const FTransactionObjectEvent& iTransactionEvent)
{
    Super::PostTransacted(iTransactionEvent);

    if ( iTransactionEvent.GetEventType() != ETransactionObjectEventType::UndoRedo )
        return;

    const TArray<FName>& changedPropertyNames = iTransactionEvent.GetChangedProperties();
    for ( const FName& propertyName : changedPropertyNames )
    {
        PropertyChanged(propertyName);
    }
}

void
UOdysseyAnimation::PostInitProperties()
{
	Super::PostInitProperties();
	
	SetAbility(MakeShared<FOdysseyAnimationImageRenderingAbility>(this));

	//IOdysseyAnimationImageRenderingAbility::OnDataChanged().AddUObject(this, &UOdysseyAnimation::OnImageRenderingDataChanged);
	//IOdysseyAnimationImageRenderingAbility::OnIdCompositionChanged().AddUObject(this, &UOdysseyAnimation::OnImageRenderingIdCompositionChanged);
}

void
UOdysseyAnimation::PropertyChanged(const FName& iPropertyName)
{
	if ( iPropertyName == "CurrentFrame" )
        CurrentFrameChanged();
    if ( iPropertyName == "FramesPerSecond" )
        FramesPerSecondChanged();
}

void
UOdysseyAnimation::CurrentFrameChanged()
{
	OnCurrentFrameChanged().Broadcast(this);
}

void
UOdysseyAnimation::FramesPerSecondChanged()
{
	OnFramesPerSecondChanged().Broadcast(this);
}

/* Events
 *****************************************************************************/
/* void
UOdysseyAnimation::Tick(float iDeltaTime)
{
	UpdateFrameBlocks();
}

void
UOdysseyAnimation::UpdateFrameBlocks()
{
	//Remove frameblocks that are not referenced by anyone
	TArray<TArray<FGuid>> idsToRemove;
	for ( auto& element : mFrameBlocks )
	{
		TArray<FGuid>& key = element.Key;
		FFrameBlock& frameBlock = element.Value;

		if ( frameBlock.mFrameIndexes.Num() <= 0 )
			idsToRemove.Add(key);
	}

	for ( const TArray<FGuid>& idToRemove : idsToRemove )
	{
		mFrameBlocks.Remove(idToRemove);
	}

	//Generate all frame blocks that needs it
	for ( auto& element : mFrameBlocks )
	{
		TArray<FGuid>& frameId = element.Key;
		FFrameBlock& frameBlock = element.Value;
		if ( frameBlock.mInvalidRects.Num() > 0 )
			GenerateFrameBlock(frameId);
	}
}

void
UOdysseyAnimation::GenerateFrameBlock(const TArray<FGuid>& iId)
{
	if (!mFrameBlocks.Contains(iId))
		return;

	FFrameBlock& frameBlock = mFrameBlocks[iId];

	if ( frameBlock.mFrameIndexes.Num() <= 0 || frameBlock.mInvalidRects.Num() <= 0 )
		return;

	::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext((ULIS::eFormat)mFormat);

	//Create the associated block if needed
	if ( !frameBlock.mRasterBlock )
		frameBlock.mRasterBlock = MakeShared<FOdysseyRasterBlock>(this);

	TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> block = frameBlock.mRasterBlock->GetBlock();
	::ULIS::FEvent eventClear;
	if (!block)
	{
		block = MakeShared<::ULIS::FBlock>(mWidth, mHeight, (ULIS::eFormat)mFormat);
		frameBlock.mRasterBlock->SetBlock(block);
		ctx.Clear(*block, ::ULIS::FRectI::Auto, ::ULIS::FSchedulePolicy::AsyncCacheEfficient, 0, nullptr, &eventClear);
	}
	else
	{
		eventClear = ::ULIS::FEvent::NoOP();
	}

	
    TSharedPtr<IOdysseyAnimationImageRenderingAbility> imageRenderAbility = GetAbility<IOdysseyAnimationImageRenderingAbility>();
    if (imageRenderAbility)
	{
		for (const ::ULIS::FRectI& rect : frameBlock.mInvalidRects)
		{
			imageRenderAbility->RenderImage(block, frameBlock.mFrameIndexes[0], rect, rect.Position(), {eventClear});
		}
	}

	ctx.Finish();
	frameBlock.mInvalidRects.Empty();
}

void
UOdysseyAnimation::OnImageRenderingDataChanged(const FOnRenderImageDataChangedEvent& iEvent)
{
	//TODO: Look for each IdComposition containg the changed Id
	//GetRects()
	//Then Invalid thos rects for the changed IdComposition
}

void
UOdysseyAnimation::OnImageRenderingIdCompositionChanged()
{
	//TODO: Find all frames to regenerate
	//Being able to know if it comes from something we are interested in, before checking would help with performances

	if (iLayerStack != mLayerStack)
		return;

	int startFrame = iRange.GetUpperBound().IsInclusive() ? iRange.GetLowerBoundValue() : iRange.GetLowerBoundValue() + 1;
	int endFrame = iRange.GetUpperBound().IsInclusive() ? iRange.GetUpperBoundValue() : iRange.GetUpperBoundValue() - 1;
	for (int i = startFrame; i <= endFrame; i++)
	{
		FString id = iLayerStack->GetFrameId(i);
		TArray<::ULIS::FRectI> invalidRects = iRects;

		//If there is no block corresponding to the given id, create an empty one
		if (!mFrameBlocks.Contains(id))
		{
			FFrameBlock frameBlock;
			frameBlock.mRasterBlock = MakeShared<FOdysseyRasterBlock>(this);
			frameBlock.mInvalidRects = { ::ULIS::FRectI::FromXYWH(0, 0, mWidth, mHeight) };
			mFrameBlocks.Add(id, frameBlock);
		}

		FFrameBlock& frameBlock = mFrameBlocks[id];

		//If we have no available frames at that index
		if (i >= mFrameIds.Num())
		{
			//Add new frame Id
			while ( i >= mFrameIds.Num() )
			{
				mFrameIds.Add(id); //We assume that intermediate frame ids will be sent in a future event, which will set it to the correct id
				frameBlock.mFrameIndexes.Add(mFrameIds.Num() - 1);
			}

			//Don't take invalidRects into account in this case
			//This is because in case a frame is added, iRects will contain the full block rect
			//But we don't actually to refresh it if the corresponding rasterblock already exist
			//Plus, if the corresponding rasterblock doesn't exist, it will fully rendered by GenerateFrame anyway
			invalidRects.Empty();
		}
		//If the id does not correspond to the frame we have at that index
		else if (mFrameIds[i] != id)
		{
			//Remove frame index from old frameblock
			FFrameBlock& oldFrameBlock = mFrameBlocks[mFrameIds[i]];
			oldFrameBlock.mFrameIndexes.Remove(i);

			//Set the correct frame Id
			mFrameIds[i] = id;
			frameBlock.mFrameIndexes.Add(i);

			//Don't take invalidRects into account in this case
			//This is because in case a frame is moved, iRects will contain the full block rect
			//But we don't actually to refresh it if the corresponding rasterblock already exist
			//Plus, if the corresponding rasterblock doesn't exist, it will fully rendered by GenerateFrame anyway
			invalidRects.Empty();
		}

		//Register what part of what frame changed (used in Tick())
		frameBlock.mInvalidRects.Append(invalidRects);
		frameBlock.mInvalidRects = OdysseyRectUtils::MergeRects(frameBlock.mInvalidRects);
	}

	OnRenderImageIdChanged().Broadcast(this);
} */


/* IMediaSource overrides
 *****************************************************************************/

bool UOdysseyAnimation::GetMediaOption(const FName& Key, bool DefaultValue) const
{
	/* if (Key == FileMediaSource::PrecacheFileOption)
	{
		return PrecacheFile;
	} */

	return Super::GetMediaOption(Key, DefaultValue);
}


bool UOdysseyAnimation::HasMediaOption(const FName& Key) const
{
	/* if (Key == FileMediaSource::PrecacheFileOption)
	{
		return true;
	} */

	return Super::HasMediaOption(Key);
}

/* UMediaSource overrides
 *****************************************************************************/

FString UOdysseyAnimation::GetUrl() const
{
	return FString(TEXT("odysseyanimation://")) + GetPathName();
}

bool UOdysseyAnimation::Validate() const
{
	return true;
}
