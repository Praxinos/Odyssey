// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyAnimationImageRenderer.h"

FOdysseyAnimationImageRenderer::FOdysseyAnimationImageRenderer(const UOdysseyAnimation* iAnimation, int iFrame, IOdysseyImageRenderer::eRenderType iRenderType, const TArray<::ULIS::FRectI> iDefaultRects)
    : IOdysseyImageRenderer(iRenderType, iDefaultRects)
    , mLayerStackRenderer(nullptr)
{
    UOdysseyAnimationLayerStack* layerStack = iAnimation->GetLayerStack();
    mLayerStackRenderer = layerStack->BuildImageRenderer(iRenderType, iFrame);
}

TArray<::ULIS::FEvent>
FOdysseyAnimationImageRenderer::Blend(TSharedPtr<::ULIS::FBlock> ioBlock, ::ULIS::eBlendMode iBlendMode, float iOpacity, const TArray<::ULIS::FRectI>& iRects, const TArray<::ULIS::FVec2I>& iPos, const TArray<::ULIS::FEvent>& iWaitList)
{
    return mLayerStackRenderer->Blend(ioBlock, iBlendMode, iOpacity, iRects, iPos, iWaitList);
}

TArray<::ULIS::FEvent>
FOdysseyAnimationImageRenderer::Copy(TSharedPtr<::ULIS::FBlock> ioBlock, const TArray<::ULIS::FRectI>& iRects, const TArray<::ULIS::FVec2I>& iPos, const TArray<::ULIS::FEvent>& iWaitList)
{
    return mLayerStackRenderer->Copy(ioBlock, iRects, iPos, iWaitList);
}