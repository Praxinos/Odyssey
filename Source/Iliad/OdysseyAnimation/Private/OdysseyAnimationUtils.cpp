// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyAnimationUtils.h"

#include "OdysseyAnimation.h"
#include "OdysseyLayerStack.h"
#include "OdysseyLayer.h"
#include "OdysseyLayerCell.h"

namespace Odyssey
{

namespace AnimationUtils
{

UOdysseyAnimation* GetLayerStackAnimation(UOdysseyLayerStack* Layerstack)
{
    UObject* outer = Layerstack->GetOuter();
    while(outer)
    {
        if (outer->GetClass() == UOdysseyAnimation::StaticClass())
            return Cast<UOdysseyAnimation>(outer);

        outer = outer->GetOuter();
    }

    return nullptr;
}

UOdysseyAnimation* GetLayerAnimation(UOdysseyLayer* Layer)
{
    return GetLayerStackAnimation(Layer->GetLayerStack());
}

UOdysseyAnimation* GetCellAnimation(UOdysseyLayerCell* Cell)
{
    return GetLayerAnimation(Cell->GetLayer());
}

} // namespace AnimationUtils

} // namespace Odyssey
