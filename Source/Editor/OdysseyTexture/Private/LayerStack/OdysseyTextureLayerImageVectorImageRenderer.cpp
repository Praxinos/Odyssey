// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "LayerStack/OdysseyTextureLayerImageVectorImageRenderer.h"

FOdysseyTextureLayerImageVectorImageRenderer::FOdysseyTextureLayerImageVectorImageRenderer(const UOdysseyTextureLayerImageVector* iLayer, TSharedPtr<::ULIS::FBlock> iBlock, IOdysseyImageRenderer::eRenderType iRenderType, const TArray<::ULIS::FRectI>& iDefaultRects)
    : IOdysseyImageRenderer(iRenderType, iDefaultRects)
    , mEngine(const_cast<UOdysseyTextureLayerImageVector*>(iLayer)->GetEngine())
    , mBlock(iBlock)
    , mRenderHUD(false)
    , mIsColored(iLayer->IsColored)
{
    UOdysseyLayerStack* layerStack = iLayer->GetLayerStack();
    if (!layerStack)
        return;

    mRenderHUD = layerStack->CurrentLayer.Get() == iLayer;
}

TArray<::ULIS::FEvent>
FOdysseyTextureLayerImageVectorImageRenderer::Blend(TSharedPtr<::ULIS::FBlock> ioBlock, ::ULIS::eBlendMode iBlendMode, float iOpacity, const TArray<::ULIS::FRectI>& iRects, const TArray<::ULIS::FVec2I>& iPos, const TArray<::ULIS::FEvent>& iWaitList)
{
    if (!mBlock)
        return iWaitList;

    mEngine->Render( mIsColored ? 0 : FOdysseyVectorObject::DRAWING_IGNORECOLOR );

    // HUD displaying only for the current layer.
    if( mRenderHUD )
        mEngine->RenderHUD();

    return ConvertAndBlend(mBlock, ioBlock, iBlendMode, iOpacity, iRects, iPos, iWaitList);
}

TArray<::ULIS::FEvent>
FOdysseyTextureLayerImageVectorImageRenderer::Copy(TSharedPtr<::ULIS::FBlock> ioBlock, const TArray<::ULIS::FRectI>& iRects, const TArray<::ULIS::FVec2I>& iPos, const TArray<::ULIS::FEvent>& iWaitList)
{
    if (!mBlock)
        return iWaitList;

    mEngine->Render( mIsColored ? 0 : FOdysseyVectorObject::DRAWING_IGNORECOLOR );

    // HUD displaying only for the current layer.
    if( mRenderHUD )
        mEngine->RenderHUD();

    return ConvertAndCopy(mBlock, ioBlock, iRects, iPos, iWaitList);
}