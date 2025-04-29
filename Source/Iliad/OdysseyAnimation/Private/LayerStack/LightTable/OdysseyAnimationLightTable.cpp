// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyAnimationLightTable.h"

FOdysseyAnimationLightTableKey
UOdysseyAnimationLighttableFunctionLibrary::GetPreviousKey(FOdysseyAnimationLightTable Lighttable, int Index)
{
    return Lighttable.PreviousKeys[Index];
}

FOdysseyAnimationLightTableKey
UOdysseyAnimationLighttableFunctionLibrary::GetNextKey(FOdysseyAnimationLightTable Lighttable, int Index)
{
    return Lighttable.NextKeys[Index];
}

FOdysseyAnimationLightTable
UOdysseyAnimationLighttableFunctionLibrary::SetPreviousKey(FOdysseyAnimationLightTable Lighttable, FOdysseyAnimationLightTableKey Key, int Index)
{
    FOdysseyAnimationLightTable lighttable = Lighttable;
    lighttable.PreviousKeys[Index] = Key;
    return lighttable;
}

FOdysseyAnimationLightTable
UOdysseyAnimationLighttableFunctionLibrary::SetNextKey(FOdysseyAnimationLightTable Lighttable, FOdysseyAnimationLightTableKey Key, int Index)
{
    FOdysseyAnimationLightTable lighttable = Lighttable;
    lighttable.NextKeys[Index] = Key;
    return lighttable;
}
