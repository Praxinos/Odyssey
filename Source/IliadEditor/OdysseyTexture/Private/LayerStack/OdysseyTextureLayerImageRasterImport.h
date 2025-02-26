// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include <Core/Core.h>

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
