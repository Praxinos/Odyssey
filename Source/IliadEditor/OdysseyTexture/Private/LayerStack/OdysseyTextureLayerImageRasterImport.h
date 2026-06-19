// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"

class UOdysseyTextureLayerImageRaster;

class FOdysseyTextureLayerImageRasterImport
{
public:
    /**
     * @brief Imports Data from the given archive into the given object
     *
     * @param iTextureLayerImageRaster
     * @param Ar
     */
    static bool Read( UOdysseyTextureLayerImageRaster* iTextureLayerImageRaster
                                  , FArchive &Ar );

private:
    /**
     * Private import functions
     */

    static void Read( UOdysseyTextureLayerImageRaster* iTextureLayerImageRaster
                                  , FArchive &Ar
                                  , uint64 iChunkEnd );
};
