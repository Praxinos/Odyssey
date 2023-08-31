// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "LayerStack/OdysseyTextureLayerStackImageRenderer.h"

FOdysseyTextureLayerStackImageRenderer::FOdysseyTextureLayerStackImageRenderer(const UOdysseyTextureLayerStack* iLayerStack, IOdysseyImageRenderer::eRenderType iRenderType, const TArray<::ULIS::FRectI> iDefaultRects)
    : IOdysseyImageRenderer(iRenderType, iDefaultRects)
    , mLayerRootRenderer(nullptr)
{
    UOdysseyTextureLayer* layerRoot = Cast<UOdysseyTextureLayer>(iLayerStack->LayerRoot);
    mLayerRootRenderer = layerRoot->BuildImageRenderer(iRenderType);
}

TArray<::ULIS::FEvent>
FOdysseyTextureLayerStackImageRenderer::Blend(TSharedPtr<::ULIS::FBlock> ioBlock, ::ULIS::eBlendMode iBlendMode, float iOpacity, const TArray<::ULIS::FRectI>& iRects, const TArray<::ULIS::FVec2I>& iPos, const TArray<::ULIS::FEvent>& iWaitList)
{
    return mLayerRootRenderer->Blend(ioBlock, iBlendMode, iOpacity, iRects, iPos, iWaitList);
}

TArray<::ULIS::FEvent>
FOdysseyTextureLayerStackImageRenderer::Copy(TSharedPtr<::ULIS::FBlock> ioBlock, const TArray<::ULIS::FRectI>& iRects, const TArray<::ULIS::FVec2I>& iPos, const TArray<::ULIS::FEvent>& iWaitList)
{
    return mLayerRootRenderer->Copy(ioBlock, iRects, iPos, iWaitList);
}