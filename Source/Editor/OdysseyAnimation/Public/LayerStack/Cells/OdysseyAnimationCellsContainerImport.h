#pragma once

#include <Core/Core.h>

class FOdysseyAnimationCellsContainer;

class FOdysseyAnimationCellsContainerImport
{
public:
    /**
     * @brief Imports Data from the given archive into the given object
     * 
     * @param iAnimationCell 
     * @param Ar 
     */
    static bool ODYSSEYANIMATION_API Read( FOdysseyAnimationCellsContainer* iAnimationCellsContainer
                                  , FArchive &Ar );

private:
    /**
     * Private import functions
     */

    static void Read( FOdysseyAnimationCellsContainer* iAnimationCellsContainer
                                  , FArchive &Ar
                                  , uint64 iChunkEnd );
};
