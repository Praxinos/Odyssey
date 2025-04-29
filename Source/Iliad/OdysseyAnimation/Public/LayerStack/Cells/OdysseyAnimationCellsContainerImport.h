// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#if WITH_EDITOR
#include <Core/Core.h>

class UOdysseyAnimationLayer;

class FOdysseyAnimationCellsContainerImport
{
public:
    /**
     * @brief Imports Data from the given archive into the given object
     *
     * @param iAnimationCell
     * @param Ar
     */
    static bool ODYSSEYANIMATION_API Read( UOdysseyAnimationLayer* iAnimationLayer
                                  , FArchive &Ar );

private:
    /**
     * Private import functions
     */

    static void Read( UOdysseyAnimationLayer* iAnimationLayer
                                  , FArchive &Ar
                                  , uint64 iChunkEnd );
};
#endif
