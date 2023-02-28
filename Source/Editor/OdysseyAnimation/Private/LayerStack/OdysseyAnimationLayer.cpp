// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/OdysseyAnimationLayer.h"

#include "LayerStack/OdysseyAnimationLayerStack.h"

#define LOCTEXT_NAMESPACE "OdysseyAnimationLayer"


UOdysseyAnimationLayer::FOnRenderImageChanged&
UOdysseyAnimationLayer::OnRenderImageChanged()
{
    static FOnRenderImageChanged onRenderImageChanged;
    return onRenderImageChanged;
}

TArray<::ULIS::FEvent>
UOdysseyAnimationLayer::RenderLayersImage(TArray<UOdysseyAnimationLayer*> iLayers, TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> ioBlock, int iFrame, const ::ULIS::FRectI& iRect, const ::ULIS::FVec2I& iPos, const TArray<::ULIS::FEvent>& iWaitList)
{
    if (!ioBlock)
        return iWaitList;

    if (iLayers.Num() <= 0)
        return iWaitList;

    //Clear the block before blending on it
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext( ioBlock->Format() );
    ::ULIS::FEvent eventClearBlock;
    ctx.Clear( *ioBlock, ::ULIS::FRectI::FromXYWH(iPos.x, iPos.y, iRect.w, iRect.h), ::ULIS::FSchedulePolicy::AsyncCacheEfficient, iWaitList.Num(), iWaitList.GetData(), &eventClearBlock );

    TArray<::ULIS::FEvent> lastEvent = { eventClearBlock };
    for (int i = iLayers.Num() - 1; i >= 0 ; i--)
    {
        UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(iLayers[i]);
        if (!layer)
            continue;

        lastEvent = layer->RenderImage(ioBlock, iFrame, iRect, iPos, lastEvent);
    }
    
    ctx.Flush();

    return lastEvent;
}

TArray<::ULIS::FEvent>
UOdysseyAnimationLayer::RenderImage(TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> ioBlock, int iFrame, const ::ULIS::FRectI& iRect, const ::ULIS::FVec2I& iPos, const TArray<::ULIS::FEvent>& iWaitList)
{
    return iWaitList;
}

TArray<::ULIS::FEvent>
UOdysseyAnimationLayer::CopyImage(TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> ioBlock, int iFrame, const ::ULIS::FRectI& iRect, const ::ULIS::FVec2I& iPos, const TArray<::ULIS::FEvent>& iWaitList)
{
    return iWaitList;
}

void
UOdysseyAnimationLayer::RenderImageChanged(bool iIsInteractive)
{
    UOdysseyAnimation* animation = GetAnimation();
    if (!animation)
        return;

    RenderImageChanged(GetFrameRange(), ::ULIS::FRectI::FromXYWH(0, 0, animation->Width, animation->Height), iIsInteractive);
}

void
UOdysseyAnimationLayer::RenderImageChanged(const TRange<int>& iFrameRange, bool iIsInteractive)
{
    UOdysseyAnimation* animation = GetAnimation();
    if (!animation)
        return;

    RenderImageChanged(iFrameRange, { ::ULIS::FRectI::FromXYWH(0, 0, animation->Width, animation->Height) }, iIsInteractive);
}

void
UOdysseyAnimationLayer::RenderImageChanged(const TArray<::ULIS::FRectI>& iRects, bool iIsInteractive)
{
    UOdysseyAnimation* animation = GetAnimation();
    if (!animation)
        return;

    RenderImageChanged(GetFrameRange(), iRects, iIsInteractive);
}

void
UOdysseyAnimationLayer::RenderImageChanged(const TRange<int>& iFrameRange, const TArray<::ULIS::FRectI>& iRects, bool iIsInteractive)
{
    UOdysseyAnimationLayerStack* layerStack = Cast<UOdysseyAnimationLayerStack>(GetLayerStack());
    if (!layerStack)
        return;

    if ( !Parent )
        return;

    //PATCH BEGIN: because Unreal Undo does not make package dirty correctly after a save
    MarkPackageDirty();
    //PATCH END:

    UOdysseyAnimationLayer* parent = Cast<UOdysseyAnimationLayer>(Parent);
    if (parent)
        parent->ChildRenderImageChanged(this, iFrameRange, iRects, iIsInteractive);
    
    OnRenderImageChanged().Broadcast(this, iFrameRange, iRects, true); //always send at least 1 interactive event
    if (!iIsInteractive )
        OnRenderImageChanged().Broadcast(this, iFrameRange, iRects, false);
}

void
UOdysseyAnimationLayer::ChildRenderImageChanged(UOdysseyAnimationLayer* iLayer, const TRange<int>& iFrameRange, const TArray<::ULIS::FRectI>& iRects, bool iIsInteractive)
{
    RenderImageChanged(iFrameRange, iRects, iIsInteractive);
}

void
UOdysseyAnimationLayer::IsActivatedChanged()
{
    Super::IsActivatedChanged();
    RenderImageChanged(false);
}

void
UOdysseyAnimationLayer::ChildrenChanged()
{
    Super::ChildrenChanged();

    //TODO: Find a way to invalid only frame ranges that actually changed, instead of the while layer
    //Maybe that should be made by the child layer, so it can inform its old and new parent about its range and size

    RenderImageChanged(false);
}

void
UOdysseyAnimationLayer::Preload(int iFrame, TArray<TSharedPtr<IOdysseyHandle>>& oHandles)
{
    const TArray<UOdysseyLayer*>& layers = Children;
    for (UOdysseyLayer* layer : layers)
    {
        UOdysseyAnimationLayer* animationLayer = Cast<UOdysseyAnimationLayer>(layer);
        if (!animationLayer)
            continue;

        animationLayer->Preload(iFrame, oHandles);
    }
}

#undef LOCTEXT_NAMESPACE
