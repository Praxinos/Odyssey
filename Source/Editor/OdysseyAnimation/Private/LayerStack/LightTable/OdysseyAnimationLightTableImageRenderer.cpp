// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "LayerStack/LightTable/OdysseyAnimationLightTableImageRenderer.h"

FOdysseyAnimationLightTableImageRenderer::FOdysseyAnimationLightTableImageRenderer(TSharedRef<const FOdysseyAnimationLightTable> iLightTable, int iFrame, IOdysseyImageRenderer::eRenderType iRenderType, const TArray<::ULIS::FRectI>& iDefaultRects)
    : IOdysseyImageRenderer(iRenderType, iDefaultRects)
{

    TSharedPtr<FOdysseyAnimationCellsContainer> cellsContainer = iLightTable->GetSourceLayer()->GetCellsContainer();
    int currentCellIndex = cellsContainer->GetCellIndexAtFrame(iFrame);
    if (currentCellIndex == INDEX_NONE)
        return;

    const TArray<TSharedPtr<FOdysseyAnimationCell>>& cells = cellsContainer->GetCells();
    if (currentCellIndex == INDEX_NONE)
        return;

    const TMap<int, FOdysseyAnimationLightTable::FKeyData>& keysData = iLightTable->GetKeysData();
    for (int i = -1; i >= -iLightTable->GetRange(); i--)
    {
        int cellIndex = currentCellIndex + i;
        if (cellIndex < 0 || cellIndex >= cells.Num())
            continue;

        TSharedPtr<FOdysseyAnimationCell> cell = cells[cellIndex];

        if (!keysData[i].mIsActivated)
            continue;

        int cellFirstFrame = cellsContainer->GetCellFrame(cell);

        FFrameData data;
        data.mOpacity = keysData[i].mOpacity;
        data.mRenderer = iLightTable->GetSourceLayer()->BuildImageRenderer(IOdysseyImageRenderer::eRenderType::Render, cellFirstFrame);
        mFramesData.Add(data);
    }

    for (int i = 1; i <= iLightTable->GetRange(); i++)
    {
        int cellIndex = currentCellIndex + i;
        if (cellIndex < 0 || cellIndex >= cells.Num())
            continue;

        TSharedPtr<FOdysseyAnimationCell> cell = cells[cellIndex];

        if (!keysData[i].mIsActivated)
            continue;

        int cellFirstFrame = cellsContainer->GetCellFrame(cell);

        FFrameData data;
        data.mOpacity = keysData[i].mOpacity;
        data.mRenderer = iLightTable->GetSourceLayer()->BuildImageRenderer(IOdysseyImageRenderer::eRenderType::Render, cellFirstFrame);
        mFramesData.Add(data);
    }
}
    
void
FOdysseyAnimationLightTableImageRenderer::Init()
{
    for (const FFrameData& frameData : mFramesData)
    {
        frameData.mRenderer->Init();
    }
}

bool
FOdysseyAnimationLightTableImageRenderer::IsGameThreadOnly()
{
    for (const FFrameData& frameData : mFramesData)
    {
        if (frameData.mRenderer->IsGameThreadOnly())
            return true;
    }
    return false;
}

TArray<::ULIS::FEvent>
FOdysseyAnimationLightTableImageRenderer::Blend(TSharedPtr<::ULIS::FBlock> ioBlock, ::ULIS::eBlendMode iBlendMode, float iOpacity, const TArray<::ULIS::FRectI>& iRects, const TArray<::ULIS::FVec2I>& iPos, const TArray<::ULIS::FEvent>& iWaitList)
{   
    TArray<::ULIS::FEvent> lastEvent = iWaitList;
    for (const FFrameData& frameData : mFramesData)
    {
        lastEvent = frameData.mRenderer->Blend(ioBlock, ::ULIS::Blend_Normal, frameData.mOpacity, iRects, iPos, lastEvent);
    }
    return lastEvent;
}

TArray<::ULIS::FEvent>
FOdysseyAnimationLightTableImageRenderer::Copy(TSharedPtr<::ULIS::FBlock> ioBlock, const TArray<::ULIS::FRectI>& iRects, const TArray<::ULIS::FVec2I>& iPos, const TArray<::ULIS::FEvent>& iWaitList)
{
    TArray<::ULIS::FEvent> clearEvents = Clear(ioBlock, iRects, iPos, iWaitList);
    TArray<::ULIS::FEvent> lastEvent = clearEvents;
    for (const FFrameData& frameData : mFramesData)
    {
        lastEvent = frameData.mRenderer->Blend(ioBlock, ::ULIS::Blend_Normal, frameData.mOpacity, iRects, iPos, lastEvent);
    }
    return lastEvent;
}
