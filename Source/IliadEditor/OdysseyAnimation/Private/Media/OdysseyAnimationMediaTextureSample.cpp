// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Media/OdysseyAnimationMediaTextureSample.h"
#include "Engine/Texture2D.h"
#include "Rendering/Texture2DResource.h"
#include "OdysseyRectUtils.h"

FOdysseyAnimationMediaTextureSample::~FOdysseyAnimationMediaTextureSample()
{
}

FOdysseyAnimationMediaTextureSample::FOdysseyAnimationMediaTextureSample(int iWidth, int iHeight, UTexture2D* iTexture)
    : mDimensions(iWidth, iHeight)
    , mTime(0)
    , mDuration(0)
    , mTexture(iTexture)
    //, mTexture2(iTexture2)
    //, mCurrentTexture(false)
    , mConverter(this)
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
    FTexture2DResource* resource = static_cast<FTexture2DResource*>(mTexture->GetResource());
    if (!resource)
        return nullptr;

    /* FTexture2DResource* resource2 = static_cast<FTexture2DResource*>(mTexture2->GetResource());
    if (!resource2)
        return nullptr; */

    //mCurrentTexture = !mCurrentTexture;
    return resource->GetTexture2DRHI();//mCurrentTexture ? resource1->GetTexture2DRHI() : resource2->GetTexture2DRHI();
}

#endif //WITH_ENGINE

IMediaTextureSampleConverter*
FOdysseyAnimationMediaTextureSample::GetMediaTextureSampleConverter()
{
    return &mConverter;
}


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
