// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyAnimationProxyImageRenderer.h"

#include "OdysseyAnimation.h"
#include "OdysseyAnimationImageRenderer.h"

FOdysseyAnimationProxyImageRenderer::FOdysseyAnimationProxyImageRenderer(const UOdysseyAnimation* iAnimation, int iFrameIndex, IOdysseyImageRenderer::eRenderType iRenderType, const TArray<::ULIS::FRectI> iDefaultRects, FImageRendererFilter iFilter)
    : IOdysseyImageRenderer(iRenderType, iDefaultRects)
    , mProxy(iAnimation->GetProxy())
    , mFrameIndex(iFrameIndex)
    , mAnimationRenderer(nullptr)
    , mBlock(nullptr)
    , mForceRender(iFilter.IsBound())
{
    mAnimationRenderer = MakeShared<FOdysseyAnimationImageRenderer>(iAnimation, iFrameIndex, iRenderType, iDefaultRects, iFilter);
}

void
FOdysseyAnimationProxyImageRenderer::Init()
{
	TRACE_CPUPROFILER_EVENT_SCOPE(FOdysseyAnimationProxyImageRenderer::Init);
    if ( GetRenderType() == IOdysseyImageRenderer::eRenderType::Editor || GetRenderType() == IOdysseyImageRenderer::eRenderType::RenderOutOfPegs || mForceRender )
        mAnimationRenderer->Init();

    if ( GetRenderType() == IOdysseyImageRenderer::eRenderType::Render )
    {
        mBlock = mProxy->GetBlock(mFrameIndex);
        if (!mBlock)
            mAnimationRenderer->Init();
    }

}

TArray<::ULIS::FEvent>
FOdysseyAnimationProxyImageRenderer::Blend(const FOdysseyImageRendererBlendParams& iParams, const TArray<::ULIS::FEvent>& iWaitList)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(FOdysseyAnimationProxyImageRenderer::Blend);
    if (GetRenderType() != IOdysseyImageRenderer::eRenderType::Render || mForceRender)
        return mAnimationRenderer->Blend(iParams, iWaitList);

    if ( !mBlock )
        return mAnimationRenderer->Blend(iParams, iWaitList);

    return ConvertAndBlend(mBlock, ::ULIS::FVec2I(0), iParams, iWaitList);
}

TArray<::ULIS::FEvent>
FOdysseyAnimationProxyImageRenderer::Copy(const FOdysseyImageRendererCopyParams& iParams, const TArray<::ULIS::FEvent>& iWaitList)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(FOdysseyAnimationProxyImageRenderer::Copy);
    if (GetRenderType() != IOdysseyImageRenderer::eRenderType::Render || mForceRender)
        return mAnimationRenderer->Copy(iParams, iWaitList);

    /* if ( !mBlock )
        mBlock = mProxy->GetBlock(mFrameIndex); //Try to get the block in memory */

    if ( !mBlock )
        return mAnimationRenderer->Copy(iParams, iWaitList);

    return ConvertAndCopy(mBlock, ::ULIS::FVec2I(0), iParams, iWaitList);
}

bool
FOdysseyAnimationProxyImageRenderer::IsGameThreadOnly()
{
    return true;
}