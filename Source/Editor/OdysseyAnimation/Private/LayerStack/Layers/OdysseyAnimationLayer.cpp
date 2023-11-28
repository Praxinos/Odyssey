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

bool
UOdysseyAnimationLayer::GetIsLightTableActivated() const
{
    return false;
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

void
UOdysseyAnimationLayer::PreBehaviourChanged()
{
    ImageRenderingCompositionChanged();
}

void
UOdysseyAnimationLayer::PostBehaviourChanged()
{
    ImageRenderingCompositionChanged();
}

void
UOdysseyAnimationLayer::PropertyChanged(const FName& iPropertyName)
{
    Super::PropertyChanged(iPropertyName);

    if (iPropertyName == "PreBehaviour")
        PreBehaviourChanged();
    if (iPropertyName == "PostBehaviour")
        PostBehaviourChanged();
}

TSharedPtr<IOdysseyImageRenderer>
UOdysseyAnimationLayer::BuildImageRenderer(IOdysseyImageRenderer::eRenderType iRenderType, int iFrame, FImageRendererFilter iFilter) const
{
    if (iFilter.IsBound() && !iFilter.Execute(this))
        return nullptr;
    
    return MakeShared<FOdysseyAnimationLayerImageRenderer>(this, iFrame, iRenderType, GetImageRenderingRects(), iFilter);
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

int
UOdysseyAnimationLayer::GetPreBehaviourFrame(EOdysseyAnimationLayerImagePostBehaviour iBehaviour, int iFrame) const
{
    FInt32Range frameRange = GetFrameRange();

    //PreBehaviour
    int frame = iFrame;
    switch(iBehaviour)
    {
        default:
        case EOdysseyAnimationLayerImagePostBehaviour::None:
        break;

        case EOdysseyAnimationLayerImagePostBehaviour::Hold:
        {
            frame = frameRange.GetLowerBoundValue();
        }
        break;
        case EOdysseyAnimationLayerImagePostBehaviour::Loop:
        {
            int offsetFromStart = frameRange.GetLowerBoundValue() - iFrame;
            int layerLength = frameRange.GetUpperBoundValue() - frameRange.GetLowerBoundValue() + 1;
            frame = frameRange.GetUpperBoundValue() - ((offsetFromStart - 1) % layerLength);
        }
        break;
        case EOdysseyAnimationLayerImagePostBehaviour::PingPong:
        {
            int offsetFromStart = frameRange.GetLowerBoundValue() - iFrame;
            int layerLength = frameRange.GetUpperBoundValue() - frameRange.GetLowerBoundValue() + 1;

            int offset = ((offsetFromStart - 1) % (layerLength - 1)) + 1;
            int forwardFrame = frameRange.GetLowerBoundValue() + offset;
            int backwardFrame = frameRange.GetUpperBoundValue() - offset;

            int direction = layerLength > 0 ? ((offsetFromStart - 1) / (layerLength - 1)) % 2 : 0;
            frame = direction == 0 ? forwardFrame : backwardFrame;
        }
        break;
    }

    return frame;
}

int
UOdysseyAnimationLayer::GetPostBehaviourFrame(EOdysseyAnimationLayerImagePostBehaviour iBehaviour, int iFrame) const
{
    //PreBehaviour
    FInt32Range frameRange = GetFrameRange();

    int frame = iFrame;
    switch(iBehaviour)
    {
        default:
        case EOdysseyAnimationLayerImagePostBehaviour::None:
        break;

        case EOdysseyAnimationLayerImagePostBehaviour::Hold:
        {
            frame = frameRange.GetUpperBoundValue();
        }
        break;
        case EOdysseyAnimationLayerImagePostBehaviour::Loop:
        {
            int offsetFromEnd = iFrame - frameRange.GetUpperBoundValue();
            int layerLength = frameRange.GetUpperBoundValue() - frameRange.GetLowerBoundValue() + 1;
            frame = frameRange.GetLowerBoundValue() + (offsetFromEnd - 1) % layerLength;
        }
        break;
        case EOdysseyAnimationLayerImagePostBehaviour::PingPong:
        {
            int offsetFromEnd = iFrame - frameRange.GetUpperBoundValue();
            int layerLength = frameRange.GetUpperBoundValue() - frameRange.GetLowerBoundValue() + 1;

            int offset = ((offsetFromEnd - 1) % (layerLength - 1)) + 1;
            int forwardFrame = frameRange.GetLowerBoundValue() + offset;
            int backwardFrame = frameRange.GetUpperBoundValue() - offset;

            int direction = layerLength > 0 ? ((offsetFromEnd - 1) / (layerLength - 1)) % 2 : 0;
            frame = direction == 0 ? backwardFrame : forwardFrame;
        }
        break;
    }

    return frame;
}

#undef LOCTEXT_NAMESPACE
