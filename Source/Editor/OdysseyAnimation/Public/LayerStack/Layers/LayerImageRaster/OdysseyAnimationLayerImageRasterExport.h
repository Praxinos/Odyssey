#pragma once

#include "CoreMinimal.h"

class UOdysseyAnimationLayerImageRaster;

class FOdysseyAnimationLayerImageRasterExport
{
public:
    static void ODYSSEYANIMATION_API Write( UOdysseyAnimationLayerImageRaster* iAnimationLayerImageRaster, FArchive &Ar );

private:
    ////////////////////////////////////
    static void WriteCellsContainer( UOdysseyAnimationLayerImageRaster* iAnimationLayerImageRaster, FArchive &Ar );
    static void WriteLightTable( UOdysseyAnimationLayerImageRaster* iAnimationLayerImageRaster, FArchive &Ar );
};
