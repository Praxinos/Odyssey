// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "LayerStack/OdysseyTextureLayerImageVectorImageRenderer.h"
#include "OdysseyVectorBlock.h"

FOdysseyTextureLayerImageVectorImageRenderer::FOdysseyTextureLayerImageVectorImageRenderer(const UOdysseyTextureLayerImageVector* iLayer, TSharedPtr<FOdysseyVectorBlock> iVectorBlock, IOdysseyImageRenderer::eRenderType iRenderType, const TArray<::ULIS::FRectI>& iDefaultRects)
    : IOdysseyImageRenderer(iRenderType, iDefaultRects)
    , mVectorBlock(iVectorBlock)
    , mBlock(nullptr)
    , mHUDBlock(nullptr)
    , mRenderHUD(false)
    , mDrawingFlags(0)
{
    UOdysseyLayerStack* layerStack = iLayer->GetLayerStack();
    if (!layerStack)
        return;

    mRenderHUD = layerStack->CurrentLayer.Get() == iLayer;

    // this is per-layer
    mDrawingFlags  = iLayer->IsColored   ? mDrawingFlags & (~FOdysseyVectorEngine::DRAWING_IGNORECOLOR )
                                         : mDrawingFlags | ( FOdysseyVectorEngine::DRAWING_IGNORECOLOR );
    // TEMP: this should be global, stored in PainterEditor. Hence this should be
    // changed when PainterEditor will be available and we can retrieve the shared flags.
    // Update: commented-out for now
    mDrawingFlags |= iLayer->IsWireframe ? mDrawingFlags | ( FOdysseyVectorEngine::DRAWING_WIREFRAME)
                                         : mDrawingFlags & (~FOdysseyVectorEngine::DRAWING_WIREFRAME);
}

void
FOdysseyTextureLayerImageVectorImageRenderer::Init()
{
    mBlock = mVectorBlock->GetBlock(mDrawingFlags);
    if (mRenderHUD)
        mHUDBlock = mVectorBlock->GetHUDBlock();
    
    mVectorBlock->Render(mDrawingFlags);
}

bool
FOdysseyTextureLayerImageVectorImageRenderer::IsGameThreadOnly()
{
    return false;
}

TArray<::ULIS::FEvent>
FOdysseyTextureLayerImageVectorImageRenderer::Blend(TSharedPtr<::ULIS::FBlock> ioBlock, ::ULIS::eBlendMode iBlendMode, float iOpacity, const TArray<::ULIS::FRectI>& iRects, const TArray<::ULIS::FVec2I>& iPos, const TArray<::ULIS::FEvent>& iWaitList)
{
    if (!mBlock)
        return iWaitList;

    TArray<::ULIS::FEvent> events = ConvertAndBlend(mBlock, ioBlock, iBlendMode, iOpacity, iRects, iPos, iWaitList);
    
    if (!mHUDBlock)
        return events;

    return ConvertAndBlend(mHUDBlock, ioBlock, ::ULIS::Blend_Normal, 1.f, iRects, iPos, events);
}

TArray<::ULIS::FEvent>
FOdysseyTextureLayerImageVectorImageRenderer::Copy(TSharedPtr<::ULIS::FBlock> ioBlock, const TArray<::ULIS::FRectI>& iRects, const TArray<::ULIS::FVec2I>& iPos, const TArray<::ULIS::FEvent>& iWaitList)
{
    if (!mBlock)
        return iWaitList;

    return ConvertAndCopy(mBlock, ioBlock, iRects, iPos, iWaitList);
}
