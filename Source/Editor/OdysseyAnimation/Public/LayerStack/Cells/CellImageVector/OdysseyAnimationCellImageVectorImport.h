#pragma once

#include <Core/Core.h>

class FOdysseyAnimationCellImageVector;

class FOdysseyAnimationCellImageVectorImport
{
public:
    /**
     * @brief Imports Data from the given archive into the given object
     * 
     * @param iAnimationCellImageVector 
     * @param Ar 
     */
    static bool ODYSSEYANIMATION_API Read( FOdysseyAnimationCellImageVector* iAnimationCellImageVector
                                  , FArchive &Ar );

private:
    /**
     * Private import functions
     */
    static void Read( FOdysseyAnimationCellImageVector* iAnimationCellImageVector
                                  , FArchive &Ar
                                  , uint64 iChunkEnd );
};
