// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"

class UOdysseyLayer;
class UOdysseyAnimationCell;

/** Cell selection uses the layer selection object so selecting cells and layers remains mutually exclusive. */
namespace OdysseyAnimationCellsSelection
{
    /** Installs the animation selection callbacks after the generic layer selection is initialized. */
    void Initialize();
    /** Removes animation callbacks before the module unloads. */
    void Shutdown();
    /** Returns selected cells, optionally filtered by their layer. */
    ODYSSEYANIMATIONEDITOR_API TArray<UOdysseyAnimationCell*> GetSelectedCells(UOdysseyLayer* InOdysseyLayer);
    /** Records undo/redo for cell selection in the shared selection object. */
    ODYSSEYANIMATIONEDITOR_API void RegisterUndo(const TArray<UOdysseyAnimationCell*>& InOldCells, const TArray<UOdysseyAnimationCell*>& InNewCells);
    /** Returns the anchor for contiguous cell selection. */
    ODYSSEYANIMATIONEDITOR_API UOdysseyAnimationCell* GetCellSelectionCursor();
    /** Sets the anchor for contiguous cell selection. */
    ODYSSEYANIMATIONEDITOR_API void SetCellSelectionCursor(UOdysseyAnimationCell* InCell);
}
