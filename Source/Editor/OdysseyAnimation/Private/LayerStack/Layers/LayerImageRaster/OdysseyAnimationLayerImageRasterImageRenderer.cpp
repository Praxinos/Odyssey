// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "LayerStack/Layers/LayerImageRaster/OdysseyAnimationLayerImageRasterImageRenderer.h"
#include "LayerStack/LightTable/OdysseyAnimationLightTableImageRenderer.h"

FOdysseyAnimationLayerImageRasterImageRenderer::FOdysseyAnimationLayerImageRasterImageRenderer(UOdysseyAnimationLayerImageRaster* iLayer, int iFrame, IOdysseyImageRenderer::eRenderType iRenderType, const TArray<::ULIS::FRectI>& iDefaultRects)
    : IOdysseyImageRenderer(iRenderType, iDefaultRects)
    , mCellRenderer(nullptr)
    , mBlendMode(::ULIS::eBlendMode(iLayer->BlendMode))
    , mOpacity(iLayer->Opacity) 
    , mLightTableDisplayPosition(iLayer->GetLightTable()->GetDisplayPosition())
{    
    int cellIndex = INDEX_NONE;
    int cellFrameIndex = INDEX_NONE;
    if (!iLayer->GetCellIndexAtFrame(iFrame, cellIndex, cellFrameIndex))
        return;

    TSharedPtr<FOdysseyAnimationCell> cell = iLayer->GetCell(cellIndex);
    if (!cell)
        return;

    TSharedPtr<IOdysseyAnimationImageRenderingAbility> cellAbility = cell->GetAbility<IOdysseyAnimationImageRenderingAbility>();
    if (!cellAbility)
        return;

    mCellRenderer = cellAbility->BuildRenderer(cellFrameIndex, iRenderType);

    if ( iRenderType == IOdysseyImageRenderer::eRenderType::Editor && iLayer->bIsLightTableActivated )
        mLightTableRenderer = MakeShared<FOdysseyAnimationLightTableImageRenderer>(iLayer->GetLightTable(), iFrame, iRenderType, iDefaultRects);
}

TArray<::ULIS::FEvent>
FOdysseyAnimationLayerImageRasterImageRenderer::Blend(TSharedPtr<::ULIS::FBlock> ioBlock, ::ULIS::eBlendMode iBlendMode, float iOpacity, const TArray<::ULIS::FRectI>& iRects, const TArray<::ULIS::FVec2I>& iPos, const TArray<::ULIS::FEvent>& iWaitList)
{
    if (!mCellRenderer)
        return iWaitList;
        
    TArray<::ULIS::FEvent> events = iWaitList;
    if ( mLightTableRenderer && mLightTableDisplayPosition == EOdysseyLightTableDisplayPosition::UnderLayer )
        events = mLightTableRenderer->Blend(ioBlock, ::ULIS::Blend_Normal, 1.f, iRects, iPos, events);

    events = mCellRenderer->Blend(ioBlock, iBlendMode, iOpacity, iRects, iPos, events);

    if ( mLightTableRenderer && mLightTableDisplayPosition == EOdysseyLightTableDisplayPosition::AboveLayer )
        events = mLightTableRenderer->Blend(ioBlock, ::ULIS::Blend_Normal, 1.f, iRects, iPos, events);

    return events;
}

TArray<::ULIS::FEvent>
FOdysseyAnimationLayerImageRasterImageRenderer::Copy(TSharedPtr<::ULIS::FBlock> ioBlock, const TArray<::ULIS::FRectI>& iRects, const TArray<::ULIS::FVec2I>& iPos, const TArray<::ULIS::FEvent>& iWaitList)
{
    if (!mCellRenderer)
        return iWaitList;
        
    TArray<::ULIS::FEvent> events = Clear(ioBlock, iRects, iPos, iWaitList);
    if ( mLightTableRenderer && mLightTableDisplayPosition == EOdysseyLightTableDisplayPosition::UnderLayer )
        events = mLightTableRenderer->Blend(ioBlock, ::ULIS::Blend_Normal, 1.f, iRects, iPos, events);

    events = mCellRenderer->Blend(ioBlock, ::ULIS::Blend_Normal, 1.f, iRects, iPos, events);

    if ( mLightTableRenderer && mLightTableDisplayPosition == EOdysseyLightTableDisplayPosition::AboveLayer )
        events = mLightTableRenderer->Blend(ioBlock, ::ULIS::Blend_Normal, 1.f, iRects, iPos, events);

    return events;
}