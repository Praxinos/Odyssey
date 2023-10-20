#pragma once

#include <Core/Core.h>

class FOdysseyAnimationCellImageStagger;

class FOdysseyAnimationCellImageStaggerImport
{
public:
    /**
     * @brief Imports Data from the given archive into the given object
     * 
     * @param iAnimationCellImageStagger 
     * @param Ar 
     */
    static bool ODYSSEYANIMATION_API Read( FOdysseyAnimationCellImageStagger* iAnimationCellImageStagger
                                  , FArchive &Ar );

private:
    /**
     * Private import functions
     */

    static void Read( FOdysseyAnimationCellImageStagger* iAnimationCellImageStagger
                                  , FArchive &Ar
                                  , uint64 iChunkEnd );
};
