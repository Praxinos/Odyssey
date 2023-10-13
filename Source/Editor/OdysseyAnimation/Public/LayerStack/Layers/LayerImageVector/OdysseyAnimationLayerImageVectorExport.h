#pragma once

#include "CoreMinimal.h"

class FOdysseyAnimationLayerImageVector;

class FOdysseyAnimationLayerImageVectorExport
{
public:
    static void ODYSSEYANIMATION_API Write( UOdysseyAnimationLayerImageVector* iAnimationLayerImageVector, FArchive &Ar );

private:
    ////////////////////////////////////
    static void WriteCellsContainer( UOdysseyAnimationLayerImageVector* iAnimationLayerImageVector, FArchive &Ar );
};
