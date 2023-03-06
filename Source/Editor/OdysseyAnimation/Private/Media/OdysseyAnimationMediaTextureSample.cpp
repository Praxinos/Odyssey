// Copyright Epic Games, Inc. All Rights Reserved.

#include "Media/OdysseyAnimationMediaTextureSample.h"
#include "Engine/Texture2DDynamic.h"
#include "OdysseyRectUtils.h"

FOdysseyAnimationMediaTextureSample::~FOdysseyAnimationMediaTextureSample()
{
}

FOdysseyAnimationMediaTextureSample::FOdysseyAnimationMediaTextureSample(int iWidth, int iHeight, UTexture2DDynamic* iTexture1, UTexture2DDynamic* iTexture2)
    : mDimensions(iWidth, iHeight)
    , mTime(0)
    , mDuration(0)
    , mTexture1(iTexture1)
    , mTexture2(iTexture2)
    , mCurrentTexture(false)
{
}

void
FOdysseyAnimationMediaTextureSample::SetTime(FMediaTimeStamp iTime)
{
    mTime = iTime;
}

void
FOdysseyAnimationMediaTextureSample::SetDuration(FTimespan iDuration)
{
    mDuration = iDuration;
}

const void*
FOdysseyAnimationMediaTextureSample::GetBuffer()
{
    return nullptr;
}

FIntPoint
FOdysseyAnimationMediaTextureSample::GetDim() const
{
    return mDimensions;
}

FTimespan
FOdysseyAnimationMediaTextureSample::GetDuration() const
{
    return mDuration; 
}

EMediaTextureSampleFormat
FOdysseyAnimationMediaTextureSample::GetFormat() const
{
    return EMediaTextureSampleFormat::CharBGRA; //BGRA8
}

FIntPoint
FOdysseyAnimationMediaTextureSample::GetOutputDim() const
{
    return mDimensions;
}

uint32
FOdysseyAnimationMediaTextureSample::GetStride() const
{
    return 0;
}


#if WITH_ENGINE

FRHITexture*
FOdysseyAnimationMediaTextureSample::GetTexture() const
{
    FTexture2DDynamicResource* resource1 = static_cast<FTexture2DDynamicResource*>(mTexture1->GetResource());
    if (!resource1)
        return nullptr;

    FTexture2DDynamicResource* resource2 = static_cast<FTexture2DDynamicResource*>(mTexture2->GetResource());
    if (!resource2)
        return nullptr;

    mCurrentTexture = !mCurrentTexture;
    return mCurrentTexture ? resource1->GetTexture2DRHI() : resource2->GetTexture2DRHI();
}

#endif //WITH_ENGINE

FMediaTimeStamp
FOdysseyAnimationMediaTextureSample::GetTime() const
{
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

