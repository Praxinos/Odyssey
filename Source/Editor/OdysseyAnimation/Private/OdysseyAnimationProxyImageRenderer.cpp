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

    if ( GetRenderType() != IOdysseyImageRenderer::eRenderType::Render )
        mAnimationRenderer = MakeShared<FOdysseyAnimationImageRenderer>(mAnimation, iFrameIndex, iRenderType, iDefaultRects);
}

TArray<::ULIS::FEvent>
FOdysseyAnimationProxyImageRenderer::Blend(TSharedPtr<::ULIS::FBlock> ioBlock, ::ULIS::eBlendMode iBlendMode, float iOpacity, const TArray<::ULIS::FRectI>& iRects, const TArray<::ULIS::FVec2I>& iPos, const TArray<::ULIS::FEvent>& iWaitList)
{
    if (GetRenderType() != IOdysseyImageRenderer::eRenderType::Render)
        return mAnimationRenderer->Blend(ioBlock, iBlendMode, iOpacity, iRects, iPos, iWaitList);

    TSharedPtr<::ULIS::FBlock> block = mProxy->GetBlock(mFrameIndex);
    if ( !block )
    {
        if (mAnimationRenderer)
            return mAnimationRenderer->Blend(ioBlock, iBlendMode, iOpacity, iRects, iPos, iWaitList);

        return iWaitList;
    }

    return ConvertAndBlend(block, ioBlock, iBlendMode, iOpacity, iRects, iPos, iWaitList);
}

TArray<::ULIS::FEvent>
FOdysseyAnimationProxyImageRenderer::Copy(TSharedPtr<::ULIS::FBlock> ioBlock, const TArray<::ULIS::FRectI>& iRects, const TArray<::ULIS::FVec2I>& iPos, const TArray<::ULIS::FEvent>& iWaitList)
{
    if (GetRenderType() != IOdysseyImageRenderer::eRenderType::Render)
        return mAnimationRenderer->Copy(ioBlock, iRects, iPos, iWaitList);

    TSharedPtr<::ULIS::FBlock> block = mProxy->GetBlock(mFrameIndex);
    if ( !block )
    {
        if (mAnimationRenderer)
            return mAnimationRenderer->Copy(ioBlock, iRects, iPos, iWaitList);

        return iWaitList;
    }

    return ConvertAndCopy(block, ioBlock, iRects, iPos, iWaitList);
}