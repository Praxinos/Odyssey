// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/Layers/LayerImageRaster/OdysseyAnimationLayerImageRasterImageRenderer.h"
#include "LayerStack/LightTable/OdysseyAnimationLightTableImageRenderer.h"
#include "LayerStack/Layers/LayerImageRaster/OdysseyAnimationLayerImageRaster.h"
#include "LayerStack/Cells/OdysseyAnimationCell.h"

FOdysseyAnimationLayerImageRasterImageRenderer::FOdysseyAnimationLayerImageRasterImageRenderer(const UOdysseyAnimationLayerImageRaster* iLayer, int iFrame, IOdysseyImageRenderer::eRenderType iRenderType, const TArray<::ULIS::FRectI>& iDefaultRects, FImageRendererFilter iFilter)
    : IOdysseyImageRenderer(iRenderType, iDefaultRects)
    , mCellRenderer(nullptr)
    , mBlendMode(::ULIS::eBlendMode(iLayer->BlendMode))
    , mOpacity(iLayer->Opacity) 
    , mLightTableDisplayPosition(iLayer->Lighttable.DisplayPosition)
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

    UOdysseyAnimationCell* cell = iLayer->GetCellAtFrame(frame);
    if (cell)
    {
        int cellFrame = frame - cell->GetFrameRange().GetLowerBoundValue();
        mCellRenderer = cell->BuildImageRenderer(iRenderType, cellFrame, iFilter);
    }

    if ( iRenderType == IOdysseyImageRenderer::eRenderType::Editor && iLayer->Lighttable.bIsActivated )
        mLightTableRenderer = MakeShared<FOdysseyAnimationLightTableImageRenderer>(iLayer, iFrame, iRenderType, iDefaultRects, iFilter);
}
    
void
FOdysseyAnimationLayerImageRasterImageRenderer::Init()
{
    if (mCellRenderer)
        mCellRenderer->Init();

    if (mLightTableRenderer)
        mLightTableRenderer->Init();
}

TArray<::ULIS::FEvent>
FOdysseyAnimationLayerImageRasterImageRenderer::Blend(const FOdysseyImageRendererBlendParams& iParams, const TArray<::ULIS::FEvent>& iWaitList)
{
    TRACE_CPUPROFILER_EVENT_SCOPE(FOdysseyAnimationLayerImageRasterImageRenderer::Blend);
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
FOdysseyAnimationLayerImageRasterImageRenderer::Copy(const FOdysseyImageRendererCopyParams& iParams, const TArray<::ULIS::FEvent>& iWaitList)
{
    TRACE_CPUPROFILER_EVENT_SCOPE(FOdysseyAnimationLayerImageRasterImageRenderer::Copy);
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
FOdysseyAnimationLayerImageRasterImageRenderer::IsGameThreadOnly()
{
    bool isGameThreadOnly = false;

    if (mCellRenderer)
        isGameThreadOnly |= mCellRenderer->IsGameThreadOnly();

    if (mLightTableRenderer)
        isGameThreadOnly |= mLightTableRenderer->IsGameThreadOnly();

    return isGameThreadOnly;
}
