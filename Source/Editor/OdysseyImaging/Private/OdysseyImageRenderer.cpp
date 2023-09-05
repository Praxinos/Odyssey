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
}

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
}

/* TSharedPtr<::ULIS::FBlock>
IOdysseyImageRenderer::CopyInNewBlock(::ULIS::eFormat iFormat, const ::ULIS::FRectI& iRect, TArray<::ULIS::FEvent>& oEvents)
{
    TSharedRef<::ULIS::FBlock> block = MakeShared<::ULIS::FBlock>(iRect.w, iRect.h, iFormat);

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(iFormat);
    ::ULIS::FEvent eventClearBlock = FULISEventBuilder().RetainBlock(block).Build();
    ctx.Clear(*block, block->Rect(), ::ULIS::FSchedulePolicy::AsyncCacheEfficient, 0, nullptr, &eventClearBlock);
    oEvents = Copy(block, iRect, ::ULIS::FVec2I(0), {eventClearBlock});
    return block;
} */

IOdysseyImageRenderer::eRenderType
IOdysseyImageRenderer::GetRenderType() const
{
    return mRenderType;
}

TArray<::ULIS::FEvent>
IOdysseyImageRenderer::ConvertAndBlend(TSharedPtr<::ULIS::FBlock> iFront, TSharedPtr<::ULIS::FBlock> iBack, ::ULIS::eBlendMode iBlendMode, float iOpacity, const TArray<::ULIS::FRectI>& iRects, const TArray<::ULIS::FVec2I>& iPos, const TArray<::ULIS::FEvent>& iWaitList)
{
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(iFront->Format());

    TArray<::ULIS::FEvent> events;
    for ( int i = 0; i < iRects.Num(); i++ )
    {
        TArray<::ULIS::FEvent> eventConvertAndExecute = ULISUtils::ConvertAndExecute(iBack, iFront->Format(), iRects[i], iPos[i], iWaitList,
            [&ctx, iFront, &iBlendMode, &iOpacity](TSharedPtr<::ULIS::FBlock> ioDest, const ::ULIS::FRectI& iRect, const ::ULIS::FVec2I& iPos, const TArray<::ULIS::FEvent>& iWaitList) -> TArray<::ULIS::FEvent>
            {
                ::ULIS::FEvent eventBlend = FULISEventBuilder().RetainBlock(iFront).RetainBlock(ioDest).Build();
                ctx.Blend(
                    *iFront,
                    *ioDest,
                    iRect,
                    iPos,
                    iBlendMode,
                    ::ULIS::Alpha_Normal,
                    iOpacity,
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

    return events;
}

TArray<::ULIS::FEvent>
IOdysseyImageRenderer::ConvertAndCopy(TSharedPtr<::ULIS::FBlock> iSrc, TSharedPtr<::ULIS::FBlock> iDst, const TArray<::ULIS::FRectI>& iRects, const TArray<::ULIS::FVec2I>& iPos, const TArray<::ULIS::FEvent>& iWaitList)
{
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(iSrc->Format());
    TArray<::ULIS::FEvent> events;
    for ( int i = 0; i < iRects.Num(); i++ )
    {
        ::ULIS::FEvent eventCopy = FULISEventBuilder().RetainBlock(iSrc).RetainBlock(iDst).Build();
        ctx.ConvertFormat(*iSrc, *iDst, iRects[i], iPos[i], ::ULIS::FSchedulePolicy::AsyncCacheEfficient, iWaitList.Num(), iWaitList.GetData(), &eventCopy);
        events.Add(eventCopy);
        ctx.Flush();
    }
    return events;

}

TArray<::ULIS::FEvent>
IOdysseyImageRenderer::Clear(TSharedPtr<::ULIS::FBlock> ioBlock, const TArray<::ULIS::FRectI>& iRects, const TArray<::ULIS::FVec2I>& iPos, const TArray<::ULIS::FEvent>& iWaitList)
{
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(ioBlock->Format());
    TArray<::ULIS::FEvent> events;
    for ( int i = 0; i < iRects.Num(); i++ )
    {
        ::ULIS::FEvent eventClear = FULISEventBuilder().RetainBlock(ioBlock).Build();
        ::ULIS::FRectI rect = ::ULIS::FRectI::FromXYWH(iPos[i].x, iPos[i].y, iRects[i].w, iRects[i].h);
        ctx.Clear(*ioBlock, rect, ::ULIS::FSchedulePolicy::AsyncCacheEfficient, iWaitList.Num(), iWaitList.GetData(), &eventClear);
        events.Add(eventClear);
        ctx.Flush();
    }

    return events;
}