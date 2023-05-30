// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/Layers/LayerImageRaster/OdysseyAnimationLayerImageRasterImageRenderingAbility.h"

FOdysseyAnimationLayerImageRasterImageRenderingAbility::FOdysseyAnimationLayerImageRasterImageRenderingAbility(UOdysseyAnimationLayerImageRaster* iLayerImageRaster)
    : mLayerImageRaster(iLayerImageRaster)
{
}

TSharedPtr<IOdysseyImageRenderer>
FOdysseyAnimationLayerImageRasterImageRenderingAbility::BuildRenderer(int iFrame) const
{
    return MakeShared<FOdysseyAnimationLayerImageRasterImageRenderer>(mLayerImageRaster, iFrame);
}

TArray<FGuid>
FOdysseyAnimationLayerImageRasterImageRenderingAbility::GetComposition(int iFrameIndex) const
{
    TArray<FGuid> idComposition = { GetId() };
    if (!mLayerImageRaster )
        return idComposition;

    int celFrameIndex = INDEX_NONE;
    TSharedPtr<FOdysseyAnimationCell> cell = mLayerImageRaster->GetCellAtFrame(iFrameIndex, celFrameIndex);
    if (cell)
    {
        TSharedPtr<IOdysseyAnimationImageRenderingAbility> cellAbility = cell->GetAbility<IOdysseyAnimationImageRenderingAbility>();
        if (cellAbility)
        {
            idComposition.Append(cellAbility->GetComposition(celFrameIndex));
        }
    }

    return idComposition;
}

TSharedPtr<IOdysseyHandle>
FOdysseyAnimationLayerImageRasterImageRenderingAbility::Preload(int iFrameIndex) const
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
            handles.Add(cellAbility->Preload(celFrameIndex));
        }
    }

    return MakeShared<FOdysseyHandleContainer>(handles);
}