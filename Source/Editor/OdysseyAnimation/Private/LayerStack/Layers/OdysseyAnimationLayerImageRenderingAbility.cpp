// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/Layers/OdysseyAnimationLayerImageRenderingAbility.h"
#include "OdysseyRectUtils.h"

FOdysseyAnimationLayerImageRenderingAbility::FOdysseyAnimationLayerImageRenderingAbility(UOdysseyAnimationLayer* iLayer)
    : mLayer(iLayer)
{
}

TArray<::ULIS::FRectI>
FOdysseyAnimationLayerImageRenderingAbility::GetRects(int iFrame) const
{
    if (!mLayer)
        return {};

    TArray<::ULIS::FRectI> rects;
    const TArray<UOdysseyLayer*>& children = mLayer->GetChildren();
    for (UOdysseyLayer* child : children)
    {
        UOdysseyAnimationLayer* animationChild = Cast<UOdysseyAnimationLayer>(child);
        if (!animationChild)
            continue;

        if (!animationChild->IsActivated)
            continue;

        TSharedPtr<IOdysseyAnimationImageRenderingAbility> layerAbility = animationChild->GetAbility<IOdysseyAnimationImageRenderingAbility>();
        if (!layerAbility)
            continue;

        rects.Append(layerAbility->GetRects(iFrame));
    }
    return OdysseyRectUtils::MergeRects(rects);
}

TArray<::ULIS::FEvent>
FOdysseyAnimationLayerImageRenderingAbility::RenderInBlock(TSharedPtr<::ULIS::FBlock> ioBlock, int iFrame, const ::ULIS::FRectI& iRect, const ::ULIS::FVec2I& iPos, const TArray<::ULIS::FEvent>& iWaitList)
{
    if (!mLayer)
        return iWaitList;

    if (!ioBlock)
        return iWaitList;

    const TArray<UOdysseyLayer*>& children = mLayer->GetChildren();
    if (children.IsEmpty())
        return iWaitList;

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext( ioBlock->Format() );    

    //Clear the block
    ::ULIS::FEvent eventClearBlock = FULISEventBuilder().RetainBlock(ioBlock).Build();
    ctx.Clear( *ioBlock, ::ULIS::FRectI::FromXYWH(iPos.x, iPos.y, iRect.w, iRect.h), ::ULIS::FSchedulePolicy::AsyncCacheEfficient, iWaitList.Num(), iWaitList.GetData(), &eventClearBlock );

    TArray<::ULIS::FEvent> lastEvent = {eventClearBlock};
    for (int i = children.Num() - 1; i >= 0 ; i--)
    {
        UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(children[i]);
        if (!layer)
            continue;

        if (!layer->IsActivated)
            continue;

        TSharedPtr<IOdysseyAnimationImageRenderingAbility> layerAbility = layer->GetAbility<IOdysseyAnimationImageRenderingAbility>();
        if (!layerAbility)
            continue;

        lastEvent = layerAbility->RenderOverBlock(ioBlock, iFrame, iRect, iPos, lastEvent);
    }
    
    ctx.Flush();

    return lastEvent;
}

TArray<FGuid>
FOdysseyAnimationLayerImageRenderingAbility::GetComposition(int iFrameIndex) const
{
    TArray<FGuid> idComposition = { GetId() };
    if (!mLayer)
        return idComposition;

    const TArray<UOdysseyLayer*>& children = mLayer->GetChildren();
    for (UOdysseyLayer* child : children)
    {
        UOdysseyAnimationLayer* animationChild = Cast<UOdysseyAnimationLayer>(child);
        if (!animationChild)
            continue;

        if (!animationChild->IsActivated)
            continue;

        TSharedPtr<IOdysseyAnimationImageRenderingAbility> layerAbility = animationChild->GetAbility<IOdysseyAnimationImageRenderingAbility>();
        if (!layerAbility)
            continue;

        idComposition.Append(layerAbility->GetComposition(iFrameIndex));
    }

    return idComposition;
}

TSharedPtr<IOdysseyHandle>
FOdysseyAnimationLayerImageRenderingAbility::Preload(int iFrame)
{
    if (!mLayer)
        return nullptr;

    const TArray<UOdysseyLayer*>& children = mLayer->GetChildren();
    if (children.IsEmpty())
        return nullptr;

    TArray<TSharedPtr<IOdysseyHandle>> handles;

    for (UOdysseyLayer* child : children)
    {
        UOdysseyAnimationLayer* animationChild = Cast<UOdysseyAnimationLayer>(child);
        if (!animationChild)
            continue;

        if (!animationChild->IsActivated)
            continue;

        TSharedPtr<IOdysseyAnimationImageRenderingAbility> layerAbility = animationChild->GetAbility<IOdysseyAnimationImageRenderingAbility>();
        if (!layerAbility)
            continue;

        handles.Add(layerAbility->Preload(iFrame));
    }

    return MakeShared<FOdysseyHandleContainer>(handles);
}