#pragma once

#include "CoreMinimal.h"

class FOdysseyAnimationCellImageVector;

namespace FOdysseyAnimationCellImageVectorExport
{
    void ODYSSEYANIMATION_API Write( FOdysseyAnimationCellImageVector* iAnimationCellImageVector, FArchive &Ar );

    ////////////////////////////////////
    void WriteResolution( FOdysseyAnimationCellImageVector* iAnimationCellImageVector, FArchive &Ar );
    void WriteVectorBlock( FOdysseyAnimationCellImageVector* iAnimationCellImageVector, FArchive &Ar );
    void WriteVectorBlockID( FOdysseyAnimationCellImageVector* iAnimationCellImageVector, FArchive &Ar );
}
