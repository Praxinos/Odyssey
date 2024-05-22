// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "LayerStack/OdysseyTextureLayerStackImageRenderer.h"
#include "LayerStack/OdysseyTextureLayer.h"
#include "OdysseyTextureLayerStack.h"

FOdysseyTextureLayerStackImageRenderer::FOdysseyTextureLayerStackImageRenderer(const UOdysseyTextureLayerStack* iLayerStack, IOdysseyImageRenderer::eRenderType iRenderType, const TArray<::ULIS::FRectI> iDefaultRects, FImageRendererFilter iFilter)
    : IOdysseyImageRenderer(iRenderType, iDefaultRects)
    , mLayerRootRenderer(nullptr)
{
    UOdysseyTextureLayer* layerRoot = Cast<UOdysseyTextureLayer>(iLayerStack->LayerRoot);
    mLayerRootRenderer = layerRoot->BuildImageRenderer(iRenderType, iFilter);
}

void
FOdysseyTextureLayerStackImageRenderer::Init()
{
    if (mLayerRootRenderer)
        mLayerRootRenderer->Init();
}

bool
FOdysseyTextureLayerStackImageRenderer::IsGameThreadOnly()
{
    if (!mLayerRootRenderer)
        return false;

    return mLayerRootRenderer->IsGameThreadOnly();
}

TArray<::ULIS::FEvent>
FOdysseyTextureLayerStackImageRenderer::Blend(const FOdysseyImageRendererBlendParams& iParams, const TArray<::ULIS::FEvent>& iWaitList)
{
    if (!mLayerRootRenderer)
        return iWaitList;

    return mLayerRootRenderer->Blend(iParams, iWaitList);
}

TArray<::ULIS::FEvent>
FOdysseyTextureLayerStackImageRenderer::Copy(const FOdysseyImageRendererCopyParams& iParams, const TArray<::ULIS::FEvent>& iWaitList)
{
    if (!mLayerRootRenderer)
        return iWaitList;
        
    return mLayerRootRenderer->Copy(iParams, iWaitList);
}