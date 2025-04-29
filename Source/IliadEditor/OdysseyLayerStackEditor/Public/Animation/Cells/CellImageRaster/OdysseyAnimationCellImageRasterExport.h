// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

class UOdysseyAnimationCellImageRaster;

class FOdysseyAnimationCellImageRasterExport
{
public:
    static void ODYSSEYLAYERSTACKEDITOR_API Write( UOdysseyAnimationCellImageRaster* iAnimationCellImageRaster, FArchive &Ar );

private:
    ////////////////////////////////////
    static void WriteRasterBlock( UOdysseyAnimationCellImageRaster* iAnimationCellImageRaster, FArchive &Ar );
};
