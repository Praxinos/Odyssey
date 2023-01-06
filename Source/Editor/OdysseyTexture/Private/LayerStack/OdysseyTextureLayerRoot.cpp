// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyTextureLayerRoot.h"

UOdysseyTextureLayerRoot::UOdysseyTextureLayerRoot()
{
    CanHaveChildren = true;
}

void
UOdysseyTextureLayerRoot::RenderImageChanged(const TArray<::ULIS::FRectI>& iRects, bool iIsInteractive)
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetLayerStack());
    if (!layerStack)
        return;

    layerStack->OnRootLayerRenderImageChanged(this, iRects, iIsInteractive);
    OnRenderImageChanged().Broadcast(this, iRects, iIsInteractive);
}