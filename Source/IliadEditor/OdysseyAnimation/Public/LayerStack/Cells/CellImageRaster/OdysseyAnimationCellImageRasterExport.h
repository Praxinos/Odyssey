// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

class UOdysseyAnimationCellImageRaster;

class FOdysseyAnimationCellImageRasterExport
{
public:
    static void ODYSSEYANIMATION_API Write( UOdysseyAnimationCellImageRaster* iAnimationCellImageRaster, FArchive &Ar );

private:
    ////////////////////////////////////
    static void WriteRasterBlock( UOdysseyAnimationCellImageRaster* iAnimationCellImageRaster, FArchive &Ar );
};
