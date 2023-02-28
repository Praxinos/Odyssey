// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyAnimationLayerRoot.h"

UOdysseyAnimationLayerRoot::UOdysseyAnimationLayerRoot()
{
    CanHaveChildren = true;
}

void
UOdysseyAnimationLayerRoot::RenderImageChanged(const TRange<int>& iFrameRange, const TArray<::ULIS::FRectI>& iRects, bool iIsInteractive)
{
    UOdysseyAnimationLayerStack* layerStack = Cast<UOdysseyAnimationLayerStack>(GetLayerStack());

    //inform the layerstack directly first, then send a event for others
    if (layerStack)
        layerStack->OnRootLayerRenderImageChanged(this, iFrame, iRects, true);
    OnRenderImageChanged().Broadcast(this, iFrame, iRects, true);

    if (!iIsInteractive)
    {
        //inform the layerstack directly first, then send a event for others
        if (layerStack)
            layerStack->OnRootLayerRenderImageChanged(this, iFrame, iRects, false);
        OnRenderImageChanged().Broadcast(this, iFrame, iRects, false);
    }
}