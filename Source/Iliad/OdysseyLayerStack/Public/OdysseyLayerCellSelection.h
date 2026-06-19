// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019
#pragma once

#include "CoreMinimal.h"

//TODO: Should belong to an editor module
#if WITH_EDITOR

class UOdysseyLayerStack;
class UOdysseyLayer;
class UOdysseyLayerCell;
class UOdysseyLayerStack;

class ODYSSEYLAYERSTACK_API FOdysseyLayerCellSelection
{
public:
    ~FOdysseyLayerCellSelection();
    FOdysseyLayerCellSelection();
    FOdysseyLayerCellSelection(UOdysseyLayerStack* iLayerStack);

public:
    void SelectCell(UOdysseyLayerCell* iCell, bool iSetAsCursor = false);
    void SetSelectedCells(const TArray<UOdysseyLayerCell*>& iCells);
    const TArray<UOdysseyLayerCell*>& GetSelectedCells() const;
    UOdysseyLayerCell* GetCellSelectionCursor() const;
    void CleanSelectedCells();
    void CleanCellSelectionCursor();

private:
    void OnCurrentLayerChanged(UOdysseyLayerStack* iLayerStack);
    void OnCellsChanged();
    void BindCurrentLayerChanged();
    void UnbindCurrentLayerChanged();
    void BindOnCellsChanged();
    void UnbindOnCellsChanged();

private:
    UOdysseyLayerStack* mLayerStack;
    TArray<UOdysseyLayerCell*> mSelectedCells;
    UOdysseyLayerCell* mCellSelectionCursor;
    UOdysseyLayer* mLayer = nullptr;
};

#endif
