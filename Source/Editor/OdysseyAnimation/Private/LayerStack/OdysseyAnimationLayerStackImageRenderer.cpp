// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "LayerStack/OdysseyAnimationLayerStackImageRenderer.h"

FOdysseyAnimationLayerStackImageRenderer::FOdysseyAnimationLayerStackImageRenderer(UOdysseyAnimationLayerStack* iLayerStack, int iFrame, IOdysseyImageRenderer::eRenderType iRenderType, const TArray<::ULIS::FRectI> iDefaultRects)
    : IOdysseyImageRenderer(iRenderType, iDefaultRects)
    , mLayerRootRenderer(nullptr)
{
    UOdysseyAnimationLayer* layerRoot = Cast<UOdysseyAnimationLayer>(iLayerStack->LayerRoot);
    TSharedPtr<IOdysseyAnimationImageRenderingAbility> layerRootAbility = layerRoot->GetAbility<IOdysseyAnimationImageRenderingAbility>();
    mLayerRootRenderer = layerRootAbility->BuildRenderer(iFrame, iRenderType);
}

TArray<::ULIS::FEvent>
FOdysseyAnimationLayerStackImageRenderer::RenderInBlock(TSharedPtr<::ULIS::FBlock> ioBlock, const TArray<::ULIS::FRectI>& iRects, const TArray<::ULIS::FVec2I>& iPos, const TArray<::ULIS::FEvent>& iWaitList)
{
    return mLayerRootRenderer->RenderInBlock(ioBlock, iRects, iPos, iWaitList);
}
