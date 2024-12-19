// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

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
