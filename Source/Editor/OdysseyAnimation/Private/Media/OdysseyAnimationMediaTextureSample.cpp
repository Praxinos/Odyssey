// Copyright Epic Games, Inc. All Rights Reserved.

#include "Media/OdysseyAnimationMediaTextureSample.h"
#include "Engine/Texture2DDynamic.h"
#include "OdysseyRectUtils.h"

FOdysseyAnimationMediaTextureSample::~FOdysseyAnimationMediaTextureSample()
{
    mAnimation->OnRenderImageChanged().RemoveAll(this);
}

FOdysseyAnimationMediaTextureSample::FOdysseyAnimationMediaTextureSample(UOdysseyAnimation* iAnimation)
    : mAnimation(iAnimation)
    , mFrameIndex(INDEX_NONE)
    , mSequenceIndex(0)
    , mTime(0)
    , mDuration(0)
    , mTexture1(UTexture2DDynamic::Create(iAnimation->Width(), iAnimation->Height(), FTexture2DDynamicCreateInfo(PF_B8G8R8A8)))
    , mTexture2(UTexture2DDynamic::Create(iAnimation->Width(), iAnimation->Height(), FTexture2DDynamicCreateInfo(PF_B8G8R8A8)))
    , mCurrentTexture(false)
{
    mAnimation->OnRenderImageChanged().AddRaw(this, &FOdysseyAnimationMediaTextureSample::OnRenderImageChanged);
}

void
FOdysseyAnimationMediaTextureSample::Update(int iFrameIndex, uint32 iSequenceIndex)
{
    if ( mFrameIndex == iFrameIndex && mSequenceIndex == iSequenceIndex )
        return;

    mFrameIndex = iFrameIndex;
    mSequenceIndex = iSequenceIndex;


    TRange<FTimespan> timeRange = mAnimation->GetFrameTimeRange(iFrameIndex);
    mTime = FMediaTimeStamp(timeRange.GetLowerBoundValue(), iSequenceIndex);
    mDuration = timeRange.Size<FTimespan>();

    if ( mFrameIndex < 0 )
        return;

    CopyRects({ ::ULIS::FRectI::FromXYWH(0, 0, mAnimation->Width(), mAnimation->Height())});
}

void
FOdysseyAnimationMediaTextureSample::CopyRects(const TArray<::ULIS::FRectI>& iRects)
{
    if ( mFrameIndex < 0 )
        return;

    TSharedPtr<::ULIS::FBlock> srcBlock = mAnimation->GetBlockAtIndex(mFrameIndex);
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(srcBlock->Format());
    ENQUEUE_RENDER_COMMAND(FWriteRawDataToTexture)(
        [this, srcBlock, iRects](FRHICommandListImmediate& RHICmdList)
        {
            FTexture2DDynamicResource* resource1 = static_cast<FTexture2DDynamicResource*>(mTexture1->GetResource());
            if ( !resource1 )
                return;

            FTexture2DDynamicResource* resource2 = static_cast<FTexture2DDynamicResource*>(mTexture2->GetResource());
            if ( !resource2 )
                return;

            CopyRects_RenderThread(resource1, srcBlock, iRects);
            CopyRects_RenderThread(resource2, srcBlock, iRects);
        }
    );

    FRenderCommandFence fence1;
    fence1.BeginFence();
    fence1.Wait();

    ctx.Finish();

    ENQUEUE_RENDER_COMMAND(FWriteRawDataToTexture2)(
        [this](FRHICommandListImmediate& RHICmdList)
        {
            FTexture2DDynamicResource* resource1 = static_cast<FTexture2DDynamicResource*>(mTexture1->GetResource());
            if ( !resource1 )
                return;

            FTexture2DDynamicResource* resource2 = static_cast<FTexture2DDynamicResource*>(mTexture2->GetResource());
            if ( !resource2 )
                return;

            FTexture2DRHIRef rhi1 = resource1->GetTexture2DRHI();
            FTexture2DRHIRef rhi2 = resource2->GetTexture2DRHI();
            RHIUnlockTexture2D(rhi1, 0, false, false);
            RHIUnlockTexture2D(rhi2, 0, false, false);
        }
    );
    
    FRenderCommandFence fence;
    fence.BeginFence();
    fence.Wait();
}

void
FOdysseyAnimationMediaTextureSample::CopyRects_RenderThread(FTexture2DDynamicResource* iResource, TSharedPtr<::ULIS::FBlock> iSrc, const TArray<::ULIS::FRectI>& iRects)
{
    check(IsInRenderingThread());

    FTexture2DRHIRef rhi = iResource->GetTexture2DRHI();

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

    //delay rects update to tick
    mInvalidRects.Append(iRects);
    mInvalidRects = OdysseyRectUtils::MergeRects(mInvalidRects);

	//CopyRects(iRects);
}

void
FOdysseyAnimationMediaTextureSample::Tick(float DeltaTime)
{
    if ( mInvalidRects.IsEmpty() )
        return;

    CopyRects(mInvalidRects);
    mInvalidRects.Empty();
}