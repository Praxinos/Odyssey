#pragma once

#include <Core/Core.h>

class FOdysseyAnimationLightTable;
struct FOdysseyAnimationLightTable::FKey;

class FOdysseyAnimationLightTableKeyImport
{
public:
    /**
     * @brief Imports Data from the given archive into the given object
     * 
     * @param iAnimationCell 
     * @param Ar 
     */
    static bool ODYSSEYANIMATION_API Read( FOdysseyAnimationLightTable::FKey* iKey
                                  , FArchive &Ar );

private:
    /**
     * Private import functions
     */

    static void Read( FOdysseyAnimationLightTable::FKey* iKey
                                  , FArchive &Ar
                                  , uint64 iChunkEnd );
};
