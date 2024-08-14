// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/LightTable/OdysseyAnimationLightTable.h"

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
