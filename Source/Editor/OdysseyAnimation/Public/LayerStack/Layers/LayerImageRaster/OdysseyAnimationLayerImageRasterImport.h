#pragma once

#include <Core/Core.h>

class FOdysseyAnimationLayerImageRaster;

class FOdysseyAnimationLayerImageRasterImport
{
public:
    /**
     * @brief Imports Data from the given archive into the given object
     * 
     * @param iAnimationLayerImageRaster 
     * @param Ar 
     */
    static bool ODYSSEYANIMATION_API Read( UOdysseyAnimationLayerImageRaster* iAnimationLayerImageRaster
                                  , FArchive &Ar );

private:
    /**
     * Private import functions
     */

    static void ODYSSEYANIMATION_API Read( UOdysseyAnimationLayerImageRaster* iAnimationLayerImageRaster
                                  , FArchive &Ar
                                  , uint64 iChunkEnd );
};
