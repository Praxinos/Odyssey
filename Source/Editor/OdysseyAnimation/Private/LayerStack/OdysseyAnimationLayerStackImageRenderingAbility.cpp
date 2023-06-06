// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/OdysseyAnimationLayerStackImageRenderingAbility.h"

FOdysseyAnimationLayerStackImageRenderingAbility::FOdysseyAnimationLayerStackImageRenderingAbility(UOdysseyAnimationLayerStack* iLayerStack)
    : mLayerStack(iLayerStack)
{
}

TSharedPtr<IOdysseyImageRenderer>
FOdysseyAnimationLayerStackImageRenderingAbility::BuildRenderer(int iFrame, IOdysseyImageRenderer::eRenderType iRenderType) const
{
    return MakeShared<FOdysseyAnimationLayerStackImageRenderer>(mLayerStack, iFrame, iRenderType, GetRects());
}

TArray<FGuid>
FOdysseyAnimationLayerStackImageRenderingAbility::GetComposition(int iFrameIndex, IOdysseyImageRenderer::eRenderType iRenderType) const
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
    
    idComposition.Append(layerRootAbility->GetComposition(iFrameIndex, iRenderType));
    return idComposition;
}

TSharedPtr<IOdysseyHandle>
FOdysseyAnimationLayerStackImageRenderingAbility::Preload(int iFrame, IOdysseyImageRenderer::eRenderType iRenderType) const
{
    if ( !mLayerStack )
        return nullptr;

    UOdysseyAnimationLayer* layerRoot = Cast<UOdysseyAnimationLayer>(mLayerStack->LayerRoot);
    if ( !layerRoot )
        return nullptr;

    TSharedPtr<IOdysseyAnimationImageRenderingAbility> layerRootAbility = layerRoot->GetAbility<IOdysseyAnimationImageRenderingAbility>();
    if (!layerRootAbility)
        return nullptr;

    return layerRootAbility->Preload(iFrame, iRenderType);
}

TArray<::ULIS::FRectI>
FOdysseyAnimationLayerStackImageRenderingAbility::GetRects() const
{
    if (!mLayerStack )
        return {};

    UOdysseyAnimation* animation = mLayerStack->GetAnimation();
    if (!animation)
        return {};

    return { ::ULIS::FRectI::FromXYWH(0, 0, animation->Width(), animation->Height()) };
}