#pragma once

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
