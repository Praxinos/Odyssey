// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyLayerCellSelection.h"

//TODO: Should belong to an editor module
#if WITH_EDITOR

#include "OdysseyLayerStack.h"
#include "OdysseyLayer.h"
#include "OdysseyLayerCell.h"

FOdysseyLayerCellSelection::~FOdysseyLayerCellSelection()
{
    if (mLayerStack)
    {
        UnbindCurrentLayerChanged();
        UnbindOnCellsChanged();
    }
}

FOdysseyLayerCellSelection::FOdysseyLayerCellSelection()
    : mLayerStack(nullptr)
{

}

FOdysseyLayerCellSelection::FOdysseyLayerCellSelection(UOdysseyLayerStack* iLayerStack)
    : mLayerStack(iLayerStack)
{
    if (mLayerStack)
    {
        BindCurrentLayerChanged();
        BindOnCellsChanged();
    }
}

void
FOdysseyLayerCellSelection::OnCurrentLayerChanged(UOdysseyLayerStack* iLayerStack)
{
    if (!mLayerStack)
        return;

    if (iLayerStack != mLayerStack)
        return;

    UnbindOnCellsChanged();
    BindOnCellsChanged();
}

void
FOdysseyLayerCellSelection::OnCellsChanged()
{
    CleanSelectedCells();
}

void
FOdysseyLayerCellSelection::CleanSelectedCells()
{
    UOdysseyLayer* currentLayer = Cast<UOdysseyLayer>(mLayerStack->GetCurrentLayer());
    if (!currentLayer)
    {
        mSelectedCells.Empty();
        return;
    }

    TArray<UOdysseyLayerCell*> cells = currentLayer->GetCells();
    mSelectedCells = mSelectedCells.FilterByPredicate(
        [cells](UOdysseyLayerCell* iCell)
        {
            if (!iCell)
                return false;

            return cells.Contains(iCell);
        }
    );
}

void
FOdysseyLayerCellSelection::CleanCellSelectionCursor()
{
    UOdysseyLayer* currentLayer = Cast<UOdysseyLayer>(mLayerStack->GetCurrentLayer());
    if (!currentLayer)
    {
        mCellSelectionCursor = nullptr;
        return;
    }

    TArray<UOdysseyLayerCell*> cells = GetSelectedCells();
    if (cells.IsEmpty())
    {
        mCellSelectionCursor = nullptr;
        return;
    }

    if (!cells.Contains(mCellSelectionCursor))
        mCellSelectionCursor = cells[0];
}

void
FOdysseyLayerCellSelection::BindCurrentLayerChanged()
{
    UOdysseyLayerStack::OnCurrentLayerChanged().AddRaw(this, &FOdysseyLayerCellSelection::OnCurrentLayerChanged);
}

void
FOdysseyLayerCellSelection::UnbindCurrentLayerChanged()
{
    UOdysseyLayerStack::OnCurrentLayerChanged().RemoveAll(this);
}

void
FOdysseyLayerCellSelection::BindOnCellsChanged()
{
    if (!mLayerStack)
        return;

    UOdysseyLayer* currentLayer = Cast<UOdysseyLayer>(mLayerStack->GetCurrentLayer());
    if (!currentLayer)
        return;

    mLayer = currentLayer;
    currentLayer->OnCellsChanged().AddRaw(this, &FOdysseyLayerCellSelection::OnCellsChanged);
}

void
FOdysseyLayerCellSelection::UnbindOnCellsChanged()
{
    if (!mLayer)
        return;

    mLayer->OnCellsChanged().RemoveAll(this);
    mLayer = nullptr;
}

void
FOdysseyLayerCellSelection::SelectCell(UOdysseyLayerCell* iCell, bool iSetAsCursor)
{
    UOdysseyLayer* currentLayer = Cast<UOdysseyLayer>(mLayerStack->GetCurrentLayer());
    if (!currentLayer)
    {
        mCellSelectionCursor = nullptr;
        mSelectedCells.Empty();
        return;
    }

    if (!iCell)
        return;

    if (!currentLayer->GetCells().Contains(iCell))
        return;

    if (iSetAsCursor)
        mCellSelectionCursor = iCell;

    if (mSelectedCells.Contains(iCell))
        return;

    mSelectedCells.Add(iCell);
    CleanSelectedCells();
}

void
FOdysseyLayerCellSelection::SetSelectedCells(const TArray<UOdysseyLayerCell*>& iSelectedCells)
{
    UOdysseyLayer* currentLayer = Cast<UOdysseyLayer>(mLayerStack->GetCurrentLayer());
    if (!currentLayer)
    {
        mSelectedCells.Empty();
        return;
    }

    mSelectedCells = iSelectedCells;

    TArray<UOdysseyLayerCell*> cells = currentLayer->GetCells();
    mSelectedCells.Sort(
        [cells](const UOdysseyLayerCell& iCellA, const UOdysseyLayerCell& iCellB)
        {
            return iCellA.GetIndexInLayer() < iCellB.GetIndexInLayer();
        }
    );

    CleanSelectedCells();
}

const TArray<UOdysseyLayerCell*>&
FOdysseyLayerCellSelection::GetSelectedCells() const
{
    return mSelectedCells;
}

UOdysseyLayerCell*
FOdysseyLayerCellSelection::GetCellSelectionCursor() const
{
    FOdysseyLayerCellSelection* self = const_cast<FOdysseyLayerCellSelection*>(this);
    self->CleanCellSelectionCursor();
    return mCellSelectionCursor;
}

#endif
