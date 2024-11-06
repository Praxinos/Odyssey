#pragma once

#include <Core/Core.h>

class UOdysseyAnimationCell;

class FOdysseyAnimationCellImport
{
public:
    /**
     * @brief Imports Data from the given archive into the given object
     *
     * @param iAnimationCell
     * @param Ar
     */
    static bool ODYSSEYANIMATION_API Read( UOdysseyAnimationCell* iAnimationCell
                                  , FArchive &Ar );

private:
    /**
     * Private import functions
     */

    static void Read( UOdysseyAnimationCell* iAnimationCell
                                  , FArchive &Ar
                                  , uint64 iChunkEnd );
};
