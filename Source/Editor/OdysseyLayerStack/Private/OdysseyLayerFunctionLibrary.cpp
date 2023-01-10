// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyLayerFunctionLibrary.h"
#include "OdysseyLayerStack.h"

bool
UOdysseyLayerFunctionLibrary::IsLayerActivatedInStack(UOdysseyLayer* iLayer)
{
	if ( !iLayer )
		return false;

	if ( !iLayer->IsActivated)
		return false;

	UOdysseyLayerStack* layerStack = iLayer->GetLayerStack();
	if ( !layerStack )
		return false;

	TArray<UOdysseyLayer*> parents = iLayer->GetParents();
	for ( UOdysseyLayer* parent : parents )
	{
		if ( !parent->IsActivated )
			return false;
	}

	return true;
}

bool
UOdysseyLayerFunctionLibrary::IsLayerLockedInStack(UOdysseyLayer* iLayer)
{
	if ( !iLayer )
		return true;

	if ( iLayer->IsLocked )
		return true;

	UOdysseyLayerStack* layerStack = iLayer->GetLayerStack();
	if ( !layerStack )
		return true;

	TArray<UOdysseyLayer*> parents = iLayer->GetParents();
	for ( UOdysseyLayer* parent : parents )
	{
		if ( parent->IsLocked )
			return true;
	}

	return false;
}