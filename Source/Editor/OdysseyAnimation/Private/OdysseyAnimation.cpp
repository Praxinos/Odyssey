// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyAnimation.h"

#include "LayerStack/OdysseyAnimationLayerImageRaster.h"
#include "LayerStack/OdysseyAnimationLayerStack.h"

#include "Misc/TransactionObjectEvent.h"

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

UOdysseyAnimation::FOnRenderImageChanged&
UOdysseyAnimation::OnRenderImageChanged()
{
    static FOnRenderImageChanged onRenderImageChanged;
    return onRenderImageChanged;
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
	return FTimespan::FromSeconds(GetFrameCount() / GetFramesPerSecond());
}

TRange<int>
UOdysseyAnimation::GetFrameRange() const
{
	return mLayerStack->GetFrameRange();
}

uint32
UOdysseyAnimation::GetFrameCount() const
{
	TRange<int> frameRange = mLayerStack->GetFrameRange();

	//TODO: deduce frame count from :
	// - startPoint / endPoint

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
	if (iTime < 0 || iTime >= GetDuration())
		return INDEX_NONE;
	
	return int(iTime.GetTotalSeconds() * GetFramesPerSecond());
}

TRange<FTimespan>
UOdysseyAnimation::GetFrameTimeRange(uint32 iFrameIndex) const
{
	//TODO:
	return TRange<FTimespan>(FTimespan::FromSeconds(iFrameIndex / GetFramesPerSecond()), FTimespan::FromSeconds((iFrameIndex + 1) / GetFramesPerSecond()));
}

TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe>
UOdysseyAnimation::GetBlockAtIndex(uint32 iIndex)
{
	if (iIndex < 0 || iIndex >= GetFrameCount())
		return nullptr;

	//TODO: GenerateFrame() if the rasterblock does not exist (will by done when asynchronous is being done)

	const FString& id = mFrameIds[iIndex];
	return mFrameBlocks[id].mRasterBlock->GetBlock();
}

UOdysseyAnimationLayerStack*
UOdysseyAnimation::GetLayerStack() const
{
	return mLayerStack;
}

TSharedPtr<IOdysseyHandle>
UOdysseyAnimation::Preload(int iFrame)
{
    return mLayerStack->Preload(iFrame);
}

void
UOdysseyAnimation::Serialize(FArchive& Ar)
{
	//Update frame blocks and Ids before any saving/loading
	if ( Ar.IsSaving() && Ar.IsPersistent() && !Ar.IsTransacting() )
	{
		UpdateFrameBlocks();
	}

	Super::Serialize(Ar);

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
	}

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
	UOdysseyAnimationLayerStack::OnRenderImageChanged().AddUObject(this, &UOdysseyAnimation::OnLayerStackRenderImageChanged);
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
void
UOdysseyAnimation::Tick(float iDeltaTime)
{
	UpdateFrameBlocks();
}

void
UOdysseyAnimation::UpdateFrameBlocks()
{
	//Remove frameblocks that are not referenced by anyone
	TArray<FString> idsToRemove;
	for ( auto& element : mFrameBlocks )
	{
		FString& frameId = element.Key;
		FFrameBlock& frameBlock = element.Value;

		if ( frameBlock.mFrameIndexes.Num() <= 0 )
			idsToRemove.Add(frameId);
	}

	for ( const FString& idToRemove : idsToRemove )
	{
		mFrameBlocks.Remove(idToRemove);
	}

	for ( auto& element : mFrameBlocks )
	{
		FString& frameId = element.Key;
		FFrameBlock& frameBlock = element.Value;
		if ( frameBlock.mInvalidRects.Num() > 0 )
			GenerateFrameBlock(frameId);
	}
}

void
UOdysseyAnimation::GenerateFrameBlock(const FString& iId)
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

	for (const ::ULIS::FRectI& rect : frameBlock.mInvalidRects)
	{
		mLayerStack->RenderImage(block, frameBlock.mFrameIndexes[0], rect, rect.Position(), {eventClear});
	}
	ctx.Finish();

	frameBlock.mInvalidRects.Empty();
}

void
UOdysseyAnimation::OnLayerStackRenderImageChanged(UOdysseyAnimationLayerStack* iLayerStack, const TRange<int>& iRange, const TArray<::ULIS::FRectI>& iRects, bool iIsInteractive)
{
	//TODO: whould we do all this if iIsInteractive is true ?

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

	OnRenderImageChanged().Broadcast(this, iRange, iRects, iIsInteractive);
}


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