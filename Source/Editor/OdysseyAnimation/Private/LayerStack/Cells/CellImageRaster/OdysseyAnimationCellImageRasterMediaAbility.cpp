// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/Cells/CellImageRaster/OdysseyAnimationCellImageRasterMediaAbility.h"
#include "OdysseyRectUtils.h"
#include "OdysseyMediaRaster.h"

FOdysseyAnimationCellImageRasterMediaAbility::FOdysseyAnimationCellImageRasterMediaAbility(TSharedPtr<FOdysseyAnimationCellImageRaster> iCellImageRaster)
    : mCellImageRaster(iCellImageRaster)
{
}

FOdysseyMediaProvider
FOdysseyAnimationCellImageRasterMediaAbility::GetMediaProvider(uint32 iFrameIndex) const
{
    TSharedPtr<FOdysseyRasterBlock> rasterBlock;
    TSharedPtr<FOdysseyAnimationCellImageRaster> cell = mCellImageRaster.Pin();
    if (cell)
        rasterBlock = cell->GetRasterBlock();

    TSharedPtr<FOdysseyMediaRaster> mediaRaster = MakeShared<FOdysseyMediaRaster>(rasterBlock);
    FOdysseyMediaProvider mediaProvider;
    mediaProvider.Add(mediaRaster);
    return mediaProvider;
}