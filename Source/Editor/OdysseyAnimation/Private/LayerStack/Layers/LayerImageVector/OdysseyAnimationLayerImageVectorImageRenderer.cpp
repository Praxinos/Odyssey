// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "LayerStack/Layers/LayerImageVector/OdysseyAnimationLayerImageVectorImageRenderer.h"
#include "LayerStack/LightTable/OdysseyAnimationLightTableImageRenderer.h"

FOdysseyAnimationLayerImageVectorImageRenderer::FOdysseyAnimationLayerImageVectorImageRenderer(const UOdysseyAnimationLayerImageVector* iLayer, int iFrame, IOdysseyImageRenderer::eRenderType iRenderType, const TArray<::ULIS::FRectI>& iDefaultRects)
    : IOdysseyImageRenderer(iRenderType, iDefaultRects)
    , mCellRenderer(nullptr)
    , mBlendMode(::ULIS::eBlendMode(iLayer->BlendMode))
    , mOpacity(iLayer->Opacity) 
    , mLightTableDisplayPosition(iLayer->GetLightTable()->GetDisplayPosition())
{    
    TSharedPtr<FOdysseyAnimationCell> cell = iLayer->GetCellsContainer()->GetCellAtFrame(iFrame);
    int cellFrameIndex = iLayer->GetCellsContainer()->GetCellFrameAtFrame(iFrame);

    if (!cell || cellFrameIndex == INDEX_NONE)
        return;

    mCellRenderer = cell->BuildImageRenderer(iRenderType, cellFrameIndex);

    if ( iRenderType == IOdysseyImageRenderer::eRenderType::Editor && iLayer->bIsLightTableActivated )
        mLightTableRenderer = MakeShared<FOdysseyAnimationLightTableImageRenderer>(iLayer->GetLightTable().ToSharedRef(), iFrame, iRenderType, iDefaultRects);
}
    
void
FOdysseyAnimationLayerImageVectorImageRenderer::Init()
{
    if (mCellRenderer)
        mCellRenderer->Init();

    if (mLightTableRenderer)
        mLightTableRenderer->Init();
}

TArray<::ULIS::FEvent>
FOdysseyAnimationLayerImageVectorImageRenderer::Blend(TSharedPtr<::ULIS::FBlock> ioBlock, ::ULIS::eBlendMode iBlendMode, float iOpacity, const TArray<::ULIS::FRectI>& iRects, const TArray<::ULIS::FVec2I>& iPos, const TArray<::ULIS::FEvent>& iWaitList)
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
FOdysseyAnimationLayerImageVectorImageRenderer::Copy(TSharedPtr<::ULIS::FBlock> ioBlock, const TArray<::ULIS::FRectI>& iRects, const TArray<::ULIS::FVec2I>& iPos, const TArray<::ULIS::FEvent>& iWaitList)
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

bool
FOdysseyAnimationLayerImageVectorImageRenderer::IsGameThreadOnly()
{
    bool isGameThreadOnly = false;

    if (mCellRenderer)
        isGameThreadOnly |= mCellRenderer->IsGameThreadOnly();

    if (mLightTableRenderer)
        isGameThreadOnly |= mLightTableRenderer->IsGameThreadOnly();

    return isGameThreadOnly;
}