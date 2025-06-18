// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

class UOdysseyAnimation;
class UOdysseyLayerStack;
class UOdysseyLayer;
class UOdysseyLayerCell;

namespace Odyssey
{
    namespace AnimationUtils
    {
        ODYSSEYANIMATION_API UOdysseyAnimation* GetLayerStackAnimation(UOdysseyLayerStack* Layerstack);
        ODYSSEYANIMATION_API UOdysseyAnimation* GetLayerAnimation(UOdysseyLayer* Layer);
        ODYSSEYANIMATION_API UOdysseyAnimation* GetCellAnimation(UOdysseyLayerCell* Cell);
    }
}
