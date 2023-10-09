// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/Layers/OdysseyAnimationLayer.h"

#include "LayerStack/OdysseyAnimationLayerStack.h"
#include "OdysseyAnimationLayerImageRenderer.h"
#include "ULISLoaderModule.h"

#define LOCTEXT_NAMESPACE "OdysseyAnimationLayer"

//===========================

UOdysseyAnimation*
UOdysseyAnimationLayer::GetAnimation() const
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

TSharedPtr<FOdysseyAnimationLightTable>
UOdysseyAnimationLayer::GetLightTable() const
{
    return nullptr;
}

void
UOdysseyAnimationLayer::ChildrenChanged()
{
    Super::ChildrenChanged();

    ImageRenderingCompositionChanged();
}

void
UOdysseyAnimationLayer::IsActivatedChanged()
{
    Super::IsActivatedChanged();

    UOdysseyAnimationLayer* parentLayer = Cast<UOdysseyAnimationLayer>(GetParent());
    if (!parentLayer)
        return;

    parentLayer->ImageRenderingCompositionChanged();
}

TSharedPtr<IOdysseyImageRenderer>
UOdysseyAnimationLayer::BuildImageRenderer(IOdysseyImageRenderer::eRenderType iRenderType, int iFrame) const
{
    return MakeShared<FOdysseyAnimationLayerImageRenderer>(this, iFrame, iRenderType, GetImageRenderingRects());
}

TArray<FGuid>
UOdysseyAnimationLayer::GetImageRenderingComposition(IOdysseyImageRenderer::eRenderType iRenderType, int iFrame) const
{
    TArray<FGuid> idComposition = { GetImageRenderingId() };

    const TArray<UOdysseyLayer*>& children = GetChildren();
    for (UOdysseyLayer* child : children)
    {
        if (!child->IsActivated)
            continue;

        UOdysseyAnimationLayer* animationChild = Cast<UOdysseyAnimationLayer>(child);
        if (!animationChild)
            continue;

        idComposition.Append(animationChild->GetImageRenderingComposition(iRenderType, iFrame));
    }

    return idComposition;
}

TArray<::ULIS::FRectI>
UOdysseyAnimationLayer::GetImageRenderingRects() const
{
    UOdysseyAnimationLayerStack* layerStack = Cast<UOdysseyAnimationLayerStack>(GetLayerStack());
    if(!layerStack)
        return {};

    return layerStack->GetImageRenderingRects();
}

#undef LOCTEXT_NAMESPACE
