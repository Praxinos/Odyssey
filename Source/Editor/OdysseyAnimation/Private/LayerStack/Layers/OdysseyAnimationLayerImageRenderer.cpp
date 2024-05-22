// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyAnimationLayerImageRenderer.h"
#include "OdysseyRectUtils.h"
#include "LayerStack/Layers/OdysseyAnimationLayer.h"

FOdysseyAnimationLayerImageRenderer::FOdysseyAnimationLayerImageRenderer(const UOdysseyAnimationLayer* iLayer, int iFrame, IOdysseyImageRenderer::eRenderType iRenderType, const TArray<::ULIS::FRectI>& iDefaultRects, FImageRendererFilter iFilter)
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
        data.mRenderer = child->BuildImageRenderer(iRenderType, iFrame, iFilter);
        data.mBlendMode = child->GetImageRenderingBlendMode();
        data.mOpacity = child->GetImageRenderingOpacity();

        if (!data.mRenderer)
            continue;
            
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
FOdysseyAnimationLayerImageRenderer::Blend(const FOdysseyImageRendererBlendParams& iParams, const TArray<::ULIS::FEvent>& iWaitList)
{
    TArray<::ULIS::FEvent> events;
    for (const ::ULIS::FRectI& rect : iParams.mRects)
    {
        TSharedPtr<::ULIS::FBlock> childrenBlock = MakeShared<::ULIS::FBlock>(rect.w, rect.h, iParams.mBlock->Format());
        ::ULIS::FRectI childrenBlockRect = childrenBlock->Rect();
        ::ULIS::FVec2I childrenBlockPos(iParams.mPos.x + rect.x, iParams.mPos.y + rect.y);
        TArray<::ULIS::FEvent> clearEvents = Clear(childrenBlock, { childrenBlockRect }, {});
        clearEvents.Append(iWaitList);

        TArray<::ULIS::FEvent> lastEvent = clearEvents;
        for (const FChildData& childData : mChildrenData)
        {
            FOdysseyImageRendererBlendParams params(iParams);
            params.mBlock = childrenBlock;
            params.mRects = { childrenBlockRect };
            params.mPos = childrenBlockPos;
            params.mBlendMode = childData.mBlendMode;
            params.mOpacity = childData.mOpacity;

            lastEvent = childData.mRenderer->Blend(params, lastEvent);
        }

        FOdysseyImageRendererBlendParams params(iParams);
        params.mRects = { rect };
        params.mTransform = ::ULIS::FMat3F();
        lastEvent = ConvertAndBlend(childrenBlock, childrenBlockPos, params, lastEvent);

        events.Append(lastEvent);
    }

    return events;
}

TArray<::ULIS::FEvent>
FOdysseyAnimationLayerImageRenderer::Copy(const FOdysseyImageRendererCopyParams& iParams, const TArray<::ULIS::FEvent>& iWaitList)
{
    TRACE_CPUPROFILER_EVENT_SCOPE(FOdysseyAnimationLayerImageRenderer::Copy);
    TArray<::ULIS::FEvent> clearEvents = Clear(iParams.mBlock, iParams.mRects, iWaitList);
    TArray<::ULIS::FEvent> lastEvent = clearEvents;
    for (const FChildData& childData : mChildrenData)
    {
        lastEvent = childData.mRenderer->Blend(iParams, lastEvent);
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