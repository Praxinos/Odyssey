// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyAnimationCellsSelection.h"

#include "Selection.h"

#include "OdysseyAnimationLayer.h"
#include "OdysseyAnimationCell.h"
#include "OdysseyLayerStackSelection.h"

namespace OdysseyAnimationCellsSelection
{
namespace
{
TObjectPtr<UOdysseyAnimationCell> GCellSelectionCursor;
FDelegateHandle SelectionChangedHandle;
FDelegateHandle CellsChangedHandle;
}

void
OnSelectionChanged(UObject* InSelectionObject)
{
    USelection* Selection = Cast<USelection>(InSelectionObject);
    if (!Selection || Selection != OdysseyLayerStackSelection::Get())
        return;

    if (!GCellSelectionCursor || !GCellSelectionCursor->IsSelected())
    {
        TArray<UOdysseyAnimationCell*> SelectedCells = GetSelectedCells(nullptr);
        if (SelectedCells.IsEmpty())
        {
            GCellSelectionCursor = nullptr;
        }
        else
        {
            GCellSelectionCursor = SelectedCells[0];
        }
    }
}

void
OnLayerCellsChanged(UOdysseyLayer* InLayer, bool IsInteractive)
{
    TArray<UOdysseyAnimationCell*> SelectedCells = GetSelectedCells(nullptr);
    OdysseyLayerStackSelection::Get()->BeginBatchSelectOperation();
    for (UOdysseyAnimationCell* Cell : SelectedCells)
    {
        if (!Cell || Cell->GetIndexInLayer() == INDEX_NONE)
            OdysseyLayerStackSelection::Get()->Deselect(Cell);
    }
    OdysseyLayerStackSelection::Get()->EndBatchSelectOperation();
    /** Garbage cells can disappear from USelection without sending another selection notification. */
    OnSelectionChanged(OdysseyLayerStackSelection::Get());
}

TArray<UOdysseyAnimationCell*>
GetSelectedCells(UOdysseyLayer* InOdysseyLayer)
{
    TArray<UOdysseyAnimationCell*> SelectedCells;
    OdysseyLayerStackSelection::Get()->GetSelectedObjects(SelectedCells);

    if (InOdysseyLayer)
    {
        SelectedCells = SelectedCells.FilterByPredicate(
            [InOdysseyLayer](UOdysseyAnimationCell* InCell)
            {
                return InCell->GetLayer() == InOdysseyLayer;
            }
        );
    }

    return SelectedCells;
}

UOdysseyAnimationCell*
GetCellSelectionCursor()
{
    return GCellSelectionCursor;
}

void
SetCellSelectionCursor(UOdysseyAnimationCell* InCell)
{
    GCellSelectionCursor = InCell;
}

void
Initialize()
{
    if (SelectionChangedHandle.IsValid())
        return;
    check(OdysseyLayerStackSelection::Get());
    SelectionChangedHandle = USelection::SelectionChangedEvent.AddStatic(&OnSelectionChanged);
    CellsChangedHandle = UOdysseyAnimationLayer::OnCellsChanged().AddStatic(&OnLayerCellsChanged);
    GIsCellSelectedInEditor = [](const UObject* InObject)
    {
        return OdysseyLayerStackSelection::Get()->IsSelected(InObject);
    };
}

void
Shutdown()
{
    USelection::SelectionChangedEvent.Remove(SelectionChangedHandle);
    UOdysseyAnimationLayer::OnCellsChanged().Remove(CellsChangedHandle);
    SelectionChangedHandle.Reset();
    CellsChangedHandle.Reset();
    GCellSelectionCursor = nullptr;
    GIsCellSelectedInEditor = nullptr;
}

void
RegisterUndo(const TArray<UOdysseyAnimationCell*>& InOldCells, const TArray<UOdysseyAnimationCell*>& InNewCells)
{
    OdysseyLayerStackSelection::RegisterObjectsUndo(TArray<UObject*>(InOldCells), TArray<UObject*>(InNewCells));
}
}
