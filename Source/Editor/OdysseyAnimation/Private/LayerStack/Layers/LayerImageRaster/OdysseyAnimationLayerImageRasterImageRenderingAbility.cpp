// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/Layers/LayerImageRaster/OdysseyAnimationLayerImageRasterImageRenderingAbility.h"

FOdysseyAnimationLayerImageRasterImageRenderingAbility::FOdysseyAnimationLayerImageRasterImageRenderingAbility(UOdysseyAnimationLayerImageRaster* iLayerImageRaster)
    : mLayerImageRaster(iLayerImageRaster)
{
}

TSharedPtr<IOdysseyImageRenderer>
FOdysseyAnimationLayerImageRasterImageRenderingAbility::BuildRenderer(int iFrame, IOdysseyImageRenderer::eRenderType iRenderType) const
{
    return MakeShared<FOdysseyAnimationLayerImageRasterImageRenderer>(mLayerImageRaster, iFrame, iRenderType, GetRects());
}

TArray<FGuid>
FOdysseyAnimationLayerImageRasterImageRenderingAbility::GetComposition(int iFrameIndex, IOdysseyImageRenderer::eRenderType iRenderType) const
{
    TArray<FGuid> idComposition = { GetId() };
    if (!mLayerImageRaster )
        return idComposition;

    TSharedPtr<FOdysseyAnimationLightTable> lightTable = mLayerImageRaster->GetLightTable();
    if (iRenderType == IOdysseyImageRenderer::eRenderType::Editor && mLayerImageRaster->bIsLightTableActivated && lightTable->GetDisplayPosition() == EOdysseyLightTableDisplayPosition::UnderLayer )
    {
        TSharedPtr<IOdysseyAnimationImageRenderingAbility> lightTableAbility = lightTable->GetAbility<IOdysseyAnimationImageRenderingAbility>();
        idComposition.Append(lightTableAbility->GetComposition(iFrameIndex, iRenderType));
    }

    int celFrameIndex = INDEX_NONE;
    TSharedPtr<FOdysseyAnimationCell> cell = mLayerImageRaster->GetCellAtFrame(iFrameIndex, celFrameIndex);
    if (cell)
    {
        TSharedPtr<IOdysseyAnimationImageRenderingAbility> cellAbility = cell->GetAbility<IOdysseyAnimationImageRenderingAbility>();
        if (cellAbility)
        {
            idComposition.Append(cellAbility->GetComposition(celFrameIndex, iRenderType));
        }
    }

    if (iRenderType == IOdysseyImageRenderer::eRenderType::Editor && mLayerImageRaster->bIsLightTableActivated && lightTable->GetDisplayPosition() == EOdysseyLightTableDisplayPosition::AboveLayer )
    {
        TSharedPtr<IOdysseyAnimationImageRenderingAbility> lightTableAbility = lightTable->GetAbility<IOdysseyAnimationImageRenderingAbility>();
        idComposition.Append(lightTableAbility->GetComposition(iFrameIndex, iRenderType));
    }

    return idComposition;
}

TSharedPtr<IOdysseyHandle>
FOdysseyAnimationLayerImageRasterImageRenderingAbility::Preload(int iFrameIndex, IOdysseyImageRenderer::eRenderType iRenderType) const
{
    if (!mLayerImageRaster )
        return nullptr;

    int celFrameIndex = INDEX_NONE;
    TSharedPtr<FOdysseyAnimationCell> cell = mLayerImageRaster->GetCellAtFrame(iFrameIndex, celFrameIndex);
    TArray<TSharedPtr<IOdysseyHandle>> handles;
    if (cell)
    {
        TSharedPtr<IOdysseyAnimationImageRenderingAbility> cellAbility = cell->GetAbility<IOdysseyAnimationImageRenderingAbility>();
        if (cellAbility)
        {
            handles.Add(cellAbility->Preload(celFrameIndex, iRenderType));
        }
    }

    if (iRenderType == IOdysseyImageRenderer::eRenderType::Editor && mLayerImageRaster->bIsLightTableActivated)
    {
        TSharedPtr<FOdysseyAnimationLightTable> lightTable = mLayerImageRaster->GetLightTable();
        TSharedPtr<IOdysseyAnimationImageRenderingAbility> lightTableAbility = lightTable->GetAbility<IOdysseyAnimationImageRenderingAbility>();
        handles.Add(lightTableAbility->Preload(iFrameIndex, iRenderType));

    }

    return MakeShared<FOdysseyHandleContainer>(handles);
}

TArray<::ULIS::FRectI>
FOdysseyAnimationLayerImageRasterImageRenderingAbility::GetRects() const
{
    if (!mLayerImageRaster )
        return {};

    UOdysseyAnimation* animation = mLayerImageRaster->GetAnimation();
    if (!animation)
        return {};

    return { ::ULIS::FRectI::FromXYWH(0, 0, animation->Width(), animation->Height()) };
}