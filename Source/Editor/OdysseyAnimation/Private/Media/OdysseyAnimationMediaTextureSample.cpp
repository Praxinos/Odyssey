// Copyright Epic Games, Inc. All Rights Reserved.

#include "Media/OdysseyAnimationMediaTextureSample.h"

FOdysseyAnimationMediaTextureSample::FOdysseyAnimationMediaTextureSample(UOdysseyAnimation* iAnimation, uint32 iFrameIndex)
    : mTime(0)
    , mDuration(0)
    , mBlock(iAnimation->GetBlockAtIndex(iFrameIndex))
{
    TRange<FTimespan> timeRange = iAnimation->GetFrameTimeRange(iFrameIndex);
    mTime = FMediaTimeStamp(timeRange.GetLowerBoundValue());
    mDuration = timeRange.GetUpperBoundValue() - timeRange.GetLowerBoundValue();
}

const void*
FOdysseyAnimationMediaTextureSample::GetBuffer()
{
    return mBlock->Bits();
}

FIntPoint
FOdysseyAnimationMediaTextureSample::GetDim() const
{
    return FIntPoint(mBlock->Width(), mBlock->Height());
}

FTimespan
FOdysseyAnimationMediaTextureSample::GetDuration() const
{
    //FTimespan(0, 0, 0, 0, 500000000) //0.5 seconds
    return mDuration; 
}

EMediaTextureSampleFormat
FOdysseyAnimationMediaTextureSample::GetFormat() const
{
    switch(mBlock->Format())
    {
        case ::ULIS::Format_BGRA8:
            return EMediaTextureSampleFormat::CharBGRA; //BGRA8        
        break;

        case ::ULIS::Format_RGBAF:
            return EMediaTextureSampleFormat::FloatRGBA; //RGBAF    
        break;
    }

    check(false);
    return EMediaTextureSampleFormat::CharBGRA; //BGRA8
}

FIntPoint
FOdysseyAnimationMediaTextureSample::GetOutputDim() const
{
    return FIntPoint(mBlock->Width(), mBlock->Height());
}

uint32
FOdysseyAnimationMediaTextureSample::GetStride() const
{
    return mBlock->BytesPerScanLine();
}


#if WITH_ENGINE

FRHITexture*
FOdysseyAnimationMediaTextureSample::GetTexture() const
{
    return nullptr;
}


#endif //WITH_ENGINE

FMediaTimeStamp
FOdysseyAnimationMediaTextureSample::GetTime() const
{
	//FTimespan frameDuration(0, 0, 0, 0, 500000000); //0.5 seconds
    return mTime;
}

bool
FOdysseyAnimationMediaTextureSample::IsCacheable() const
{
    return true; //should return true when caching will be made
}

bool
FOdysseyAnimationMediaTextureSample::IsOutputSrgb() const
{
    return false;
}
