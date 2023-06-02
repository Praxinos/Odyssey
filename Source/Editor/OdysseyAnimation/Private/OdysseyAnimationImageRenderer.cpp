// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyAnimationImageRenderer.h"

FOdysseyAnimationImageRenderer::FOdysseyAnimationImageRenderer(UOdysseyAnimation* iAnimation, int iFrame, IOdysseyImageRenderer::eRenderType iRenderType, const TArray<::ULIS::FRectI> iDefaultRects)
    : IOdysseyImageRenderer(iRenderType, iDefaultRects)
    , mLayerStackRenderer(nullptr)
{
    UOdysseyAnimationLayerStack* layerStack = iAnimation->GetLayerStack();
    TSharedPtr<IOdysseyAnimationImageRenderingAbility> layerStackAbility = layerStack->GetAbility<IOdysseyAnimationImageRenderingAbility>();
    mLayerStackRenderer = layerStackAbility->BuildRenderer(iFrame, iRenderType);
}

TArray<::ULIS::FEvent>
FOdysseyAnimationImageRenderer::RenderInBlock(TSharedPtr<::ULIS::FBlock> ioBlock, const TArray<::ULIS::FRectI>& iRects, const TArray<::ULIS::FVec2I>& iPos, const TArray<::ULIS::FEvent>& iWaitList)
{
    return mLayerStackRenderer->RenderInBlock(ioBlock, iRects, iPos, iWaitList);
}
