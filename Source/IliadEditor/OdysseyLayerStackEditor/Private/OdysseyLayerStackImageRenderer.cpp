// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyLayerStackImageRenderer.h"
#include "OdysseyLayerStack.h"
#include "OdysseyLayer.h"

FOdysseyLayerStackImageRenderer::FOdysseyLayerStackImageRenderer(const UOdysseyLayerStack* iLayerStack, int iFrame, IOdysseyImageRenderer::eRenderType iRenderType, const TArray<::ULIS::FRectI> iDefaultRects, FImageRendererFilter iFilter)
    : IOdysseyImageRenderer(iRenderType, iDefaultRects)
    , mLayerRootRenderer(nullptr)
{
    UOdysseyLayer* layerRoot = Cast<UOdysseyLayer>(iLayerStack->LayerRoot);
    mLayerRootRenderer = layerRoot->BuildImageRenderer(iRenderType, iFrame, iFilter);
}

void
FOdysseyLayerStackImageRenderer::Init()
{
    if (mLayerRootRenderer)
        mLayerRootRenderer->Init();
}

TArray<::ULIS::FEvent>
FOdysseyLayerStackImageRenderer::Blend(const FOdysseyImageRendererBlendParams& iParams, const TArray<::ULIS::FEvent>& iWaitList)
{
    if (!mLayerRootRenderer)
        return iWaitList;

    return mLayerRootRenderer->Blend(iParams, iWaitList);
}

TArray<::ULIS::FEvent>
FOdysseyLayerStackImageRenderer::Copy(const FOdysseyImageRendererCopyParams& iParams, const TArray<::ULIS::FEvent>& iWaitList)
{
    TRACE_CPUPROFILER_EVENT_SCOPE(FOdysseyLayerStackImageRenderer::Copy);
    if (!mLayerRootRenderer)
        return iWaitList;

    return mLayerRootRenderer->Copy(iParams, iWaitList);
}

bool
FOdysseyLayerStackImageRenderer::IsGameThreadOnly()
{
    if (!mLayerRootRenderer)
        return false;

    return mLayerRootRenderer->IsGameThreadOnly();
}
