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
    static bool ODYSSEYTEXTURE_API Read( UOdysseyTextureLayerImageRaster* iTextureLayerImageRaster
                                  , FArchive &Ar );

private:
    /**
     * Private import functions
     */

    static void Read( UOdysseyTextureLayerImageRaster* iTextureLayerImageRaster
                                  , FArchive &Ar
                                  , uint64 iChunkEnd );
};
