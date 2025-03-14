// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

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
