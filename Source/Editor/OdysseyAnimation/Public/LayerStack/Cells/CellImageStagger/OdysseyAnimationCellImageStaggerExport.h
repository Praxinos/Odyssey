#pragma once

#include "CoreMinimal.h"

class FOdysseyAnimationCellImageStagger;

class FOdysseyAnimationCellImageStaggerExport
{
public:
    static void ODYSSEYANIMATION_API Write( FOdysseyAnimationCellImageStagger* iAnimationCellImageStagger, FArchive &Ar );

private:
    ////////////////////////////////////
    static void WriteBehaviour( FOdysseyAnimationCellImageStagger* iAnimationCellImageStagger, FArchive &Ar );
    static void WriteReach( FOdysseyAnimationCellImageStagger* iAnimationCellImageStagger, FArchive &Ar );
};
