// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyAnimationImageRenderingAbility.h"
#include "OdysseyAnimationProxyImageRenderer.h"

FOdysseyAnimationImageRenderingAbility::FOdysseyAnimationImageRenderingAbility(UOdysseyAnimation* iAnimation)
    : mAnimation(iAnimation)
{
}

TSharedPtr<IOdysseyImageRenderer>
FOdysseyAnimationImageRenderingAbility::BuildRenderer(int iFrame, bool iThreadSafe) const
{
    return MakeShared<FOdysseyAnimationProxyImageRenderer>(mAnimation, GetComposition(iFrame), iThreadSafe);
}

TArray<FGuid>
FOdysseyAnimationImageRenderingAbility::GetComposition(int iFrameIndex) const
{
    TArray<FGuid> idComposition = { GetId() };
    if (!mAnimation)
        return idComposition;

    UOdysseyAnimationLayerStack* layerStack = mAnimation->GetLayerStack();
    if (!layerStack)
        return idComposition;

    TSharedPtr<IOdysseyAnimationImageRenderingAbility> layerStackAbility = layerStack->GetAbility<IOdysseyAnimationImageRenderingAbility>();
    if (!layerStackAbility)
        return idComposition;

    idComposition.Append(layerStackAbility->GetComposition(iFrameIndex));

    return idComposition;
}

TSharedPtr<IOdysseyHandle>
FOdysseyAnimationImageRenderingAbility::Preload(int iFrame)
{
    if (!mAnimation)
        return nullptr;

    UOdysseyAnimationLayerStack* layerStack = mAnimation->GetLayerStack();
    if (!layerStack)
        return nullptr;

    TSharedPtr<IOdysseyAnimationImageRenderingAbility> layerStackAbility = layerStack->GetAbility<IOdysseyAnimationImageRenderingAbility>();
    if (!layerStackAbility)
        return nullptr;

    return layerStackAbility->Preload(iFrame);
}