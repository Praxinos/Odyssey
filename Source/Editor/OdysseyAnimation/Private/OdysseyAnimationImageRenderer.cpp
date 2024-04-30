// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyAnimationImageRenderer.h"

FOdysseyAnimationImageRenderer::FOdysseyAnimationImageRenderer(const UOdysseyAnimation* iAnimation, int iFrame, IOdysseyImageRenderer::eRenderType iRenderType, const TArray<::ULIS::FRectI> iDefaultRects, FImageRendererFilter iFilter)
    : IOdysseyImageRenderer(iRenderType, iDefaultRects)
    , mLayerStackRenderer(nullptr)
{
    UOdysseyAnimationLayerStack* layerStack = iAnimation->GetLayerStack();
    mLayerStackRenderer = layerStack->BuildImageRenderer(iRenderType, iFrame, iFilter);
}

void
FOdysseyAnimationImageRenderer::Init()
{
    if (mLayerStackRenderer)
        mLayerStackRenderer->Init();
}

TArray<::ULIS::FEvent>
FOdysseyAnimationImageRenderer::Blend(const FOdysseyImageRendererBlendParams& iParams, const TArray<::ULIS::FEvent>& iWaitList)
{
    if (!mLayerStackRenderer)
        return iWaitList;

    return mLayerStackRenderer->Blend(iParams, iWaitList);
}

TArray<::ULIS::FEvent>
FOdysseyAnimationImageRenderer::Copy(const FOdysseyImageRendererCopyParams& iParams, const TArray<::ULIS::FEvent>& iWaitList)
{
    TRACE_CPUPROFILER_EVENT_SCOPE(FOdysseyAnimationImageRenderer::Copy);
    if (!mLayerStackRenderer)
        return iWaitList;

    return mLayerStackRenderer->Copy(iParams, iWaitList);
}

bool
FOdysseyAnimationImageRenderer::IsGameThreadOnly()
{
    if (!mLayerStackRenderer)
        return false;

    return mLayerStackRenderer->IsGameThreadOnly();
}