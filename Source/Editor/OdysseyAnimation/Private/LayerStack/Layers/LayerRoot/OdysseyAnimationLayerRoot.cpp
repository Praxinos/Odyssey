// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/Layers/LayerRoot/OdysseyAnimationLayerRoot.h"
#include "LayerStack/Layers/OdysseyAnimationLayerImageRenderingAbility.h"

UOdysseyAnimationLayerRoot::UOdysseyAnimationLayerRoot()
{
    CanHaveChildren = true;
}

void
UOdysseyAnimationLayerRoot::PostInitProperties()
{
    Super::PostInitProperties();

    SetAbility(MakeShared<FOdysseyAnimationLayerImageRenderingAbility>(this));
}