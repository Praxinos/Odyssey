// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyImageRenderer.h"
#include "ULISEventBuilder.h"
#include "ULISLoaderModule.h"

IOdysseyImageRenderer::IOdysseyImageRenderer(IOdysseyImageRenderer::eRenderType iRenderType, const TArray<::ULIS::FRectI>& iDefaultRects)
    : mRenderType(iRenderType)
    , mDefaultRects(iDefaultRects)
{
}

TArray<::ULIS::FEvent>
IOdysseyImageRenderer::RenderOverBlock(TSharedPtr<::ULIS::FBlock> ioBlock, const TArray<::ULIS::FRectI>& iRects, const TArray<::ULIS::FVec2I>& iPos, const TArray<::ULIS::FEvent>& iWaitList)
{
    return RenderInBlock(ioBlock, iRects, iPos, iWaitList);
}

TArray<::ULIS::FEvent>
IOdysseyImageRenderer::RenderOverBlock(TSharedPtr<::ULIS::FBlock> ioBlock, const TArray<::ULIS::FRectI>& iRects, const TArray<::ULIS::FEvent>& iWaitList)
{
    TArray<::ULIS::FVec2I> pos;
    for (const ::ULIS::FRectI& rect : iRects)
        pos.Add(rect.Position());

    return RenderOverBlock(ioBlock, iRects, pos, iWaitList);
}

TArray<::ULIS::FEvent>
IOdysseyImageRenderer::RenderOverBlock(TSharedPtr<::ULIS::FBlock> ioBlock, const ::ULIS::FRectI& iRect, const ::ULIS::FVec2I& iPos, const TArray<::ULIS::FEvent>& iWaitList)
{
    TArray<::ULIS::FRectI> rects = { iRect };
    TArray<::ULIS::FVec2I> pos = { iPos };
    return RenderOverBlock(ioBlock, rects, pos, iWaitList);
}

TArray<::ULIS::FEvent>
IOdysseyImageRenderer::RenderOverBlock(TSharedPtr<::ULIS::FBlock> ioBlock, const ::ULIS::FRectI& iRect, const TArray<::ULIS::FEvent>& iWaitList)
{
    TArray<::ULIS::FRectI> rects = { iRect };
    TArray<::ULIS::FVec2I> pos = { iRect.Position() };
    return RenderOverBlock(ioBlock, rects, pos, iWaitList);
}

TArray<::ULIS::FEvent>
IOdysseyImageRenderer::RenderOverBlock(TSharedPtr<::ULIS::FBlock> ioBlock, const TArray<::ULIS::FEvent>& iWaitList)
{
    return RenderOverBlock(ioBlock, mDefaultRects, iWaitList);
}

TArray<::ULIS::FEvent>
IOdysseyImageRenderer::RenderInBlock(TSharedPtr<::ULIS::FBlock> ioBlock, const TArray<::ULIS::FRectI>& iRects, const TArray<::ULIS::FEvent>& iWaitList)
{
    TArray<::ULIS::FVec2I> pos;
    for (const ::ULIS::FRectI& rect : iRects)
        pos.Add(rect.Position());

    return RenderInBlock(ioBlock, iRects, pos, iWaitList);
}

TArray<::ULIS::FEvent>
IOdysseyImageRenderer::RenderInBlock(TSharedPtr<::ULIS::FBlock> ioBlock, const ::ULIS::FRectI& iRect, const ::ULIS::FVec2I& iPos, const TArray<::ULIS::FEvent>& iWaitList)
{
    TArray<::ULIS::FRectI> rects = { iRect };
    TArray<::ULIS::FVec2I> pos = { iPos };
    return RenderInBlock(ioBlock, rects, pos, iWaitList);
}

TArray<::ULIS::FEvent>
IOdysseyImageRenderer::RenderInBlock(TSharedPtr<::ULIS::FBlock> ioBlock, const ::ULIS::FRectI& iRect, const TArray<::ULIS::FEvent>& iWaitList)
{
    TArray<::ULIS::FRectI> rects = { iRect };
    TArray<::ULIS::FVec2I> pos = { iRect.Position() };
    return RenderInBlock(ioBlock, rects, pos, iWaitList);
}

TArray<::ULIS::FEvent>
IOdysseyImageRenderer::RenderInBlock(TSharedPtr<::ULIS::FBlock> ioBlock, const TArray<::ULIS::FEvent>& iWaitList)
{
    return RenderInBlock(ioBlock, mDefaultRects, iWaitList);
}

TSharedPtr<::ULIS::FBlock>
IOdysseyImageRenderer::RenderInNewBlock(::ULIS::eFormat iFormat, const ::ULIS::FRectI& iRect, TArray<::ULIS::FEvent>& oEvents)
{
    TSharedRef<::ULIS::FBlock> block = MakeShared<::ULIS::FBlock>(iRect.w, iRect.h, iFormat);

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(iFormat);
    ::ULIS::FEvent eventClearBlock = FULISEventBuilder().RetainBlock(block).Build();
    ctx.Clear(*block, block->Rect(), ::ULIS::FSchedulePolicy::AsyncCacheEfficient, 0, nullptr, &eventClearBlock);
    oEvents = RenderInBlock(block, iRect, ::ULIS::FVec2I(0), {eventClearBlock});
    return block;
}

IOdysseyImageRenderer::eRenderType
IOdysseyImageRenderer::GetRenderType() const
{
    return mRenderType;
}

void
IOdysseyImageRenderer::AddHandle(TSharedPtr<IOdysseyHandle> iHandle)
{
    mHandles.Add( iHandle );
}