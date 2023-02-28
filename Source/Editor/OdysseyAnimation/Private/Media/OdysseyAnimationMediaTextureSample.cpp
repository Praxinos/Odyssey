// Copyright Epic Games, Inc. All Rights Reserved.

#include "Media/OdysseyAnimationMediaTextureSample.h"
#include "Engine/Texture2DDynamic.h"

FOdysseyAnimationMediaTextureSample::~FOdysseyAnimationMediaTextureSample()
{
    mAnimation->OnRenderImageChanged().RemoveAll(this);
}

FOdysseyAnimationMediaTextureSample::FOdysseyAnimationMediaTextureSample(UOdysseyAnimation* iAnimation)
    : mAnimation(iAnimation)
    , mIsValid(false)
    , mFrameIndex(0)
    , mSequenceIndex(0)
    , mTime(0)
    , mDuration(0)
    , mTexture(UTexture2DDynamic::Create(iAnimation->Width(), iAnimation->Height(), FTexture2DDynamicCreateInfo(PF_B8G8R8A8)))
{
    mAnimation->OnRenderImageChanged().AddRaw(this, &FOdysseyAnimationMediaTextureSample::OnRenderImageChanged);
}

void
FOdysseyAnimationMediaTextureSample::Update(uint32 iFrameIndex, uint32 iSequenceIndex)
{
    if ( mIsValid && mFrameIndex == iFrameIndex && mSequenceIndex == iSequenceIndex )
        return;

    mFrameIndex = iFrameIndex;
    mSequenceIndex = iSequenceIndex;
    TRange<FTimespan> timeRange = mAnimation->GetFrameTimeRange(iFrameIndex);
    mTime = FMediaTimeStamp(timeRange.GetLowerBoundValue(), iSequenceIndex);
    mDuration = timeRange.GetUpperBoundValue() - timeRange.GetLowerBoundValue();
    CopyRects({ ::ULIS::FRectI::FromXYWH(0, 0, mAnimation->Width(), mAnimation->Height())});
    mIsValid = true;
}

void
FOdysseyAnimationMediaTextureSample::CopyRects(const TArray<::ULIS::FRectI>& iRects)
{
    TSharedPtr<::ULIS::FBlock> srcBlock = mAnimation->GetBlockAtIndex(mFrameIndex);
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(srcBlock->Format());
    ENQUEUE_RENDER_COMMAND(FWriteRawDataToTexture)(
        [this, srcBlock, iRects](FRHICommandListImmediate& RHICmdList)
        {
            CopyRects_RenderThread(srcBlock, iRects);
        }
    );

    FRenderCommandFence fence1;
    fence1.BeginFence();
    fence1.Wait();

    ctx.Finish();

    ENQUEUE_RENDER_COMMAND(FWriteRawDataToTexture2)(
        [this](FRHICommandListImmediate& RHICmdList)
        {
            FTexture2DDynamicResource* resource = static_cast<FTexture2DDynamicResource*>(mTexture->GetResource());
            if ( !resource )
                return;

            FTexture2DRHIRef rhi = resource->GetTexture2DRHI();
            RHIUnlockTexture2D(rhi, 0, false, false);
        }
    );
    
    FRenderCommandFence fence;
    fence.BeginFence();
    fence.Wait();
}

void
FOdysseyAnimationMediaTextureSample::CopyRects_RenderThread(TSharedPtr<::ULIS::FBlock> iSrc, const TArray<::ULIS::FRectI>& iRects)
{
    check(IsInRenderingThread());
    FTexture2DDynamicResource* resource = static_cast<FTexture2DDynamicResource*>(mTexture->GetResource());
    if (!resource)
        return;

    FTexture2DRHIRef rhi = resource->GetTexture2DRHI();

	const int32 w = rhi->GetSizeX();
	const int32 h = rhi->GetSizeY();

    //8bits version
	uint32 stride = 0;
	uint8* data = reinterpret_cast<uint8*>(RHILockTexture2D(rhi, 0, RLM_WriteOnly, stride, false, false));
    TSharedPtr<::ULIS::FBlock> dstBlock = MakeShared<::ULIS::FBlock>(data, w, h, ::ULIS::eFormat::Format_BGRA8);

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(iSrc->Format());
    /*
        ES: copying rects only causes flickering, I don't know exactly why
        Probably "data" isn't initialized and contains garbage, because it assumes we will copy the whole texture ?
    
    for (const ::ULIS::FRectI& rect : iRects)
    {   
        ::ULIS::FEvent eventConvert = FULISEventBuilder().RetainBlock(dstBlock).Build();
        ctx.ConvertFormat(
            *iSrc,
            *dstBlock,
            rect,
            rect.Position(),
            ::ULIS::FSchedulePolicy::AsyncCacheEfficient,
            0, 
            nullptr,
            &eventConvert
        );
    }

    */

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

const void*
FOdysseyAnimationMediaTextureSample::GetBuffer()
{
    return nullptr;
}

FIntPoint
FOdysseyAnimationMediaTextureSample::GetDim() const
{
    return FIntPoint(mAnimation->Width(), mAnimation->Height());
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
    return EMediaTextureSampleFormat::CharBGRA; //BGRA8
}

FIntPoint
FOdysseyAnimationMediaTextureSample::GetOutputDim() const
{
    return FIntPoint(mAnimation->Width(), mAnimation->Height());
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
    FTexture2DDynamicResource* resource = static_cast<FTexture2DDynamicResource*>(mTexture->GetResource());
    if (!resource)
        return nullptr;

    return resource->GetTexture2DRHI();
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

void
FOdysseyAnimationMediaTextureSample::OnRenderImageChanged(UOdysseyAnimation* iAnimation, const TRange<int>& iRange, const TArray<::ULIS::FRectI>& iRects, bool iIsInteractive)
{
	if (iAnimation != mAnimation || !iRange.Contains(mFrameIndex))
		return;

	CopyRects(iRects);
}