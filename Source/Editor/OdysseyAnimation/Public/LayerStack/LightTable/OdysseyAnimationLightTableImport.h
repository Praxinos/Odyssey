#pragma once

#include <Core/Core.h>

class FOdysseyAnimationLightTable;

class FOdysseyAnimationLightTableImport
{
public:
    /**
     * @brief Imports Data from the given archive into the given object
     * 
     * @param iAnimationCell 
     * @param Ar 
     */
    static bool ODYSSEYANIMATION_API Read( FOdysseyAnimationLightTable* iAnimationLightTable
                                  , FArchive &Ar );

private:
    /**
     * Private import functions
     */

    static void Read( FOdysseyAnimationLightTable* iAnimationLightTable
                                  , FArchive &Ar
                                  , uint64 iChunkEnd );
};
