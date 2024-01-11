#pragma once

#include "CoreMinimal.h"

class FOdysseyAnimationLightTable;
struct FOdysseyAnimationLightTable::FKey;

class FOdysseyAnimationLightTableKeyExport
{
public:
    static void ODYSSEYANIMATION_API Write( FOdysseyAnimationLightTable::FKey* iKey, FArchive &Ar );

private:
    ////////////////////////////////////
    static void WriteIsActivated( FOdysseyAnimationLightTable::FKey* iKey, FArchive &Ar );
    static void WriteOpacity( FOdysseyAnimationLightTable::FKey* iKey, FArchive &Ar );
};
