// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/OdysseyAnimationLayerStackImageRenderer.h"
#include "LayerStack/OdysseyAnimationLayerStack.h"
#include "LayerStack/Layers/OdysseyAnimationLayer.h"

FOdysseyAnimationLayerStackImageRenderer::FOdysseyAnimationLayerStackImageRenderer(const UOdysseyAnimationLayerStack* iLayerStack, int iFrame, IOdysseyImageRenderer::eRenderType iRenderType, const TArray<::ULIS::FRectI> iDefaultRects, FImageRendererFilter iFilter)
    : IOdysseyImageRenderer(iRenderType, iDefaultRects)
    , mLayerRootRenderer(nullptr)
{
    UOdysseyAnimationLayer* layerRoot = Cast<UOdysseyAnimationLayer>(iLayerStack->LayerRoot);
    mLayerRootRenderer = layerRoot->BuildImageRenderer(iRenderType, iFrame, iFilter);
}

void
FOdysseyAnimationLayerStackImageRenderer::Init()
{
    if (mLayerRootRenderer)
        mLayerRootRenderer->Init();
}

TArray<::ULIS::FEvent>
FOdysseyAnimationLayerStackImageRenderer::Blend(const FOdysseyImageRendererBlendParams& iParams, const TArray<::ULIS::FEvent>& iWaitList)
{
    if (!mLayerRootRenderer)
        return iWaitList;

    return mLayerRootRenderer->Blend(iParams, iWaitList);
}

TArray<::ULIS::FEvent>
FOdysseyAnimationLayerStackImageRenderer::Copy(const FOdysseyImageRendererCopyParams& iParams, const TArray<::ULIS::FEvent>& iWaitList)
{
    TRACE_CPUPROFILER_EVENT_SCOPE(FOdysseyAnimationLayerStackImageRenderer::Copy);
    if (!mLayerRootRenderer)
        return iWaitList;

    return mLayerRootRenderer->Copy(iParams, iWaitList);
}

bool
FOdysseyAnimationLayerStackImageRenderer::IsGameThreadOnly()
{
    if (!mLayerRootRenderer)
        return false;

    return mLayerRootRenderer->IsGameThreadOnly();
}
