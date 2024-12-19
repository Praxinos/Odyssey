// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

class UOdysseyAnimationCellImageVector;

class FOdysseyAnimationCellImageVectorExport
{
public:
    static void ODYSSEYANIMATION_API Write( UOdysseyAnimationCellImageVector* iAnimationCellImageVector, FArchive &Ar );

private:
    ////////////////////////////////////
    static void WriteResolution( UOdysseyAnimationCellImageVector* iAnimationCellImageVector, FArchive &Ar );
    static void WriteVectorBlock( UOdysseyAnimationCellImageVector* iAnimationCellImageVector, FArchive &Ar );
    static void WriteVectorBlockID( UOdysseyAnimationCellImageVector* iAnimationCellImageVector, FArchive &Ar );
};
