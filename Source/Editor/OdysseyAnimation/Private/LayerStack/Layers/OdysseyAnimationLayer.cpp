// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/Layers/OdysseyAnimationLayer.h"

#include "LayerStack/OdysseyAnimationLayerStack.h"
#include "ULISLoaderModule.h"

#define LOCTEXT_NAMESPACE "OdysseyAnimationLayer"

//===========================

UOdysseyAnimation*
UOdysseyAnimationLayer::GetAnimation()
{
    UOdysseyAnimationLayerStack* layerStack = Cast<UOdysseyAnimationLayerStack>(GetLayerStack());
    if(!layerStack)
        return nullptr;

    return layerStack->GetAnimation();
}

FInt32Range
UOdysseyAnimationLayer::GetFrameRange() const
{
    TArray<FInt32Range> ranges;
    const TArray<UOdysseyLayer*> layers = Children;
    for (UOdysseyLayer* layer : layers)
    {
        UOdysseyAnimationLayer* animationLayer = Cast<UOdysseyAnimationLayer>(layer);
        if (!animationLayer)
            continue;

        ranges.Add(animationLayer->GetFrameRange());
    }

    return FInt32Range::Hull(ranges);
}

void
UOdysseyAnimationLayer::ChildrenChanged()
{
    Super::ChildrenChanged();

    TSharedPtr<IOdysseyAnimationImageRenderingAbility> imageRenderAbility = GetAbility<IOdysseyAnimationImageRenderingAbility>();
    if ( imageRenderAbility )
    {
        imageRenderAbility->CompositionChanged(imageRenderAbility->GetId());
        imageRenderAbility->CompositionCommited(imageRenderAbility->GetId());
    }
}

void
UOdysseyAnimationLayer::IsActivatedChanged()
{
    Super::IsActivatedChanged();

    UOdysseyAnimationLayer* parentLayer = Cast<UOdysseyAnimationLayer>(GetParent());
    if (!parentLayer)
        return;
    
    TSharedPtr<IOdysseyAnimationImageRenderingAbility> imageRenderAbility = parentLayer->GetAbility<IOdysseyAnimationImageRenderingAbility>();
    if ( imageRenderAbility )
    {
        imageRenderAbility->CompositionChanged(imageRenderAbility->GetId());
        imageRenderAbility->CompositionCommited(imageRenderAbility->GetId());
    }
}

#undef LOCTEXT_NAMESPACE
