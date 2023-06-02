// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/LightTable/OdysseyAnimationLightTableImageRenderingAbility.h"

FOdysseyAnimationLightTableImageRenderingAbility::FOdysseyAnimationLightTableImageRenderingAbility(TSharedPtr<FOdysseyAnimationLightTable> iLightTable)
    : mLightTable(iLightTable)
{
}

TSharedPtr<IOdysseyImageRenderer>
FOdysseyAnimationLightTableImageRenderingAbility::BuildRenderer(int iFrame, IOdysseyImageRenderer::eRenderType iRenderType) const
{
    TSharedPtr<FOdysseyAnimationLightTable> lightTable = mLightTable.Pin();
    if (!lightTable)
        return nullptr;

    return MakeShared<FOdysseyAnimationLightTableImageRenderer>(lightTable, iFrame, iRenderType, GetRects());
}

TArray<FGuid>
FOdysseyAnimationLightTableImageRenderingAbility::GetComposition(int iFrameIndex, IOdysseyImageRenderer::eRenderType iRenderType) const
{
    TArray<FGuid> idComposition = { GetId() };
    
    TSharedPtr<FOdysseyAnimationLightTable> lightTable = mLightTable.Pin();
    if (!lightTable)
        return idComposition;

    UOdysseyAnimationLayer* layer = lightTable->GetSourceLayer();
    if(!layer)
        return idComposition;

    TSharedPtr<IOdysseyAnimationImageRenderingAbility> layerAbility = layer->GetAbility<IOdysseyAnimationImageRenderingAbility>();
    if (!layerAbility)
        return idComposition;

    const TArray<FOdysseyAnimationLightTable::FKeyData>& keysData = lightTable->GetKeysData();
    for (int i = 0; i < keysData.Num(); i++)
    {
        if (!lightTable->GetKeyIsActivated(i))
            continue;

        //Find the cell or frame 
        int offset = lightTable->GetKeyOffset(i);
        int celFrameIndex = INDEX_NONE;
        idComposition.Append(layerAbility->GetComposition(iFrameIndex + offset, IOdysseyImageRenderer::eRenderType::Render));
    }

    return idComposition;
}

TArray<::ULIS::FRectI>
FOdysseyAnimationLightTableImageRenderingAbility::GetRects() const
{
    TSharedPtr<FOdysseyAnimationLightTable> lightTable = mLightTable.Pin();
    if (!lightTable)
        return {};

    UOdysseyAnimationLayer* layer = lightTable->GetOwnerLayer();
    if (!layer)
        return {};

    UOdysseyAnimation* animation = layer->GetAnimation();
    if (!animation)
        return {};

    return { ::ULIS::FRectI::FromXYWH(0, 0, animation->Width(), animation->Height()) };
}


TSharedPtr<IOdysseyHandle>
FOdysseyAnimationLightTableImageRenderingAbility::Preload(int iFrame, IOdysseyImageRenderer::eRenderType iRenderType) const
{
    TSharedPtr<FOdysseyAnimationLightTable> lightTable = mLightTable.Pin();
    if (!lightTable)
        return nullptr;

    UOdysseyAnimationLayer* layer = lightTable->GetSourceLayer();
    if(!layer)
        return nullptr;

    TSharedPtr<IOdysseyAnimationImageRenderingAbility> layerAbility = layer->GetAbility<IOdysseyAnimationImageRenderingAbility>();
    if (!layerAbility)
        return nullptr;

    TArray<TSharedPtr<IOdysseyHandle>> handles;
    const TArray<FOdysseyAnimationLightTable::FKeyData>& keysData = lightTable->GetKeysData();
    for (int i = 0; i < keysData.Num(); i++)
    {
        if (!lightTable->GetKeyIsActivated(i))
            continue;

        //Find the cell or frame 
        int offset = lightTable->GetKeyOffset(i);
        handles.Add(layerAbility->Preload(iFrame + offset, IOdysseyImageRenderer::eRenderType::Render));
    }

    return MakeShared<FOdysseyHandleContainer>(handles);
}