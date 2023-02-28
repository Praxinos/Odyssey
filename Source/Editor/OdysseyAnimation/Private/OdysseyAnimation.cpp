// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyAnimation.h"

#include <ULIS>
#include "ULISLoaderModule.h"

void UOdysseyAnimation::Init(const FOdysseyAnimationConfiguration& iConfiguration)
{
	mWidth = iConfiguration.Width;
	mHeight = iConfiguration.Height;
	mFormat = iConfiguration.ULISFormat();
	mFramesPerSecond = iConfiguration.FramesPerSecond;

	::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(iConfiguration.ULISFormat());
	for(int i = 0; i < 10; i++)
	{
		TSharedPtr<FOdysseyRasterBlock> rasterBlock = MakeShared<FOdysseyRasterBlock>();
		TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> block = MakeShared<::ULIS::FBlock>(iConfiguration.Width, iConfiguration.Height, iConfiguration.ULISFormat());

        ctx.Fill(
			*block
			, ::ULIS::FColor::HSVA8(i*20, 255, 255)
		);

		ctx.Finish();

    	rasterBlock->SetBlock(block);
		mRasterBlocks.Add(rasterBlock);
	}

	mLayerStack = NewObject<UOdysseyAnimationLayerStack>(this, "LayerStack", RF_Public | RF_Transactional);
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

uint32
UOdysseyAnimation::GetFrameCount() const
{
	return mRasterBlocks.Num();
}

double
UOdysseyAnimation::GetFramesPerSecond() const
{
	return mFramesPerSecond;
}

uint32
UOdysseyAnimation::GetFrameIndexAtTime(FTimespan iTime) const
{
	if (iTime < 0 || iTime > GetDuration())
		return INDEX_NONE;
	
	return uint32(iTime.GetTotalSeconds() * GetFramesPerSecond());
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
	if (iIndex < 0 || iIndex >= uint32(mRasterBlocks.Num()))
		return nullptr;

	return mRasterBlocks[iIndex]->GetBlock();
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
	Super::Serialize(Ar);

	Ar << mRasterBlocks;
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