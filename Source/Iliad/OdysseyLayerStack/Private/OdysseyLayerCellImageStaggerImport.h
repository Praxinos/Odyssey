// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "Serialization/Archive.h"

class UOdysseyLayerCellImageStagger;

class FOdysseyLayerCellImageStaggerImport
{
public:
    /**
     * @brief Imports Data from the given archive into the given object
     *
     * @param iLayerCellImageStagger
     * @param Ar
     */
    static bool ODYSSEYLAYERSTACK_API Read( UOdysseyLayerCellImageStagger* iLayerCellImageStagger, FArchive& Ar );

private:
    /**
     * Private import functions
     */

    static void Read( UOdysseyLayerCellImageStagger* iLayerCellImageStagger
                                  , FArchive &Ar
                                  , uint64 iChunkEnd );
};
