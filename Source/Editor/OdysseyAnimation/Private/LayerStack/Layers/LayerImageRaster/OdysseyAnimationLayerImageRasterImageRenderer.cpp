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
    , mIsLightTableActivated(iLayer->bIsLightTableActivated)
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

    if ( iRenderType == IOdysseyImageRenderer::eRenderType::Editor && mIsLightTableActivated )
        mLightTableRenderer = MakeShared<FOdysseyAnimationLightTableImageRenderer>(iLayer->GetLightTable(), iFrame, iRenderType, iDefaultRects);
}

TArray<::ULIS::FEvent>
FOdysseyAnimationLayerImageRasterImageRenderer::RenderInBlock(TSharedPtr<::ULIS::FBlock> ioBlock, const TArray<::ULIS::FRectI>& iRects, const TArray<::ULIS::FVec2I>& iPos, const TArray<::ULIS::FEvent>& iWaitList)
{
    if (!mCellRenderer)
        return iWaitList;
        
    TArray<::ULIS::FEvent> events = iWaitList;
    if ( GetRenderType() == IOdysseyImageRenderer::eRenderType::Editor && mIsLightTableActivated && mLightTableDisplayPosition == EOdysseyLightTableDisplayPosition::UnderLayer )
        events = mLightTableRenderer->RenderOverBlock(ioBlock, iRects, iPos, events);

    events = mCellRenderer->RenderInBlock(ioBlock, iRects, iPos, events);

    if ( GetRenderType() == IOdysseyImageRenderer::eRenderType::Editor && mIsLightTableActivated && mLightTableDisplayPosition == EOdysseyLightTableDisplayPosition::AboveLayer )
        events = mLightTableRenderer->RenderOverBlock(ioBlock, iRects, iPos, events);

    return events;
}

TArray<::ULIS::FEvent>
FOdysseyAnimationLayerImageRasterImageRenderer::RenderOverBlock(TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> ioBlock, const TArray<::ULIS::FRectI>& iRects, const TArray<::ULIS::FVec2I>& iPos, const TArray<::ULIS::FEvent>& iWaitList)
{
    if (!mCellRenderer)
        return iWaitList;

    if (!ioBlock)
        return iWaitList;

    TArray<::ULIS::FEvent> lightTableEvents = iWaitList;
    if ( GetRenderType() == IOdysseyImageRenderer::eRenderType::Editor && mIsLightTableActivated && mLightTableDisplayPosition == EOdysseyLightTableDisplayPosition::UnderLayer )
        lightTableEvents = mLightTableRenderer->RenderOverBlock(ioBlock, iRects, iPos, iWaitList);

    ::ULIS::eFormat format = ioBlock->Format();
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(format);

    TArray<::ULIS::FEvent> events;
    for (int i = 0; i < iRects.Num(); i++)
    {
        //Generate the folder block, which is all children layers blended together
        TArray<::ULIS::FEvent> rasterBlockEvent;
        TSharedPtr<::ULIS::FBlock> rasterBlock = mCellRenderer->RenderInNewBlock(format, iRects[i], rasterBlockEvent);
        if(!rasterBlock)
            continue;

        rasterBlockEvent.Append(lightTableEvents);

        TArray<::ULIS::FEvent> eventConvertAndExecute = ULISUtils::ConvertAndExecute(ioBlock, rasterBlock->Format(), rasterBlock->Rect(), iPos[i], rasterBlockEvent,
            [this, &rasterBlock, &ctx](TSharedPtr<::ULIS::FBlock> ioDest, const ::ULIS::FRectI& iRect, const ::ULIS::FVec2I& iPos, const TArray<::ULIS::FEvent>& iWaitList) -> TArray<::ULIS::FEvent>
            {
                ::ULIS::FEvent eventBlend = FULISEventBuilder()
                    .RetainBlock(rasterBlock)
                    .RetainBlock(ioDest)
                    .Build();

                ctx.Blend(
                    *rasterBlock,
                    *ioDest,
                    iRect,
                    iPos,
                    mBlendMode,
                    ::ULIS::Alpha_Normal,
                    mOpacity,
                    ::ULIS::FSchedulePolicy::AsyncCacheEfficient,
                    iWaitList.Num(),
                    iWaitList.GetData(),
                    &eventBlend
                );
                return { eventBlend };
            }
        );

        events.Append(eventConvertAndExecute);
    }

    ctx.Flush();

    if ( GetRenderType() == IOdysseyImageRenderer::eRenderType::Editor && mIsLightTableActivated && mLightTableDisplayPosition == EOdysseyLightTableDisplayPosition::AboveLayer )
        events = mLightTableRenderer->RenderOverBlock(ioBlock, iRects, iPos, events);

    return events;
}