// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyAnimationLayer.h"

#include "OdysseyAnimationLayerStack.h"
#include "OdysseyAnimationCell.h"

#if WITH_EDITOR
#include "UObject/OdysseyObjectEditorUtils.h"
#endif

//===========================

UOdysseyAnimation*
UOdysseyAnimationLayer::GetAnimation() const
{
    UOdysseyAnimationLayerStack* layerStack = Cast<UOdysseyAnimationLayerStack>(GetLayerStack());
    if(!layerStack)
        return nullptr;

    return layerStack->GetAnimation();
}

UOdysseyAnimationLayerStack*
UOdysseyAnimationLayer::GetLayerStack() const
{
    return Cast<UOdysseyAnimationLayerStack>(UOdysseyLayer::GetLayerStack());
}
