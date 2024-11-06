#pragma once

#include <Core/Core.h>

struct FOdysseyAnimationLightTableKey;

class FOdysseyAnimationLightTableKeyImport
{
public:
    /**
     * @brief Imports Data from the given archive into the given object
     *
     * @param iAnimationCell
     * @param Ar
     */
    static bool ODYSSEYANIMATION_API Read( FOdysseyAnimationLightTableKey* iKey
                                  , FArchive &Ar );

private:
    /**
     * Private import functions
     */

    static void Read( FOdysseyAnimationLightTableKey* iKey
                                  , FArchive &Ar
                                  , uint64 iChunkEnd );
};
