// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyImageRenderer.h"
#include "ULISEventBuilder.h"
#include "ULISLoaderModule.h"
#include "ULISUtils.h"

IOdysseyImageRenderer::IOdysseyImageRenderer(IOdysseyImageRenderer::eRenderType iRenderType, const TArray<::ULIS::FRectI>& iDefaultRects)
    : mRenderType(iRenderType)
    , mDefaultRects(iDefaultRects)
{
}

void
IOdysseyImageRenderer::Init()
{
}

void
IOdysseyImageRenderer::Lock()
{
}

void
IOdysseyImageRenderer::Unlock()
{
}

bool
IOdysseyImageRenderer::IsGameThreadOnly()
{
    return false;
}
/*
TArray<::ULIS::FEvent>
IOdysseyImageRenderer::Blend(TSharedPtr<::ULIS::FBlock> ioBlock, ::ULIS::eBlendMode iBlendMode, float iOpacity, const TArray<::ULIS::FRectI>& iRects, const TArray<::ULIS::FEvent>& iWaitList)
{
    TArray<::ULIS::FVec2I> pos;
    for (const ::ULIS::FRectI& rect : iRects)
        pos.Add(rect.Position());

    return Blend(ioBlock, iBlendMode, iOpacity, iRects, pos, iWaitList);
}

TArray<::ULIS::FEvent>
IOdysseyImageRenderer::Blend(TSharedPtr<::ULIS::FBlock> ioBlock, ::ULIS::eBlendMode iBlendMode, float iOpacity, const ::ULIS::FRectI& iRect, const ::ULIS::FVec2I& iPos, const TArray<::ULIS::FEvent>& iWaitList)
{
    TArray<::ULIS::FRectI> rects = { iRect };
    TArray<::ULIS::FVec2I> pos = { iPos };
    return Blend(ioBlock, iBlendMode, iOpacity, rects, pos, iWaitList);
}

TArray<::ULIS::FEvent>
IOdysseyImageRenderer::Blend(TSharedPtr<::ULIS::FBlock> ioBlock, ::ULIS::eBlendMode iBlendMode, float iOpacity, const ::ULIS::FRectI& iRect, const TArray<::ULIS::FEvent>& iWaitList)
{
    TArray<::ULIS::FRectI> rects = { iRect };
    TArray<::ULIS::FVec2I> pos = { iRect.Position() };
    return Blend(ioBlock, iBlendMode, iOpacity, rects, pos, iWaitList);
}

TArray<::ULIS::FEvent>
IOdysseyImageRenderer::Blend(TSharedPtr<::ULIS::FBlock> ioBlock, ::ULIS::eBlendMode iBlendMode, float iOpacity, const TArray<::ULIS::FEvent>& iWaitList)
{
    return Blend(ioBlock, iBlendMode, iOpacity, mDefaultRects, iWaitList);
}*/
/*
TArray<::ULIS::FEvent>
IOdysseyImageRenderer::Copy(TSharedPtr<::ULIS::FBlock> ioBlock, const TArray<::ULIS::FRectI>& iRects, const TArray<::ULIS::FEvent>& iWaitList)
{
    TArray<::ULIS::FVec2I> pos;
    for (const ::ULIS::FRectI& rect : iRects)
        pos.Add(rect.Position());

    return Copy(ioBlock, iRects, pos, iWaitList);
}

TArray<::ULIS::FEvent>
IOdysseyImageRenderer::Copy(TSharedPtr<::ULIS::FBlock> ioBlock, const ::ULIS::FRectI& iRect, const ::ULIS::FVec2I& iPos, const TArray<::ULIS::FEvent>& iWaitList)
{
    TArray<::ULIS::FRectI> rects = { iRect };
    TArray<::ULIS::FVec2I> pos = { iPos };
    return Copy(ioBlock, rects, pos, iWaitList);
}

TArray<::ULIS::FEvent>
IOdysseyImageRenderer::Copy(TSharedPtr<::ULIS::FBlock> ioBlock, const ::ULIS::FRectI& iRect, const TArray<::ULIS::FEvent>& iWaitList)
{
    TArray<::ULIS::FRectI> rects = { iRect };
    TArray<::ULIS::FVec2I> pos = { iRect.Position() };
    return Copy(ioBlock, rects, pos, iWaitList);
}

TArray<::ULIS::FEvent>
IOdysseyImageRenderer::Copy(TSharedPtr<::ULIS::FBlock> ioBlock, const TArray<::ULIS::FEvent>& iWaitList)
{
    return Copy(ioBlock, mDefaultRects, iWaitList);
} */

IOdysseyImageRenderer::eRenderType
IOdysseyImageRenderer::GetRenderType() const
{
    return mRenderType;
}

TArray<::ULIS::FEvent>
IOdysseyImageRenderer::ConvertAndBlend(TSharedPtr<::ULIS::FBlock> iFront, const ::ULIS::FVec2I& iFrontOffset, const FOdysseyImageRendererBlendParams& iParams, const TArray<::ULIS::FEvent>& iWaitList)
{
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(iFront->Format());

    TArray<::ULIS::FEvent> events;
    ::ULIS::FVec2I pos = iParams.mPos - iFrontOffset;
    for ( int i = 0; i < iParams.mRects.Num(); i++ )
    {
        if (iParams.mTransform == ::ULIS::FMat3F())
        {
            ::ULIS::FRectI srcRect = ::ULIS::FRectI::FromXYWH(iParams.mRects[i].x + pos.x, iParams.mRects[i].y + pos.y, iParams.mRects[i].w, iParams.mRects[i].h);
            ::ULIS::FVec2I dstPos(iParams.mRects[i].x, iParams.mRects[i].y);
            TArray<::ULIS::FEvent> eventConvertAndExecute = ULISUtils::ConvertAndExecute(iParams.mBlock, iFront->Format(), srcRect, dstPos, iWaitList,
                [&ctx, iFront, iParams](TSharedPtr<::ULIS::FBlock> ioDest, const ::ULIS::FRectI& iRect, const ::ULIS::FVec2I& iPos, const TArray<::ULIS::FEvent>& iWaitList) -> TArray<::ULIS::FEvent>
                {
                    ::ULIS::FEvent eventBlend = FULISEventBuilder().RetainBlock(iFront).RetainBlock(ioDest).Build();
                    ctx.Blend(
                        *iFront,
                        *ioDest,
                        iRect,
                        iPos,
                        iParams.mBlendMode,
                        ::ULIS::Alpha_Normal,
                        iParams.mOpacity,
                        ::ULIS::FSchedulePolicy::AsyncCacheEfficient,
                        iWaitList.Num(),
                        iWaitList.GetData(),
                        &eventBlend
                    );
                    return { eventBlend };
                }
            );
            events.Append(eventConvertAndExecute);
            ctx.Flush();
        }
        else
        {
            TSharedPtr<::ULIS::FBlock> srcBlock = MakeShared<::ULIS::FBlock>(iParams.mRects[i].w, iParams.mRects[i].h, iFront->Format());
            ::ULIS::FEvent eventClear = FULISEventBuilder().RetainBlock(srcBlock).Build();
            ctx.Clear(*srcBlock, ::ULIS::FRectI::Auto, ::ULIS::FSchedulePolicy::AsyncCacheEfficient, iWaitList.Num(), iWaitList.GetData(), &eventClear);

            ::ULIS::FRectI dstRect = ::ULIS::FRectI::FromXYWH(iParams.mRects[i].x + pos.x, iParams.mRects[i].y + pos.y, iParams.mRects[i].w, iParams.mRects[i].h);

            ::ULIS::FEvent eventTransform = FULISEventBuilder().RetainBlock(iFront).RetainBlock(srcBlock).Build();
            ctx.TransformAffine(
                *iFront,
                *srcBlock,
                ::ULIS::FRectI::Auto,
                ::ULIS::FMat3F::MakeTranslationMatrix(-pos.x, -pos.y) * iParams.mTransform,
                ::ULIS::eResamplingMethod::Resampling_Bilinear,
                ::ULIS::eBorderMode::Border_Transparent,
                ::ULIS::FColor::Transparent,
                ::ULIS::FSchedulePolicy::AsyncCacheEfficient,
                1,
                &eventClear,
                &eventTransform
            );
            
            ::ULIS::FVec2I dstPos(iParams.mRects[i].x, iParams.mRects[i].y);
            TArray<::ULIS::FEvent> eventConvertAndExecute = ULISUtils::ConvertAndExecute(iParams.mBlock, iFront->Format(), ::ULIS::FRectI::Auto, dstPos, { eventTransform },
                [&ctx, srcBlock, iParams](TSharedPtr<::ULIS::FBlock> ioDest, const ::ULIS::FRectI& iRect, const ::ULIS::FVec2I& iPos, const TArray<::ULIS::FEvent>& iWaitList) -> TArray<::ULIS::FEvent>
                {
                    ::ULIS::FEvent eventBlend = FULISEventBuilder().RetainBlock(srcBlock).RetainBlock(ioDest).Build();
                    ctx.Blend(
                        *srcBlock,
                        *ioDest,
                        iRect,
                        iPos,
                        iParams.mBlendMode,
                        ::ULIS::Alpha_Normal,
                        iParams.mOpacity,
                        ::ULIS::FSchedulePolicy::AsyncCacheEfficient,
                        iWaitList.Num(),
                        iWaitList.GetData(),
                        &eventBlend
                    );
                    return { eventBlend };
                }
            );
            events.Append(eventConvertAndExecute);
            ctx.Flush();
        }
    }

    return events;
}

TArray<::ULIS::FEvent>
IOdysseyImageRenderer::ConvertAndCopy(TSharedPtr<::ULIS::FBlock> iSrc, const ::ULIS::FVec2I& iSrcOffset, const FOdysseyImageRendererCopyParams& iParams, const TArray<::ULIS::FEvent>& iWaitList)
{
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(iSrc->Format());
    TArray<::ULIS::FEvent> events;
    ::ULIS::FVec2I pos = iParams.mPos - iSrcOffset;
    for ( int i = 0; i < iParams.mRects.Num(); i++ )
    {
        if (iParams.mTransform == ::ULIS::FMat3F())
        {
            ::ULIS::FEvent eventCopy = FULISEventBuilder().RetainBlock(iSrc).RetainBlock(iParams.mBlock).Build();
            ::ULIS::FRectI srcRect = ::ULIS::FRectI::FromXYWH(iParams.mRects[i].x + pos.x, iParams.mRects[i].y + pos.y, iParams.mRects[i].w, iParams.mRects[i].h);
            ::ULIS::FVec2I dstPos(iParams.mRects[i].x, iParams.mRects[i].y);
            ctx.ConvertFormat(
                *iSrc,
                *iParams.mBlock,
                srcRect,
                dstPos,
                ::ULIS::FSchedulePolicy::AsyncCacheEfficient,
                iWaitList.Num(), iWaitList.GetData(), &eventCopy);
            events.Add(eventCopy);
            ctx.Flush();
        }
        else
        {
            TSharedPtr<::ULIS::FBlock> srcBlock = MakeShared<::ULIS::FBlock>(iParams.mRects[i].w, iParams.mRects[i].h, iSrc->Format());
            ::ULIS::FEvent eventClear = FULISEventBuilder().RetainBlock(srcBlock).Build();
            ctx.Clear(*srcBlock, ::ULIS::FRectI::Auto, ::ULIS::FSchedulePolicy::AsyncCacheEfficient, iWaitList.Num(), iWaitList.GetData(), &eventClear);

            ::ULIS::FRectI dstRect = ::ULIS::FRectI::FromXYWH(iParams.mRects[i].x + pos.x, iParams.mRects[i].y + pos.y, iParams.mRects[i].w, iParams.mRects[i].h);

            ::ULIS::FEvent eventTransform = FULISEventBuilder().RetainBlock(iSrc).RetainBlock(srcBlock).Build();
            ctx.TransformAffine(
                *iSrc,
                *srcBlock,
                ::ULIS::FRectI::Auto,
                ::ULIS::FMat3F::MakeTranslationMatrix(-pos.x, -pos.y) * iParams.mTransform,
                ::ULIS::eResamplingMethod::Resampling_Bilinear,
                ::ULIS::eBorderMode::Border_Transparent,
                ::ULIS::FColor::Transparent,
                ::ULIS::FSchedulePolicy::AsyncCacheEfficient,
                1,
                &eventClear,
                &eventTransform
            );
            
            ::ULIS::FVec2I copyPos(iParams.mRects[i].x, iParams.mRects[i].y);

            ::ULIS::FEvent eventCopy = FULISEventBuilder().RetainBlock(srcBlock).RetainBlock(iParams.mBlock).Build();
            ctx.ConvertFormat(
                *srcBlock,
                *iParams.mBlock,
                ::ULIS::FRectI::Auto,
                copyPos,
                ::ULIS::FSchedulePolicy::AsyncCacheEfficient,
                1,
                &eventTransform,
                &eventCopy
            );
            events.Add(eventCopy);
            ctx.Flush();
        }
    }
    return events;
}

TArray<::ULIS::FEvent>
IOdysseyImageRenderer::Clear(TSharedPtr<::ULIS::FBlock> ioBlock, const TArray<::ULIS::FRectI>& iRects, const TArray<::ULIS::FEvent>& iWaitList)
{
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(ioBlock->Format());
    TArray<::ULIS::FEvent> events;
    for ( int i = 0; i < iRects.Num(); i++ )
    {
        ::ULIS::FEvent eventClear = FULISEventBuilder().RetainBlock(ioBlock).Build();
        ::ULIS::FRectI rect = iRects[i];
        ctx.Clear(*ioBlock, rect, ::ULIS::FSchedulePolicy::AsyncCacheEfficient, iWaitList.Num(), iWaitList.GetData(), &eventClear);
        events.Add(eventClear);
        ctx.Flush();
    }

    return events;
}