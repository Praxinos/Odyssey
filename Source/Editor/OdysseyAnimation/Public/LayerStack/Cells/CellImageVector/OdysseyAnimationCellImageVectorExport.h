#pragma once

#include "CoreMinimal.h"

class FOdysseyAnimationCellImageVector;

class FOdysseyAnimationCellImageVectorExport
{
public:
    static void ODYSSEYANIMATION_API Write( FOdysseyAnimationCellImageVector* iAnimationCellImageVector, FArchive &Ar );

private:
    ////////////////////////////////////
    static void WriteResolution( FOdysseyAnimationCellImageVector* iAnimationCellImageVector, FArchive &Ar );
    static void WriteVectorBlock( FOdysseyAnimationCellImageVector* iAnimationCellImageVector, FArchive &Ar );
    static void WriteVectorBlockID( FOdysseyAnimationCellImageVector* iAnimationCellImageVector, FArchive &Ar );
};
