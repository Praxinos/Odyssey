#pragma once

#include <Core/Core.h>

class UOdysseyTextureLayerImageVector;

class FOdysseyTextureLayerImageVectorImport
{
public:
    /**
     * @brief Imports Data from the given archive into the given object
     *
     * @param iTextureLayerImageVector
     * @param Ar
     */
    static bool ODYSSEYTEXTURE_API Read( UOdysseyTextureLayerImageVector* iTextureLayerImageVector
                                  , FArchive &Ar );

private:
    /**
     * Private import functions
     */

    static void Read( UOdysseyTextureLayerImageVector* iTextureLayerImageVector
                                  , FArchive &Ar
                                  , uint64 iChunkEnd );
};
