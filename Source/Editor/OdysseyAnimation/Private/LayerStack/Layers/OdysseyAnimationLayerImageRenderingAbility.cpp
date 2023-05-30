// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/Layers/OdysseyAnimationLayerImageRenderingAbility.h"
#include "OdysseyRectUtils.h"

FOdysseyAnimationLayerImageRenderingAbility::FOdysseyAnimationLayerImageRenderingAbility(UOdysseyAnimationLayer* iLayer)
    : mLayer(iLayer)
{
}

TSharedPtr<IOdysseyImageRenderer>
FOdysseyAnimationLayerImageRenderingAbility::BuildRenderer(int iFrame) const
{
    return MakeShared<FOdysseyAnimationLayerImageRenderer>(mLayer, iFrame);
}

TArray<FGuid>
FOdysseyAnimationLayerImageRenderingAbility::GetComposition(int iFrameIndex) const
{
    TArray<FGuid> idComposition = { GetId() };
    if (!mLayer)
        return idComposition;

    const TArray<UOdysseyLayer*>& children = mLayer->GetChildren();
    for (UOdysseyLayer* child : children)
    {
        UOdysseyAnimationLayer* animationChild = Cast<UOdysseyAnimationLayer>(child);
        if (!animationChild)
            continue;

        if (!animationChild->IsActivated)
            continue;

        TSharedPtr<IOdysseyAnimationImageRenderingAbility> layerAbility = animationChild->GetAbility<IOdysseyAnimationImageRenderingAbility>();
        if (!layerAbility)
            continue;

        idComposition.Append(layerAbility->GetComposition(iFrameIndex));
    }

    return idComposition;
}

TSharedPtr<IOdysseyHandle>
FOdysseyAnimationLayerImageRenderingAbility::Preload(int iFrame) const
{
    if (!mLayer)
        return nullptr;

    const TArray<UOdysseyLayer*>& children = mLayer->GetChildren();
    if (children.IsEmpty())
        return nullptr;

    TArray<TSharedPtr<IOdysseyHandle>> handles;

    for (UOdysseyLayer* child : children)
    {
        UOdysseyAnimationLayer* animationChild = Cast<UOdysseyAnimationLayer>(child);
        if (!animationChild)
            continue;

        if (!animationChild->IsActivated)
            continue;

        TSharedPtr<IOdysseyAnimationImageRenderingAbility> layerAbility = animationChild->GetAbility<IOdysseyAnimationImageRenderingAbility>();
        if (!layerAbility)
            continue;

        handles.Add(layerAbility->Preload(iFrame));
    }

    return MakeShared<FOdysseyHandleContainer>(handles);
}