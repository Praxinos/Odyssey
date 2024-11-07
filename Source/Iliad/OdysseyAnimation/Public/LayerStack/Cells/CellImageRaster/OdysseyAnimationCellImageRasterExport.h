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
