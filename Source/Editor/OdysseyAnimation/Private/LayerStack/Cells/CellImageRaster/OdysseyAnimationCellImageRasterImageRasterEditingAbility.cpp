// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/Cells/CellImageRaster/OdysseyAnimationCellImageRasterImageRasterEditingAbility.h"

FOdysseyAnimationCellImageRasterImageRasterEditingAbility::FOdysseyAnimationCellImageRasterImageRasterEditingAbility(TSharedPtr<FOdysseyAnimationCellImageRaster> iCellImageRaster)
    : mCellImageRaster(iCellImageRaster)
{
}

TSharedPtr<FOdysseyRasterBlock>
FOdysseyAnimationCellImageRasterImageRasterEditingAbility::GetRasterBlock(uint32 iFrameIndex) const
{
    TSharedPtr<FOdysseyAnimationCellImageRaster> cell = mCellImageRaster.Pin();
    if (!cell)
        return nullptr;

    return cell->GetRasterBlock();
}