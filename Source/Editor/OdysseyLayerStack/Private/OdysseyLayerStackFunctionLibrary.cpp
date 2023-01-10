// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyLayerStackFunctionLibrary.h"
#include "OdysseyLayerStack.h"

TArray<UOdysseyLayer*>
UOdysseyLayerStackFunctionLibrary::FilterTopmostLayers(TArray<UOdysseyLayer*> iLayers)
{
	TArray< UOdysseyLayer* > topmostLayers;
	for( UOdysseyLayer* layer : iLayers)
	{
		bool hasParentInDraggedLayers = false;
		UOdysseyLayerStack* layerStack = layer->GetLayerStack();
		if (!layerStack)
			continue;

		TArray<UOdysseyLayer*> parents = layer->GetParents();
		for ( UOdysseyLayer* parent : parents )
		{
			if ( iLayers.Contains(parent) )
			{
				hasParentInDraggedLayers = true;
				break;
			}
		}

		if (hasParentInDraggedLayers)
			continue;

		topmostLayers.Add(layer);
	}
	return topmostLayers;
}

TArray<UOdysseyLayer*>
UOdysseyLayerStackFunctionLibrary::SortLayers(TArray<UOdysseyLayer*> iLayers, bool iReverse)
{
	if (iLayers.Num() <= 0)
		return TArray<UOdysseyLayer*>();

	if (!iLayers[0])
		return TArray<UOdysseyLayer*>();

	UOdysseyLayerStack* layerStack = iLayers[0]->GetLayerStack();
	if (!layerStack)
		return TArray<UOdysseyLayer*>();

	for (UOdysseyLayer* layer : iLayers)
	{
		if (!layer)
			return TArray<UOdysseyLayer*>();

		if (layer->GetLayerStack() != layerStack)
			return TArray<UOdysseyLayer*>();
	}

	iLayers.Sort(
        [layerStack, iReverse](UOdysseyLayer& iLayerA, UOdysseyLayer& iLayerB)
        {
            //true => iLayerA is before iLayerB
            int depthA = iLayerA.GetParents().Num();
            int depthB = iLayerB.GetParents().Num();

            if (depthA < depthB)
                return !iReverse;

            if (depthA == depthB)
            {
                int indexA = iLayerA.GetIndexInParent();
                int indexB = iLayerB.GetIndexInParent();

                return iReverse ? indexA > indexB : indexA < indexB;
            }

            return iReverse;
        }
    );

	return iLayers;
}


    