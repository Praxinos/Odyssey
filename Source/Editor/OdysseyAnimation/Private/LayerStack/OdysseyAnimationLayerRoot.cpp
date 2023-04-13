// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/OdysseyAnimationLayerRoot.h"

UOdysseyAnimationLayerRoot::UOdysseyAnimationLayerRoot()
{
    CanHaveChildren = true;
}

void
UOdysseyAnimationLayerRoot::RenderImageDataChanged(const FOdysseyAnimationRenderImageId& iFrameId, const TArray<::ULIS::FRectI>& iRects)
{
    UOdysseyAnimationLayerStack* layerStack = Cast<UOdysseyAnimationLayerStack>(GetLayerStack());

    //inform the layerstack directly first, then send a event for others
    if (layerStack)
        layerStack->OnRootLayerRenderImageChanged(this, iFrameId, iRects);

    OnRenderImageChanged().Broadcast(this, iFrameId, iRects);

    if (!iIsInteractive)
    {
        //inform the layerstack directly first, then send a event for others
        if (layerStack)
            layerStack->OnRootLayerRenderImageChanged(this, iFrameId, iRects);
        OnRenderImageChanged().Broadcast(this, iFrameId, iRects);
    }
}