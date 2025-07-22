// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyAnimationBlueprintLibrary.h"

#include "OdysseyAnimationUtils.h"

//static
UOdysseyAnimation*
UOdysseyAnimationBlueprintLibrary::GetLayerStackAnimation(UOdysseyLayerStack* Layerstack)
{
    return Odyssey::AnimationUtils::GetLayerStackAnimation(Layerstack);
}

//static
UOdysseyAnimation*
UOdysseyAnimationBlueprintLibrary::GetLayerAnimation(UOdysseyLayer* Layer)
{
    return Odyssey::AnimationUtils::GetLayerAnimation(Layer);
}

//static
UOdysseyAnimation*
UOdysseyAnimationBlueprintLibrary::GetCellAnimation(UOdysseyLayerCell* Cell)
{
    return Odyssey::AnimationUtils::GetCellAnimation(Cell);
}
