// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/OdysseyAnimationLayerStackImageRenderingAbility.h"

FOdysseyAnimationLayerStackImageRenderingAbility::FOdysseyAnimationLayerStackImageRenderingAbility(UOdysseyAnimationLayerStack* iLayerStack)
    : mLayerStack(iLayerStack)
{
}

TSharedPtr<IOdysseyImageRenderer>
FOdysseyAnimationLayerStackImageRenderingAbility::BuildRenderer(int iFrame) const
{
    return MakeShared<FOdysseyAnimationLayerStackImageRenderer>(mLayerStack, iFrame);
}

TArray<FGuid>
FOdysseyAnimationLayerStackImageRenderingAbility::GetComposition(int iFrameIndex) const
{
    TArray<FGuid> idComposition = { GetId() };
    if (!mLayerStack)
        return idComposition;

    UOdysseyAnimationLayer* layerRoot = Cast<UOdysseyAnimationLayer>(mLayerStack->LayerRoot);
    if ( !layerRoot )
        return idComposition;

    TSharedPtr<IOdysseyAnimationImageRenderingAbility> layerRootAbility = layerRoot->GetAbility<IOdysseyAnimationImageRenderingAbility>();
    if (!layerRootAbility)
        return idComposition;
    
    idComposition.Append(layerRootAbility->GetComposition(iFrameIndex));
    return idComposition;
}

TSharedPtr<IOdysseyHandle>
FOdysseyAnimationLayerStackImageRenderingAbility::Preload(int iFrame) const
{
    if ( !mLayerStack )
        return nullptr;

    UOdysseyAnimationLayer* layerRoot = Cast<UOdysseyAnimationLayer>(mLayerStack->LayerRoot);
    if ( !layerRoot )
        return nullptr;

    TSharedPtr<IOdysseyAnimationImageRenderingAbility> layerRootAbility = layerRoot->GetAbility<IOdysseyAnimationImageRenderingAbility>();
    if (!layerRootAbility)
        return nullptr;

    return layerRootAbility->Preload(iFrame);
}
