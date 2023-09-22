#pragma once

#include <Core/Core.h>

class FOdysseyAnimationCellImageRaster;

class FOdysseyAnimationCellImageRasterImport
{
public:
    /**
     * @brief Imports Data from the given archive into the given object
     * 
     * @param iAnimationCellImageRaster 
     * @param Ar 
     */
    static bool ODYSSEYANIMATION_API Read( FOdysseyAnimationCellImageRaster* iAnimationCellImageRaster
                                  , FArchive &Ar );

private:
    /**
     * Private import functions
     */

    static void ODYSSEYANIMATION_API Read( FOdysseyAnimationCellImageRaster* iAnimationCellImageRaster
                                  , FArchive &Ar
                                  , uint64 iChunkEnd );
};
