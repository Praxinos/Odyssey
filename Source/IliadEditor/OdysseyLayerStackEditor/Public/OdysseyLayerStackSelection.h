// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#ifdef WITH_EDITOR

#include "CoreMinimal.h"
#include "Selection.h"

/** Shares layer and editor object selection, with selection changes supported by undo/redo. */

 class UOdysseyLayer;
 class UOdysseyLayerStack;

namespace OdysseyLayerStackSelection {

    /**
     * Initialize the LayerStack Selection
     */
    void Initialize();

    /**
     * Returns the Layer Stack Selection Object
     * Use this to modify the selection
     */
    ODYSSEYLAYERSTACKEDITOR_API USelection* Get();


    /**
     * Returns the selected layers in the given layerstack
     * If InOdysseyLayerStack == nullptr, then it returns the selected layers without checking their layerstack
     * If InIncludeCurrentLayer == true, ensures the given layerstack current layer is included in the returned layers, even if it is not selected
     */
    ODYSSEYLAYERSTACKEDITOR_API TArray<UOdysseyLayer*> GetSelectedLayers(UOdysseyLayerStack* InOdysseyLayerStack, bool InIncludeCurrentLayer);

    ODYSSEYLAYERSTACKEDITOR_API void RegisterUndo(const TArray<UOdysseyLayer*>& InOldLayers, const TArray<UOdysseyLayer*>& InNewLayers);

    /** Records undo/redo for selected editor objects in the shared selection. */
    ODYSSEYLAYERSTACKEDITOR_API void RegisterObjectsUndo(const TArray<UObject*>& InOldObjects, const TArray<UObject*>& InNewObjects);
}

#endif
