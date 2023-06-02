// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "LayerStack/Layers/OdysseyAnimationLayerImageRenderer.h"
#include "OdysseyRectUtils.h"

FOdysseyAnimationLayerImageRenderer::FOdysseyAnimationLayerImageRenderer(UOdysseyAnimationLayer* iLayer, int iFrame, IOdysseyImageRenderer::eRenderType iRenderType, const TArray<::ULIS::FRectI>& iDefaultRects)
    : IOdysseyImageRenderer(iRenderType, iDefaultRects)
{    
    const TArray<UOdysseyLayer*>& children = iLayer->GetChildren();
    for (int i = children.Num() - 1; i >= 0 ; i--)
    {
        UOdysseyAnimationLayer* animationChild = Cast<UOdysseyAnimationLayer>(children[i]);
        if (!animationChild)
            continue;

        if (!animationChild->IsActivated)
            continue;

        TSharedPtr<IOdysseyAnimationImageRenderingAbility> layerAbility = animationChild->GetAbility<IOdysseyAnimationImageRenderingAbility>();
        if (!layerAbility)
            continue;

        mChildrenRenderers.Add(layerAbility->BuildRenderer(iFrame, iRenderType));
    }
}
/*
TArray<::ULIS::FRectI>
FOdysseyAnimationLayerImageRenderer::GetRects() const
{
    TArray<::ULIS::FRectI> rects;
    for (TSharedPtr<IOdysseyImageRenderer> childRenderer : mChildrenRenderers)
    {
        rects.Append(childRenderer->GetRects());
    }
    return OdysseyRectUtils::MergeRects(rects);
}
*/

TArray<::ULIS::FEvent>
FOdysseyAnimationLayerImageRenderer::RenderInBlock(TSharedPtr<::ULIS::FBlock> ioBlock, const TArray<::ULIS::FRectI>& iRects, const TArray<::ULIS::FVec2I>& iPos, const TArray<::ULIS::FEvent>& iWaitList)
{
    if (!ioBlock)
        return iWaitList;

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext( ioBlock->Format() );

    TArray<::ULIS::FEvent> lastEvent = iWaitList;

    for (TSharedPtr<IOdysseyImageRenderer> childRenderer : mChildrenRenderers)
    {
        lastEvent = childRenderer->RenderOverBlock(ioBlock, iRects, iPos, lastEvent);
    }
    
    ctx.Flush();

    return lastEvent;
}
