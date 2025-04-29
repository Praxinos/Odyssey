// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023#include "OdysseyAnimationCellSelection.h"

#include "OdysseyAnimationCellSelection.h"

#include "OdysseyAnimation.h"
#include "OdysseyLayerStack.h"
#include "OdysseyAnimationLayer.h"
#include "OdysseyAnimationCell.h"

FOdysseyAnimationCellSelection::~FOdysseyAnimationCellSelection()
{
    if (mLayerStack)
    {
        UnbindCurrentLayerChanged();
        UnbindOnCellsChanged();
    }
}

FOdysseyAnimationCellSelection::FOdysseyAnimationCellSelection()
    : mLayerStack(nullptr)
{

}

FOdysseyAnimationCellSelection::FOdysseyAnimationCellSelection(UOdysseyAnimationLayerStack* iLayerStack)
    : mLayerStack(iLayerStack)
{
    if (mLayerStack)
    {
        BindCurrentLayerChanged();
        BindOnCellsChanged();
    }
}

void
FOdysseyAnimationCellSelection::OnCurrentLayerChanged(UOdysseyLayerStack* iLayerStack)
{
    if (!mLayerStack)
        return;

    if (iLayerStack != mLayerStack)
        return;

    UnbindOnCellsChanged();
    BindOnCellsChanged();
}

void
FOdysseyAnimationCellSelection::OnCellsChanged()
{
    CleanSelectedCells();
}

void
FOdysseyAnimationCellSelection::CleanSelectedCells()
{
    UOdysseyAnimationLayer* currentLayer = Cast<UOdysseyAnimationLayer>(mLayerStack->CurrentLayer.Get());
    if (!currentLayer)
    {
        mSelectedCells.Empty();
        return;
    }

    TArray<UOdysseyAnimationCell*> cells = currentLayer->GetCells();
    mSelectedCells = mSelectedCells.FilterByPredicate(
        [cells](UOdysseyAnimationCell* iCell)
        {
            if (!iCell)
                return false;

            return cells.Contains(iCell);
        }
    );
}

void
FOdysseyAnimationCellSelection::CleanCellSelectionCursor()
{
    UOdysseyAnimationLayer* currentLayer = Cast<UOdysseyAnimationLayer>(mLayerStack->CurrentLayer.Get());
    if (!currentLayer)
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
FOdysseyAnimationCellSelection::BindCurrentLayerChanged()
{
    UOdysseyLayerStack::OnCurrentLayerChanged().AddRaw(this, &FOdysseyAnimationCellSelection::OnCurrentLayerChanged);
}

void
FOdysseyAnimationCellSelection::UnbindCurrentLayerChanged()
{
    UOdysseyLayerStack::OnCurrentLayerChanged().RemoveAll(this);
}

void
FOdysseyAnimationCellSelection::BindOnCellsChanged()
{
    if (!mLayerStack)
        return;

    UOdysseyAnimationLayer* currentLayer = Cast<UOdysseyAnimationLayer>(mLayerStack->CurrentLayer.Get());
    if (!currentLayer)
        return;

    mLayer = currentLayer;
    currentLayer->OnCellsChanged().AddRaw(this, &FOdysseyAnimationCellSelection::OnCellsChanged);
}

void
FOdysseyAnimationCellSelection::UnbindOnCellsChanged()
{
    if (!mLayer)
        return;

    mLayer->OnCellsChanged().RemoveAll(this);
    mLayer = nullptr;
}

void
FOdysseyAnimationCellSelection::SelectCell(UOdysseyAnimationCell* iCell, bool iSetAsCursor)
{
    UOdysseyAnimationLayer* currentLayer = Cast<UOdysseyAnimationLayer>(mLayerStack->CurrentLayer.Get());
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
FOdysseyAnimationCellSelection::SetSelectedCells(const TArray<UOdysseyAnimationCell*>& iSelectedCells)
{
    UOdysseyAnimationLayer* currentLayer = Cast<UOdysseyAnimationLayer>(mLayerStack->CurrentLayer.Get());
    if (!currentLayer)
    {
        mSelectedCells.Empty();
        return;
    }

    mSelectedCells = iSelectedCells;

    TArray<UOdysseyAnimationCell*> cells = currentLayer->GetCells();
    mSelectedCells.Sort(
        [cells](const UOdysseyAnimationCell& iCellA, const UOdysseyAnimationCell& iCellB)
        {
            return iCellA.IndexInLayer < iCellB.IndexInLayer;
        }
    );

    CleanSelectedCells();
}

const TArray<UOdysseyAnimationCell*>&
FOdysseyAnimationCellSelection::GetSelectedCells() const
{
    return mSelectedCells;
}

UOdysseyAnimationCell*
FOdysseyAnimationCellSelection::GetCellSelectionCursor() const
{
    FOdysseyAnimationCellSelection* self = const_cast<FOdysseyAnimationCellSelection*>(this);
    self->CleanCellSelectionCursor();
    return mCellSelectionCursor;
}
