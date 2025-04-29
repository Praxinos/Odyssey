// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyAnimationProxyImageRenderer.h"

#include "OdysseyAnimationLayerRoot.h"
#include "OdysseyLayerImageRenderer.h"
#include "OdysseyAnimationProxy.h"

FOdysseyAnimationProxyImageRenderer::FOdysseyAnimationProxyImageRenderer(const UOdysseyAnimationLayerRoot* iLayer, int iFrameIndex, EOdysseyRenderingType iRenderType, const TArray<FIntRect> iDefaultRects, FImageRendererFilter iFilter)
    : IOdysseyImageRenderer(iRenderType, iDefaultRects)
    //, mProxy(iLayer->GetProxy())
    , mFrameIndex(iFrameIndex)
    , mLayerRenderer(nullptr)
    , mBlock(nullptr)
    , mForceRender(iFilter.IsBound())
{
    mLayerRenderer = MakeShared<FOdysseyLayerImageRenderer>(iLayer, iFrameIndex, iRenderType, iDefaultRects, iFilter);
}

void
FOdysseyAnimationProxyImageRenderer::Init()
{
    TRACE_CPUPROFILER_EVENT_SCOPE(FOdysseyAnimationProxyImageRenderer::Init);
    if ( GetRenderType() == EOdysseyRenderingType::Editor || GetRenderType() == EOdysseyRenderingType::RenderOutOfPegs || mForceRender )
        mLayerRenderer->Init();

    if ( GetRenderType() == EOdysseyRenderingType::Render )
    {
        //mBlock = mProxy->GetBlock(mFrameIndex);
        if (!mBlock)
            mLayerRenderer->Init();
    }

}

TArray<::ULIS::FEvent>
FOdysseyAnimationProxyImageRenderer::Blend(const FOdysseyImageRendererBlendParams& iParams, const TArray<::ULIS::FEvent>& iWaitList)
{
    TRACE_CPUPROFILER_EVENT_SCOPE(FOdysseyAnimationProxyImageRenderer::Blend);
    if (GetRenderType() != EOdysseyRenderingType::Render || mForceRender)
        return mLayerRenderer->Blend(iParams, iWaitList);

    if ( !mBlock )
        return mLayerRenderer->Blend(iParams, iWaitList);

    return ConvertAndBlend(mBlock, ::ULIS::FVec2I(0), iParams, iWaitList);
}

TArray<::ULIS::FEvent>
FOdysseyAnimationProxyImageRenderer::Copy(const FOdysseyImageRendererCopyParams& iParams, const TArray<::ULIS::FEvent>& iWaitList)
{
    TRACE_CPUPROFILER_EVENT_SCOPE(FOdysseyAnimationProxyImageRenderer::Copy);
    if (GetRenderType() != EOdysseyRenderingType::Render || mForceRender)
        return mLayerRenderer->Copy(iParams, iWaitList);

    if ( !mBlock )
        return mLayerRenderer->Copy(iParams, iWaitList);

    return ConvertAndCopy(mBlock, ::ULIS::FVec2I(0), iParams, iWaitList);
}

bool
FOdysseyAnimationProxyImageRenderer::IsGameThreadOnly()
{
    return true;
}
