// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "LayerStack/LightTable/OdysseyAnimationLightTableImageRenderer.h"

FOdysseyAnimationLightTableImageRenderer::FOdysseyAnimationLightTableImageRenderer(TSharedRef<const FOdysseyAnimationLightTable> iLightTable, int iFrame, IOdysseyImageRenderer::eRenderType iRenderType, const TArray<::ULIS::FRectI>& iDefaultRects)
    : IOdysseyImageRenderer(iRenderType, iDefaultRects)
{
    const TArray<FOdysseyAnimationLightTable::FKeyData>& keysData = iLightTable->GetKeysData();
    for (int i = 0; i < keysData.Num(); i++)
    {
        if (!iLightTable->GetKeyIsActivated(i))
            continue;

        FFrameData data;
        data.mOpacity = iLightTable->GetKeyOpacity(i);
        data.mDisplayMode = iLightTable->GetKeyDisplayMode(i);
        data.mColor = iLightTable->GetKeyColor(i);
        data.mRenderer = iLightTable->GetSourceLayer()->BuildImageRenderer(IOdysseyImageRenderer::eRenderType::Render, iFrame + iLightTable->GetKeyOffset(i));
        mFramesData.Add(data);
    }
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
