#pragma once

#include "CoreMinimal.h"

class UOdysseyAnimationCellImageVector;

class FOdysseyAnimationCellImageVectorExport
{
public:
    static void ODYSSEYANIMATION_API Write( UOdysseyAnimationCellImageVector* iAnimationCellImageVector, FArchive &Ar );

private:
    ////////////////////////////////////
    static void WriteVectorBlock( UOdysseyAnimationCellImageVector* iAnimationCellImageVector, FArchive &Ar );
    static void WriteVectorBlockID( UOdysseyAnimationCellImageVector* iAnimationCellImageVector, FArchive &Ar );
};
