#pragma once

#include "CoreMinimal.h"

class UOdysseyAnimationLayerImageVector;

class FOdysseyAnimationLayerImageVectorExport
{
public:
    static void ODYSSEYANIMATION_API Write( UOdysseyAnimationLayerImageVector* iAnimationLayerImageVector, FArchive &Ar );

private:
    ////////////////////////////////////
    static void WriteCellsContainer( UOdysseyAnimationLayerImageVector* iAnimationLayerImageVector, FArchive &Ar );
    static void WriteLightTable( UOdysseyAnimationLayerImageVector* iAnimationLayerImageVector, FArchive &Ar );
};
