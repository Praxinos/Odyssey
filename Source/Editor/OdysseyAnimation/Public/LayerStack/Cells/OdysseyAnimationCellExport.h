#pragma once

#include "CoreMinimal.h"

class FOdysseyAnimationCell;

class FOdysseyAnimationCellExport
{
public:
    static void ODYSSEYANIMATION_API Write( FOdysseyAnimationCell* iAnimationCell, FArchive &Ar );

private:
    ////////////////////////////////////
    static void WriteLength( FOdysseyAnimationCell* iAnimationCell, FArchive &Ar );
    static void WriteMarkId( FOdysseyAnimationCell* iAnimationCell, FArchive &Ar );
};
