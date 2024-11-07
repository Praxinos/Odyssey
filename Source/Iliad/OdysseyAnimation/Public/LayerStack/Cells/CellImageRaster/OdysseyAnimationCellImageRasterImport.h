#pragma once

#include <Core/Core.h>

class UOdysseyAnimationCellImageRaster;

class FOdysseyAnimationCellImageRasterImport
{
public:
    /**
     * @brief Imports Data from the given archive into the given object
     *
     * @param iAnimationCellImageRaster
     * @param Ar
     */
    static bool ODYSSEYANIMATION_API Read( UOdysseyAnimationCellImageRaster* iAnimationCellImageRaster
                                  , FArchive &Ar );

private:
    /**
     * Private import functions
     */

    static void Read( UOdysseyAnimationCellImageRaster* iAnimationCellImageRaster
                                  , FArchive &Ar
                                  , uint64 iChunkEnd );
};
