#pragma once

#include <Core/Core.h>

class UOdysseyAnimationLayerImageVector;

class FOdysseyAnimationLayerImageVectorImport
{
public:
    /**
     * @brief Imports Data from the given archive into the given object
     * 
     * @param iAnimationLayerImageVector 
     * @param Ar 
     */
    static bool ODYSSEYANIMATION_API Read( UOdysseyAnimationLayerImageVector* iAnimationLayerImageVector
                                  , FArchive &Ar );

private:
    /**
     * Private import functions
     */

    static void Read( UOdysseyAnimationLayerImageVector* iAnimationLayerImageVector
                                  , FArchive &Ar
                                  , uint64 iChunkEnd );
};
