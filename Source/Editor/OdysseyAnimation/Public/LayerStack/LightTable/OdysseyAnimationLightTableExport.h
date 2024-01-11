#pragma once

#include "CoreMinimal.h"

class FOdysseyAnimationLightTable;

class FOdysseyAnimationLightTableExport
{
public:
    static void ODYSSEYANIMATION_API Write( FOdysseyAnimationLightTable* iAnimationLightTable, FArchive &Ar );

private:
    ////////////////////////////////////
    static void WriteDisplayPosition( FOdysseyAnimationLightTable* iAnimationLightTable, FArchive &Ar );
    static void WriteColors( FOdysseyAnimationLightTable* iAnimationLightTable, FArchive &Ar );
    static void WriteContrasts( FOdysseyAnimationLightTable* iAnimationLightTable, FArchive &Ar );
    static void WriteKeys( FOdysseyAnimationLightTable* iAnimationLightTable, FArchive &Ar );
};
