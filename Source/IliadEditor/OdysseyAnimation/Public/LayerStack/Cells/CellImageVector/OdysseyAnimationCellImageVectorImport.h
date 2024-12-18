// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

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
