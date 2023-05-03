// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Abilities/IOdysseyAnimationImageRenderingAbility.h"
#include "ULISEventBuilder.h"
#include "ULISLoaderModule.h"


IOdysseyAnimationImageRenderingAbility::FOnChanged&
IOdysseyAnimationImageRenderingAbility::OnChanged()
{
    static FOnChanged onChanged;
    return onChanged;
}

IOdysseyAnimationImageRenderingAbility::FOnChanged&
IOdysseyAnimationImageRenderingAbility::OnCommited()
{
    static FOnChanged onCommited;
    return onCommited;
}

IOdysseyAnimationImageRenderingAbility::FOnCompositionChanged&
IOdysseyAnimationImageRenderingAbility::OnCompositionChanged()
{
    static FOnCompositionChanged onCompositionChanged;
    return onCompositionChanged;
}

IOdysseyAnimationImageRenderingAbility::FOnCompositionChanged&
IOdysseyAnimationImageRenderingAbility::OnCompositionCommited()
{
    static FOnCompositionChanged onCompositionCommited;
    return onCompositionCommited;
}

IOdysseyAnimationImageRenderingAbility::IOdysseyAnimationImageRenderingAbility()
    : mId(FGuid::NewGuid())
{
}
    
const FGuid&
IOdysseyAnimationImageRenderingAbility::GetId() const
{
    return mId;
}

TArray<::ULIS::FEvent>
IOdysseyAnimationImageRenderingAbility::RenderOverBlock(TSharedPtr<::ULIS::FBlock> ioBlock, int iFrame, const TArray<::ULIS::FRectI>& iRects, const TArray<::ULIS::FVec2I>& iPos, const TArray<::ULIS::FEvent>& iWaitList)
{
    return RenderInBlock(ioBlock, iFrame, iRects, iPos, iWaitList);
}

TArray<::ULIS::FEvent>
IOdysseyAnimationImageRenderingAbility::RenderOverBlock(TSharedPtr<::ULIS::FBlock> ioBlock, int iFrame, const TArray<::ULIS::FRectI>& iRects, const TArray<::ULIS::FEvent>& iWaitList)
{
    TArray<::ULIS::FVec2I> pos;
    for (const ::ULIS::FRectI& rect : iRects)
        pos.Add(rect.Position());

    return RenderOverBlock(ioBlock, iFrame, iRects, pos, iWaitList);
}

TArray<::ULIS::FEvent>
IOdysseyAnimationImageRenderingAbility::RenderOverBlock(TSharedPtr<::ULIS::FBlock> ioBlock, int iFrame, const ::ULIS::FRectI& iRect, const ::ULIS::FVec2I& iPos, const TArray<::ULIS::FEvent>& iWaitList)
{
    TArray<::ULIS::FRectI> rects = { iRect };
    TArray<::ULIS::FVec2I> pos = { iPos };
    return RenderOverBlock(ioBlock, iFrame, rects, pos, iWaitList);
}

TArray<::ULIS::FEvent>
IOdysseyAnimationImageRenderingAbility::RenderOverBlock(TSharedPtr<::ULIS::FBlock> ioBlock, int iFrame, const ::ULIS::FRectI& iRect, const TArray<::ULIS::FEvent>& iWaitList)
{
    TArray<::ULIS::FRectI> rects = { iRect };
    TArray<::ULIS::FVec2I> pos = { iRect.Position() };
    return RenderOverBlock(ioBlock, iFrame, rects, pos, iWaitList);
}

TArray<::ULIS::FEvent>
IOdysseyAnimationImageRenderingAbility::RenderOverBlock(TSharedPtr<::ULIS::FBlock> ioBlock, int iFrame, const TArray<::ULIS::FEvent>& iWaitList)
{
    return RenderOverBlock(ioBlock, iFrame, GetRects(iFrame), iWaitList);
}

TArray<::ULIS::FEvent>
IOdysseyAnimationImageRenderingAbility::RenderInBlock(TSharedPtr<::ULIS::FBlock> ioBlock, int iFrame, const TArray<::ULIS::FRectI>& iRects, const TArray<::ULIS::FEvent>& iWaitList)
{
    TArray<::ULIS::FVec2I> pos;
    for (const ::ULIS::FRectI& rect : iRects)
        pos.Add(rect.Position());

    return RenderInBlock(ioBlock, iFrame, iRects, pos, iWaitList);
}

TArray<::ULIS::FEvent>
IOdysseyAnimationImageRenderingAbility::RenderInBlock(TSharedPtr<::ULIS::FBlock> ioBlock, int iFrame, const ::ULIS::FRectI& iRect, const ::ULIS::FVec2I& iPos, const TArray<::ULIS::FEvent>& iWaitList)
{
    TArray<::ULIS::FRectI> rects = { iRect };
    TArray<::ULIS::FVec2I> pos = { iPos };
    return RenderInBlock(ioBlock, iFrame, rects, pos, iWaitList);
}

TArray<::ULIS::FEvent>
IOdysseyAnimationImageRenderingAbility::RenderInBlock(TSharedPtr<::ULIS::FBlock> ioBlock, int iFrame, const ::ULIS::FRectI& iRect, const TArray<::ULIS::FEvent>& iWaitList)
{
    TArray<::ULIS::FRectI> rects = { iRect };
    TArray<::ULIS::FVec2I> pos = { iRect.Position() };
    return RenderInBlock(ioBlock, iFrame, rects, pos, iWaitList);
}

TArray<::ULIS::FEvent>
IOdysseyAnimationImageRenderingAbility::RenderInBlock(TSharedPtr<::ULIS::FBlock> ioBlock, int iFrame, const TArray<::ULIS::FEvent>& iWaitList)
{
    return RenderInBlock(ioBlock, iFrame, GetRects(iFrame), iWaitList);
}

TSharedPtr<::ULIS::FBlock>
IOdysseyAnimationImageRenderingAbility::RenderInNewBlock(int iFrame, ::ULIS::eFormat iFormat, const ::ULIS::FRectI& iRect, TArray<::ULIS::FEvent>& oEvents)
{
    TSharedRef<::ULIS::FBlock> block = MakeShared<::ULIS::FBlock>(iRect.w, iRect.h, iFormat);

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(iFormat);
    ::ULIS::FEvent eventClearBlock = FULISEventBuilder().RetainBlock(block).Build();
    ctx.Clear(*block, block->Rect(), ::ULIS::FSchedulePolicy::AsyncCacheEfficient, 0, nullptr, &eventClearBlock);
    oEvents = RenderInBlock(block, iFrame, iRect, ::ULIS::FVec2I(0), {eventClearBlock});
    return block;
}

TSharedPtr<IOdysseyHandle>
IOdysseyAnimationImageRenderingAbility::Preload(int iFrame)
{
    return nullptr;
}