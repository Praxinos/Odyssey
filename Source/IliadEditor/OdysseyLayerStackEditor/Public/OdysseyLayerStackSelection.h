// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#ifdef WITH_EDITOR

#include "CoreMinimal.h"
#include "Selection.h"

/**
 * The layerstack selection stores selected cells and selected layers.
 * Cell selection and Layer selection are mutually exclusive, meaning you cannot select some cells and select some layers at the same time.
 */

 class UOdysseyLayer;
 class UOdysseyLayerCell;
 class UOdysseyLayerStack;

namespace OdysseyLayerStackSelection {

    /**
     * Initialize the LayerStack Selection
     */
    void Initialize();

    /**
     * Returns the Layer Stack Selection Object
     * Use this to modify the selection
     * Make sure to deselect every cells
     */
    ODYSSEYLAYERSTACKEDITOR_API USelection* Get();

    /**
     * Returns the selected cells in the given layer
     * If InOdysseyLayer == nullptr, then it returns the selected cells without checking if they belong to a specific layer
     */
    ODYSSEYLAYERSTACKEDITOR_API TArray<UOdysseyLayerCell*> GetSelectedCells(UOdysseyLayer* InOdysseyLayer);

    /**
     * Returns the selected layers in the given layerstack
     * If InOdysseyLayerStack == nullptr, then it returns the selected cells without checking if they belong to a specific LayerStack
     * If InIncludeCurrentLayer == true, ensures the given layerstack current layer is included in the returned layers, even if it is not selected
     */
    ODYSSEYLAYERSTACKEDITOR_API TArray<UOdysseyLayer*> GetSelectedLayers(UOdysseyLayerStack* InOdysseyLayerStack, bool InIncludeCurrentLayer);

    ODYSSEYLAYERSTACKEDITOR_API void RegisterUndo(const TArray<UOdysseyLayerCell*>& InOldCells, const TArray<UOdysseyLayerCell*>& InNewCells);
    ODYSSEYLAYERSTACKEDITOR_API void RegisterUndo(const TArray<UOdysseyLayer*>& InOldLayers, const TArray<UOdysseyLayer*>& InNewLayers);

    /**
     * Returns the Cell to use as base for Contiguous Cell Selection
    */
    ODYSSEYLAYERSTACKEDITOR_API UOdysseyLayerCell* GetCellSelectionCursor();

    /**
     * Sets the Cell to use as base for Contiguous Cell Selection
    */
    ODYSSEYLAYERSTACKEDITOR_API void SetCellSelectionCursor(UOdysseyLayerCell* InCell);
}

#endif
