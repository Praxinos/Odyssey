#pragma once

#include <Core/Core.h>

class FOdysseyAnimationCell;

class FOdysseyAnimationCellImport
{
public:
    /**
     * @brief Imports Data from the given archive into the given object
     * 
     * @param iAnimationCell 
     * @param Ar 
     */
    static bool ODYSSEYANIMATION_API Read( FOdysseyAnimationCell* iAnimationCell
                                  , FArchive &Ar );

private:
    /**
     * Private import functions
     */

    static void ODYSSEYANIMATION_API Read( FOdysseyAnimationCell* iAnimationCell
                                  , FArchive &Ar
                                  , uint64 iChunkEnd );
};
