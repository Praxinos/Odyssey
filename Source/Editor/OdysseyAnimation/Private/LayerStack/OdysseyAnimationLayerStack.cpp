// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/OdysseyAnimationLayerStack.h"
#include "LayerStack/Layers/LayerRoot/OdysseyAnimationLayerRoot.h"
#include "LayerStack/Layers/LayerFolder/OdysseyAnimationLayerFolder.h"
#include "LayerStack/Layers/LayerImageRaster/OdysseyAnimationLayerImageRaster.h"
#include "LayerStack/Layers/LayerImageVector/OdysseyAnimationLayerImageVector.h"
#include "OdysseyRectUtils.h"
#include "OdysseyAnimation.h"
#include "LayerStack/Cells/OdysseyAnimationCellSelection.h"

//===============================================

UOdysseyAnimationLayerStack::UOdysseyAnimationLayerStack()
    : mCellSelection(MakeShared<FOdysseyAnimationCellSelection>(this))
{
    CompatibleLayers.Add(UOdysseyAnimationLayerFolder::StaticClass());
    CompatibleLayers.Add(UOdysseyAnimationLayerImageRaster::StaticClass());
    CompatibleLayers.Add(UOdysseyAnimationLayerImageVector::StaticClass());

    LayerRootClass = UOdysseyAnimationLayerRoot::StaticClass();
}

UOdysseyAnimation*
UOdysseyAnimationLayerStack::GetAnimation() const
{
    UObject* outer = GetOuter();
    while(outer)
    {
        if (outer->GetClass() == UOdysseyAnimation::StaticClass())
            return Cast<UOdysseyAnimation>(outer);

        outer = outer->GetOuter();
    }

    return nullptr;
}

FInt32Range
UOdysseyAnimationLayerStack::GetFrameRange() const
{
    return Cast<UOdysseyAnimationLayerRoot>(LayerRoot)->GetFrameRange();
}

TArray<::ULIS::FRectI>
UOdysseyAnimationLayerStack::GetImageRenderingRects() const
{
    UOdysseyAnimation* animation = GetAnimation();
    if (!animation)
        return {};

    return animation->GetImageRenderingRects();
}

TSharedRef<FOdysseyAnimationCellSelection>
UOdysseyAnimationLayerStack::GetCellSelection() const
{
    return mCellSelection;
}
