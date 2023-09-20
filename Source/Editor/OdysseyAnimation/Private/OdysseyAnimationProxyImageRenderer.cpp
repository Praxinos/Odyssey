// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyAnimationProxyImageRenderer.h"


FOdysseyAnimationProxyImageRenderer::FOdysseyAnimationProxyImageRenderer(const UOdysseyAnimation* iAnimation, int iFrameIndex, IOdysseyImageRenderer::eRenderType iRenderType, const TArray<::ULIS::FRectI> iDefaultRects)
    : IOdysseyImageRenderer(iRenderType, iDefaultRects)
    , mProxy(iAnimation->GetProxy())
    , mFrameIndex(iFrameIndex)
    , mAnimationRenderer(nullptr)
    , mBlock(nullptr)
{
    if ( GetRenderType() != IOdysseyImageRenderer::eRenderType::Render )
    {
        mAnimationRenderer = MakeShared<FOdysseyAnimationImageRenderer>(iAnimation, iFrameIndex, iRenderType, iDefaultRects);
    }
}

void
FOdysseyAnimationProxyImageRenderer::Init()
{
    if ( GetRenderType() != IOdysseyImageRenderer::eRenderType::Render )
    {
        mAnimationRenderer->Init();
    }
}

TArray<::ULIS::FEvent>
FOdysseyAnimationProxyImageRenderer::Blend(TSharedPtr<::ULIS::FBlock> ioBlock, ::ULIS::eBlendMode iBlendMode, float iOpacity, const TArray<::ULIS::FRectI>& iRects, const TArray<::ULIS::FVec2I>& iPos, const TArray<::ULIS::FEvent>& iWaitList)
{
    if (GetRenderType() != IOdysseyImageRenderer::eRenderType::Render)
        return mAnimationRenderer->Blend(ioBlock, iBlendMode, iOpacity, iRects, iPos, iWaitList);

    if ( !mBlock )
        mBlock = mProxy->GetBlock(mFrameIndex); //Try to get the block in memory

    if ( !mBlock )
        return iWaitList;

    return ConvertAndBlend(mBlock, ioBlock, iBlendMode, iOpacity, iRects, iPos, iWaitList);
}

TArray<::ULIS::FEvent>
FOdysseyAnimationProxyImageRenderer::Copy(TSharedPtr<::ULIS::FBlock> ioBlock, const TArray<::ULIS::FRectI>& iRects, const TArray<::ULIS::FVec2I>& iPos, const TArray<::ULIS::FEvent>& iWaitList)
{
    if (GetRenderType() != IOdysseyImageRenderer::eRenderType::Render)
        return mAnimationRenderer->Copy(ioBlock, iRects, iPos, iWaitList);

    if ( !mBlock )
        mBlock = mProxy->GetBlock(mFrameIndex); //Try to get the block in memory

    if ( !mBlock )
        return iWaitList;

    return ConvertAndCopy(mBlock, ioBlock, iRects, iPos, iWaitList);
}

bool
FOdysseyAnimationProxyImageRenderer::IsGameThreadOnly()
{
    return true;
}