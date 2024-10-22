#include "OdysseyAnimationEditorTimelineCellSelection.h"

#include "OdysseyAnimation.h"
#include "OdysseyLayerStack.h"
#include "LayerStack/Layers/OdysseyAnimationLayer.h"

FOdysseyAnimationEditorTimelineCellSelection::~FOdysseyAnimationEditorTimelineCellSelection()
{
	if (mAnimation)
	{
		UnbindCurrentLayerChanged();
		UnbindOnCellsChanged();
	}
}

FOdysseyAnimationEditorTimelineCellSelection::FOdysseyAnimationEditorTimelineCellSelection()
    : mAnimation(nullptr)
{

}

FOdysseyAnimationEditorTimelineCellSelection::FOdysseyAnimationEditorTimelineCellSelection(UOdysseyAnimation* iAnimation)
    : mAnimation(iAnimation)
{
	if (mAnimation)
	{
		BindCurrentLayerChanged();
		BindOnCellsChanged();
	}
}

void 
FOdysseyAnimationEditorTimelineCellSelection::OnCurrentLayerChanged(UOdysseyLayerStack* iLayerStack)
{
    if (!mAnimation)
        return;

    UOdysseyLayerStack* layerStack = mAnimation->GetLayerStack();
    if (!layerStack)
        return;

    if (iLayerStack != layerStack)
        return;

    UnbindOnCellsChanged();
    BindOnCellsChanged();
}

void 
FOdysseyAnimationEditorTimelineCellSelection::OnCellsChanged()
{
    CleanSelectedCells();
}

void
FOdysseyAnimationEditorTimelineCellSelection::CleanSelectedCells()
{
    if (!mLayer)
	{
		mSelectedCells.Empty();
        return;
	}

    TArray<UOdysseyAnimationCell*> cells = mLayer->GetCells();
    mSelectedCells = mSelectedCells.FilterByPredicate(
        [cells](UOdysseyAnimationCell* iCell)
        {
            return cells.Contains(iCell);
        }
    );
}

void
FOdysseyAnimationEditorTimelineCellSelection::CleanCellSelectionCursor()
{
    if (!mLayer)
	{
		mCellSelectionCursor = nullptr;
        return;
	}

    TArray<UOdysseyAnimationCell*> cells = GetSelectedCells();
    if (cells.IsEmpty())
    {
        mCellSelectionCursor = nullptr;
        return;
    }
    
    if (!cells.Contains(mCellSelectionCursor))
        mCellSelectionCursor = cells[0];
}

void 
FOdysseyAnimationEditorTimelineCellSelection::BindCurrentLayerChanged()
{
    UOdysseyLayerStack::OnCurrentLayerChanged().AddRaw(this, &FOdysseyAnimationEditorTimelineCellSelection::OnCurrentLayerChanged);
}

void 
FOdysseyAnimationEditorTimelineCellSelection::UnbindCurrentLayerChanged()
{
    UOdysseyLayerStack::OnCurrentLayerChanged().RemoveAll(this);
}

void 
FOdysseyAnimationEditorTimelineCellSelection::BindOnCellsChanged()
{
    if (!mAnimation)
        return;

    UOdysseyLayerStack* layerStack = mAnimation->GetLayerStack();
    if (!layerStack)
        return;

    UOdysseyAnimationLayer* currentLayer = Cast<UOdysseyAnimationLayer>(layerStack->CurrentLayer.Get());    
    if (!currentLayer)
        return;

	mLayer = currentLayer;
    currentLayer->OnCellsChanged().AddRaw(this, &FOdysseyAnimationEditorTimelineCellSelection::OnCellsChanged);
}

void 
FOdysseyAnimationEditorTimelineCellSelection::UnbindOnCellsChanged()
{
    if (!mLayer)
        return;

    mLayer->OnCellsChanged().RemoveAll(this);
	mLayer = nullptr;
}

void
FOdysseyAnimationEditorTimelineCellSelection::SelectCell(UOdysseyAnimationCell* iCell, bool iSetAsCursor)
{
    if (!mLayer)
    {
        mCellSelectionCursor = nullptr;
        mSelectedCells.Empty();
        return;
    }

    if (!mLayer->GetCells().Contains(iCell))
        return;

    if (iSetAsCursor)
        mCellSelectionCursor = iCell;

    if (mSelectedCells.Contains(iCell))
        return;

    mSelectedCells.Add(iCell);
    CleanSelectedCells();
}

void
FOdysseyAnimationEditorTimelineCellSelection::SetSelectedCells(const TArray<UOdysseyAnimationCell*>& iSelectedCells)
{
    if (!mLayer)
    {
        mSelectedCells.Empty();
        return;
    }

    mSelectedCells = iSelectedCells;
    
    TArray<UOdysseyAnimationCell*> cells = mLayer->GetCells();
    mSelectedCells.Sort(
        [cells](const UOdysseyAnimationCell& iCellA, const UOdysseyAnimationCell& iCellB)
        {
            return iCellA.IndexInLayer < iCellB.IndexInLayer;
        }
    );
}

const TArray<UOdysseyAnimationCell*>&
FOdysseyAnimationEditorTimelineCellSelection::GetSelectedCells() const
{
    return mSelectedCells;
}

UOdysseyAnimationCell*
FOdysseyAnimationEditorTimelineCellSelection::GetCellSelectionCursor() const
{
    FOdysseyAnimationEditorTimelineCellSelection* self = const_cast<FOdysseyAnimationEditorTimelineCellSelection*>(this);
    self->CleanCellSelectionCursor();
    return mCellSelectionCursor;
}

		