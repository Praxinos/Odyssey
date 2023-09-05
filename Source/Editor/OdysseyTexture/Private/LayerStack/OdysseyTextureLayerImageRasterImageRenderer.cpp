// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "LayerStack/OdysseyTextureLayerImageRasterImageRenderer.h"

FOdysseyTextureLayerImageRasterImageRenderer::FOdysseyTextureLayerImageRasterImageRenderer(const UOdysseyTextureLayerImageRaster* iLayer, IOdysseyImageRenderer::eRenderType iRenderType, const TArray<::ULIS::FRectI>& iDefaultRects)
    : IOdysseyImageRenderer(iRenderType, iDefaultRects)
    , mBlock()
{
    TSharedPtr<FOdysseyRasterBlock> rasterBlock = iLayer->GetRasterBlock();
    if (rasterBlock)
        mBlock = rasterBlock->GetBlock();
}

TArray<::ULIS::FEvent>
FOdysseyTextureLayerImageRasterImageRenderer::Blend(TSharedPtr<::ULIS::FBlock> ioBlock, ::ULIS::eBlendMode iBlendMode, float iOpacity, const TArray<::ULIS::FRectI>& iRects, const TArray<::ULIS::FVec2I>& iPos, const TArray<::ULIS::FEvent>& iWaitList)
{
    if (!mBlock)
        return iWaitList;

    return ConvertAndBlend(mBlock, ioBlock, iBlendMode, iOpacity, iRects, iPos, iWaitList);
}

TArray<::ULIS::FEvent>
FOdysseyTextureLayerImageRasterImageRenderer::Copy(TSharedPtr<::ULIS::FBlock> ioBlock, const TArray<::ULIS::FRectI>& iRects, const TArray<::ULIS::FVec2I>& iPos, const TArray<::ULIS::FEvent>& iWaitList)
{
    if (!mBlock)
        return iWaitList;

    return ConvertAndCopy(mBlock, ioBlock, iRects, iPos, iWaitList);
}