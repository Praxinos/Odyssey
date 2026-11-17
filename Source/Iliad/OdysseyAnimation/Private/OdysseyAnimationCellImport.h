// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"

#if WITH_EDITOR

class FArchive;
class UOdysseyAnimationCell;

class FOdysseyAnimationCellImport
{
public:
    /**
     * @brief Imports Data from the given archive into the given object
     *
     * @param iCell
     * @param Ar
     */
    static bool ODYSSEYANIMATION_API Read( UOdysseyAnimationCell* iCell
                                  , FArchive &Ar );

private:
    /**
     * Private import functions
     */

    static void Read( UOdysseyAnimationCell* iCell
                                  , FArchive &Ar
                                  , uint64 iChunkEnd );
};

#endif
