// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"

class FArchive;
class UOdysseyLayerCell;

class FOdysseyLayerCellImport
{
public:
    /**
     * @brief Imports Data from the given archive into the given object
     *
     * @param iCell
     * @param Ar
     */
    static bool ODYSSEYLAYERSTACK_API Read( UOdysseyLayerCell* iCell
                                  , FArchive &Ar );

private:
    /**
     * Private import functions
     */

    static void Read( UOdysseyLayerCell* iCell
                                  , FArchive &Ar
                                  , uint64 iChunkEnd );
};
