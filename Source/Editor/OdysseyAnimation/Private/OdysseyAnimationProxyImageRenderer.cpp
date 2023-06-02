// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyAnimationProxyImageRenderer.h"


FOdysseyAnimationProxyImageRenderer::FOdysseyAnimationProxyImageRenderer(UOdysseyAnimation* iAnimation, int iFrameIndex, IOdysseyImageRenderer::eRenderType iRenderType, const TArray<::ULIS::FRectI> iDefaultRects)
    : IOdysseyImageRenderer(iRenderType, iDefaultRects)
    , mAnimation(iAnimation)
    , mProxy(iAnimation->GetProxy())
    , mFrameIndex(iFrameIndex)
    , mAnimationRenderer(nullptr)
{
    TSharedPtr<IOdysseyAnimationImageRenderingAbility> animationAbility = mAnimation->GetAbility<IOdysseyAnimationImageRenderingAbility>();
    mAnimationRenderer = MakeShared<FOdysseyAnimationImageRenderer>(mAnimation, iFrameIndex, iRenderType, iDefaultRects);
}

TArray<::ULIS::FEvent>
FOdysseyAnimationProxyImageRenderer::RenderInBlock(TSharedPtr<::ULIS::FBlock> ioBlock, const TArray<::ULIS::FRectI>& iRects, const TArray<::ULIS::FVec2I>& iPos, const TArray<::ULIS::FEvent>& iWaitList)
{
    if (GetRenderType() != IOdysseyImageRenderer::eRenderType::Render)
        return mAnimationRenderer->RenderInBlock(ioBlock, iRects, iPos, iWaitList);

    TSharedPtr<::ULIS::FBlock> block = mProxy->GetBlock(mFrameIndex);
    if ( !block )
        return mAnimationRenderer->RenderInBlock(ioBlock, iRects, iPos, iWaitList);


    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(ioBlock->Format());

    TArray<::ULIS::FEvent> events;
    for ( int i = 0; i < iRects.Num(); i++ )
    {
        TArray<::ULIS::FEvent> eventConvertAndExecute = ULISUtils::ConvertAndExecute(ioBlock, block->Format(), iRects[i], iPos[i], iWaitList,
            [this, &block, &ctx](TSharedPtr<::ULIS::FBlock> ioDest, const ::ULIS::FRectI& iRect, const ::ULIS::FVec2I& iPos, const TArray<::ULIS::FEvent>& iWaitList) -> TArray<::ULIS::FEvent>
            {
                ::ULIS::FEvent eventCopy = FULISEventBuilder().RetainBlock(block).RetainBlock(ioDest).Build();
                ctx.Copy(
                    *block,
                    *ioDest,
                    iRect,
                    iPos,
                    ::ULIS::FSchedulePolicy::AsyncCacheEfficient,
                    iWaitList.Num(),
                    iWaitList.GetData(),
                    &eventCopy
                );
                return { eventCopy };
            }
        );
        events.Append(eventConvertAndExecute);
    }

    ctx.Flush();

    return events;
}