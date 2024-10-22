// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022
#pragma once

#include "CoreMinimal.h"

class UOdysseyAnimation;
class UOdysseyLayerStack;
class UOdysseyAnimationLayer;
class UOdysseyAnimationCell;

class FOdysseyAnimationEditorTimelineCellSelection
{
public:
    ~FOdysseyAnimationEditorTimelineCellSelection();
	FOdysseyAnimationEditorTimelineCellSelection();
    FOdysseyAnimationEditorTimelineCellSelection(UOdysseyAnimation* iAnimation);

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
	UOdysseyAnimation* mAnimation;
    TArray<UOdysseyAnimationCell*> mSelectedCells;
    UOdysseyAnimationCell* mCellSelectionCursor;
	UOdysseyAnimationLayer* mLayer = nullptr;
};