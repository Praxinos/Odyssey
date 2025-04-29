// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022
#pragma once

#include "CoreMinimal.h"

class UOdysseyLayerStack;
class UOdysseyAnimationLayer;
class UOdysseyAnimationCell;
class UOdysseyAnimationLayerStack;

class ODYSSEYLAYERSTACKEDITOR_API FOdysseyAnimationCellSelection
{
public:
    ~FOdysseyAnimationCellSelection();
    FOdysseyAnimationCellSelection();
    FOdysseyAnimationCellSelection(UOdysseyAnimationLayerStack* iLayerStack);

public:
    void SelectCell(UOdysseyAnimationCell* iCell, bool iSetAsCursor = false);
    void SetSelectedCells(const TArray<UOdysseyAnimationCell*>& iCells);
    const TArray<UOdysseyAnimationCell*>& GetSelectedCells() const;
    UOdysseyAnimationCell* GetCellSelectionCursor() const;
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
    UOdysseyAnimationLayerStack* mLayerStack;
    TArray<UOdysseyAnimationCell*> mSelectedCells;
    UOdysseyAnimationCell* mCellSelectionCursor;
    UOdysseyAnimationLayer* mLayer = nullptr;
};
