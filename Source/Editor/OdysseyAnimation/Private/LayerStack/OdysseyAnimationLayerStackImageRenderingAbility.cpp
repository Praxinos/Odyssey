// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/OdysseyAnimationLayerStackImageRenderingAbility.h"

FOdysseyAnimationLayerStackImageRenderingAbility::FOdysseyAnimationLayerStackImageRenderingAbility(UOdysseyAnimationLayerStack* iLayerStack)
    : mLayerStack(iLayerStack)
{
}

TArray<::ULIS::FRectI>
FOdysseyAnimationLayerStackImageRenderingAbility::GetRects(int iFrame) const
{
    if (!mLayerStack)
        return {};

    UOdysseyAnimationLayer* layerRoot = Cast<UOdysseyAnimationLayer>(mLayerStack->LayerRoot);
    if ( !layerRoot )
        return {};

    TSharedPtr<IOdysseyAnimationImageRenderingAbility> layerRootAbility = layerRoot->GetAbility<IOdysseyAnimationImageRenderingAbility>();
    if (!layerRootAbility)
        return {};

    return layerRootAbility->GetRects(iFrame);
}

TArray<::ULIS::FEvent>
FOdysseyAnimationLayerStackImageRenderingAbility::RenderInBlock(TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> ioBlock, int iFrame, const ::ULIS::FRectI& iRect, const ::ULIS::FVec2I& iPos, const TArray<::ULIS::FEvent>& iWaitList)
{
    if (!mLayerStack)
        return iWaitList;

    UOdysseyAnimationLayer* layerRoot = Cast<UOdysseyAnimationLayer>(mLayerStack->LayerRoot);
    if ( !layerRoot )
        return iWaitList;

    TSharedPtr<IOdysseyAnimationImageRenderingAbility> layerRootAbility = layerRoot->GetAbility<IOdysseyAnimationImageRenderingAbility>();
    if (!layerRootAbility)
        return iWaitList;

    return layerRootAbility->RenderInBlock(ioBlock, iFrame, iRect, iPos, iWaitList);
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
FOdysseyAnimationLayerStackImageRenderingAbility::Preload(int iFrame)
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
