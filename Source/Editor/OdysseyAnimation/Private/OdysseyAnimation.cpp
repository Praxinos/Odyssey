// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyAnimation.h"

#include <ULIS>
#include "ULISLoaderModule.h"

void UOdysseyAnimation::Init(const FOdysseyAnimationConfiguration& iConfiguration)
{
	mWidth = iConfiguration.Width;
	mHeight = iConfiguration.Height;
	mFormat = iConfiguration.Format;
	mFramesPerSecond = iConfiguration.FramesPerSecond;

	::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(iConfiguration.ULISFormat());
	for(int i = 0; i < 10; i++)
	{
		::ULIS::FBlock* block = new ::ULIS::FBlock(iConfiguration.Width, iConfiguration.Height, iConfiguration.ULISFormat());

		::ULIS::FWString text = ::ULIS::FWString::FromInt(i);
        ctx.Fill(
			*block
			, ::ULIS::FColor::HSVA8(i*20, 255, 255)
		);

		mBlocks.Add(block);
	}
	
    ctx.Finish();
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
	return mBlocks.Num();
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

::ULIS::FBlock*
UOdysseyAnimation::GetBlockAtIndex(uint32 iIndex)
{
	if (iIndex < 0 || iIndex >= uint32(mBlocks.Num()))
		return nullptr;

	return mBlocks[iIndex];
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


/* UObject overrides
******************************************************************************/

/* void
UOdysseyAnimation::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);

	if (Ar.IsSaving())
	{
		int32 numBulk = mBulkDatas.Num();
		Ar << numBulk;
		for (auto& Elem : mBulkDatas)
		{
			Ar << Elem.Key;
			Elem.Value.Serialize(Ar);
		}
	}
	else if (Ar.IsLoading())
	{
		int32 numBulk = 0;
		Ar << numBulk;
		for (int32 = 0; i < numBulk; i++)
		{
			FGuid guid;
			UE::Serialization::FEditorBulkData;

			Ar << guid;
			bulkData.Emplace(guid);
			bulkData[guid].Serialize(Ar);
		}
	}
} */

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