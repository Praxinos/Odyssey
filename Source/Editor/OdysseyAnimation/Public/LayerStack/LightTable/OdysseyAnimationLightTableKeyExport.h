#pragma once

#include "CoreMinimal.h"

class FOdysseyAnimationLightTable;
struct FOdysseyAnimationLightTableKey;

class FOdysseyAnimationLightTableKeyExport
{
public:
    static void ODYSSEYANIMATION_API Write( FOdysseyAnimationLightTableKey* iKey, FArchive &Ar );

private:
    ////////////////////////////////////
    static void WriteIsActivated( FOdysseyAnimationLightTableKey* iKey, FArchive &Ar );
    static void WriteOpacity( FOdysseyAnimationLightTableKey* iKey, FArchive &Ar );
};
