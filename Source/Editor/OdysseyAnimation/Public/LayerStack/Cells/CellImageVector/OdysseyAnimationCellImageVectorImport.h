#pragma once

#include <Core/Core.h>

class UOdysseyAnimationCellImageVector;

class FOdysseyAnimationCellImageVectorImport
{
public:
    /**
     * @brief Imports Data from the given archive into the given object
     *
     * @param iAnimationCellImageVector
     * @param Ar
     */
    static bool ODYSSEYANIMATION_API Read( UOdysseyAnimationCellImageVector* iAnimationCellImageVector
                                  , FArchive &Ar );

private:
    /**
     * Private import functions
     */
    static void Read( UOdysseyAnimationCellImageVector* iAnimationCellImageVector
                                  , FArchive &Ar
                                  , uint64 iChunkEnd );
};
