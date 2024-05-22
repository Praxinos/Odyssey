// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "LayerStack/Layers/LayerImageVector/OdysseyAnimationLayerImageVectorImageRenderer.h"
#include "LayerStack/LightTable/OdysseyAnimationLightTableImageRenderer.h"
#include "LayerStack/Layers/LayerImageVector/OdysseyAnimationLayerImageVector.h"
#include "LayerStack/Cells/OdysseyAnimationCellsContainer.h"
#include "LayerStack/Cells/OdysseyAnimationCell.h"

FOdysseyAnimationLayerImageVectorImageRenderer::FOdysseyAnimationLayerImageVectorImageRenderer(const UOdysseyAnimationLayerImageVector* iLayer, int iFrame, IOdysseyImageRenderer::eRenderType iRenderType, const TArray<::ULIS::FRectI>& iDefaultRects, FImageRendererFilter iFilter)
    : IOdysseyImageRenderer(iRenderType, iDefaultRects)
    , mCellRenderer(nullptr)
    , mBlendMode(::ULIS::eBlendMode(iLayer->BlendMode))
    , mOpacity(iLayer->Opacity) 
    , mLightTableDisplayPosition(iLayer->GetLightTable()->GetDisplayPosition())
{ 
    int frame = iFrame;
    FInt32Range frameRange = iLayer->GetFrameRange();
    if (iFrame < frameRange.GetLowerBoundValue())
    {
        frame = iLayer->GetPreBehaviourFrame(iLayer->PreBehaviour, iFrame);
    }
    else if (iFrame > frameRange.GetUpperBoundValue())
    {
        frame = iLayer->GetPostBehaviourFrame(iLayer->PostBehaviour, iFrame);
    }

    TSharedPtr<FOdysseyAnimationCell> cell = iLayer->GetCellsContainer()->GetCellAtFrame(frame);
    int cellFrameIndex = iLayer->GetCellsContainer()->GetCellFrameAtFrame(frame);
    if (cell && cellFrameIndex != INDEX_NONE)
        mCellRenderer = cell->BuildImageRenderer(iRenderType, cellFrameIndex, iFilter);

    if ( iRenderType == IOdysseyImageRenderer::eRenderType::Editor && iLayer->bIsLightTableActivated )
        mLightTableRenderer = MakeShared<FOdysseyAnimationLightTableImageRenderer>(iLayer->GetLightTable().ToSharedRef(), iFrame, iRenderType, iDefaultRects, iFilter);
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
FOdysseyAnimationLayerImageVectorImageRenderer::Blend(const FOdysseyImageRendererBlendParams& iParams, const TArray<::ULIS::FEvent>& iWaitList)
{
    TArray<::ULIS::FEvent> events = iWaitList;
    if (!mCellRenderer)
        return events;
        
    if ( mLightTableRenderer && mLightTableDisplayPosition == EOdysseyLightTableDisplayPosition::UnderLayer )
    {
        FOdysseyImageRendererBlendParams lightTableParams(iParams);
        lightTableParams.mBlendMode = ::ULIS::Blend_Normal;
        lightTableParams.mOpacity = 1.f;
        events = mLightTableRenderer->Blend(lightTableParams, events);
    }

    events = mCellRenderer->Blend(iParams, events);

    if ( mLightTableRenderer && mLightTableDisplayPosition == EOdysseyLightTableDisplayPosition::AboveLayer )
    {
        FOdysseyImageRendererBlendParams lightTableParams(iParams);
        lightTableParams.mBlendMode = ::ULIS::Blend_Normal;
        lightTableParams.mOpacity = 1.f;
        events = mLightTableRenderer->Blend(lightTableParams, events);
    }

    return events;
}

TArray<::ULIS::FEvent>
FOdysseyAnimationLayerImageVectorImageRenderer::Copy(const FOdysseyImageRendererCopyParams& iParams, const TArray<::ULIS::FEvent>& iWaitList)
{
    if (!mCellRenderer)
        return iWaitList;
        
    TArray<::ULIS::FEvent> events = Clear(iParams.mBlock, iParams.mRects, iWaitList);
    if ( mLightTableRenderer && mLightTableDisplayPosition == EOdysseyLightTableDisplayPosition::UnderLayer )
    {
        FOdysseyImageRendererBlendParams lightTableParams(iParams);
        lightTableParams.mBlendMode = ::ULIS::Blend_Normal;
        lightTableParams.mOpacity = 1.f;
        events = mLightTableRenderer->Blend(lightTableParams, events);
    }

    events = mCellRenderer->Blend(iParams, events);

    if ( mLightTableRenderer && mLightTableDisplayPosition == EOdysseyLightTableDisplayPosition::AboveLayer )
    {
        FOdysseyImageRendererBlendParams lightTableParams(iParams);
        lightTableParams.mBlendMode = ::ULIS::Blend_Normal;
        lightTableParams.mOpacity = 1.f;
        events = mLightTableRenderer->Blend(lightTableParams, events);
    }

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