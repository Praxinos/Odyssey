// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyAnimationLayerImageRenderer.h"
#include "OdysseyRectUtils.h"

FOdysseyAnimationLayerImageRenderer::FOdysseyAnimationLayerImageRenderer(const UOdysseyAnimationLayer* iLayer, int iFrame, IOdysseyImageRenderer::eRenderType iRenderType, const TArray<::ULIS::FRectI>& iDefaultRects)
    : IOdysseyImageRenderer(iRenderType, iDefaultRects)
{    
    const TArray<UOdysseyLayer*>& children = iLayer->GetChildren();
    for (int i = children.Num() - 1; i >= 0 ; i--)
    {
        UOdysseyAnimationLayer* child = Cast<UOdysseyAnimationLayer>(children[i]);
        if (!child)
            continue;

        if (!child->IsActivated)
            continue;

        FChildData data;
        data.mRenderer = child->BuildImageRenderer(iRenderType, iFrame);
        data.mBlendMode = child->GetImageRenderingBlendMode();
        data.mOpacity = child->GetImageRenderingOpacity();

        mChildrenData.Add(data);
    }
}

void
FOdysseyAnimationLayerImageRenderer::Init()
{
    for (const FChildData& childData : mChildrenData)
    {
        childData.mRenderer->Init();
    }
}

TArray<::ULIS::FEvent>
FOdysseyAnimationLayerImageRenderer::Blend(TSharedPtr<::ULIS::FBlock> ioBlock, ::ULIS::eBlendMode iBlendMode, float iOpacity, const TArray<::ULIS::FRectI>& iRects, const TArray<::ULIS::FVec2I>& iPos, const TArray<::ULIS::FEvent>& iWaitList)
{
    if (!ioBlock)
        return iWaitList;

    TArray<::ULIS::FEvent> events;
    for (int i = 0; i < iRects.Num(); i++)
    {
        const ::ULIS::FRectI& rect = iRects[i];
        const ::ULIS::FVec2I& pos = iPos[i];
        TSharedPtr<::ULIS::FBlock> childrenBlock = MakeShared<::ULIS::FBlock>(rect.w, rect.h, ioBlock->Format());
        ::ULIS::FRectI childrenBlockRect = childrenBlock->Rect();
        ::ULIS::FVec2I childrenBlockPos(0);
        TArray<::ULIS::FEvent> clearEvents = Clear(childrenBlock, { childrenBlockRect }, { childrenBlockPos }, {});
        clearEvents.Append(iWaitList);

        TArray<::ULIS::FEvent> lastEvent = clearEvents;
        for (const FChildData& childData : mChildrenData)
        {
            lastEvent = childData.mRenderer->Blend(childrenBlock, childData.mBlendMode, childData.mOpacity, { rect }, { childrenBlockPos }, lastEvent);
        }

        lastEvent = ConvertAndBlend(childrenBlock, ioBlock, iBlendMode, iOpacity, { childrenBlockRect }, { pos }, lastEvent);

        events.Append(lastEvent);
    }

    return events;
}

TArray<::ULIS::FEvent>
FOdysseyAnimationLayerImageRenderer::Copy(TSharedPtr<::ULIS::FBlock> ioBlock, const TArray<::ULIS::FRectI>& iRects, const TArray<::ULIS::FVec2I>& iPos, const TArray<::ULIS::FEvent>& iWaitList)
{
    if (!ioBlock)
        return iWaitList;

    TArray<::ULIS::FEvent> clearEvents = Clear(ioBlock, iRects, iPos, iWaitList);
    TArray<::ULIS::FEvent> lastEvent = clearEvents;
    for (const FChildData& childData : mChildrenData)
    {
        lastEvent = childData.mRenderer->Blend(ioBlock, childData.mBlendMode, childData.mOpacity, iRects, iPos, lastEvent);
    }
    return lastEvent;
}

bool
FOdysseyAnimationLayerImageRenderer::IsGameThreadOnly()
{
    for (const FChildData& childData : mChildrenData)
    {
        if (childData.mRenderer->IsGameThreadOnly())
            return true;
    }
    return false;
}