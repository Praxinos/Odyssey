// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#if WITH_EDITOR
#include "CoreMinimal.h"

class UOdysseyAnimationLayerImageRaster;

class FOdysseyAnimationLayerImageRasterImport
{
public:
    /**
     * @brief Imports Data from the given archive into the given object
     *
     * @param iAnimationLayerImageRaster
     * @param Ar
     */
    ODYSSEYANIMATION_API static bool Read( UOdysseyAnimationLayerImageRaster* iAnimationLayerImageRaster
                                  , FArchive &Ar );

private:
    /**
     * Private import functions
     */

    static void Read( UOdysseyAnimationLayerImageRaster* iAnimationLayerImageRaster
                                  , FArchive &Ar
                                  , uint64 iChunkEnd );
};
#endif
