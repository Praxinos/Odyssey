#pragma once

#include "CoreMinimal.h"

class FOdysseyAnimationCellImageRaster;

class FOdysseyAnimationCellImageRasterExport
{
public:
    static void ODYSSEYANIMATION_API Write( FOdysseyAnimationCellImageRaster* iAnimationCellImageRaster, FArchive &Ar );

private:
    ////////////////////////////////////
    static void WriteRasterBlock( FOdysseyAnimationCellImageRaster* iAnimationCellImageRaster, FArchive &Ar );
};
