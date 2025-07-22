// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyLighttable.h"

FOdysseyLighttableKey
UOdysseyLighttableFunctionLibrary::GetPreviousKey(FOdysseyLighttable Lighttable, int Index)
{
    return Lighttable.PreviousKeys[Index];
}

FOdysseyLighttableKey
UOdysseyLighttableFunctionLibrary::GetNextKey(FOdysseyLighttable Lighttable, int Index)
{
    return Lighttable.NextKeys[Index];
}

FOdysseyLighttable
UOdysseyLighttableFunctionLibrary::SetPreviousKey(FOdysseyLighttable Lighttable, FOdysseyLighttableKey Key, int Index)
{
    FOdysseyLighttable lighttable = Lighttable;
    lighttable.PreviousKeys[Index] = Key;
    return lighttable;
}

FOdysseyLighttable
UOdysseyLighttableFunctionLibrary::SetNextKey(FOdysseyLighttable Lighttable, FOdysseyLighttableKey Key, int Index)
{
    FOdysseyLighttable lighttable = Lighttable;
    lighttable.NextKeys[Index] = Key;
    return lighttable;
}
