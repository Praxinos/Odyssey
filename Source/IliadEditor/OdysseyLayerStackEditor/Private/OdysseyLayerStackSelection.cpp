// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#ifdef WITH_EDITOR

#include "OdysseyLayerStackSelection.h"

#include "Editor.h"
#include "Misc/Change.h"
#include "Misc/ITransaction.h"
#include "Selection.h"
#include "UObject/GCObject.h"

#include "OdysseyLayer.h"
#include "OdysseyLayerCell.h"
#include "OdysseyLayerStack.h"

TObjectPtr<USelection> GOdysseyLayerStackSelection;
TObjectPtr<UOdysseyLayerCell> GCellSelectionCursor;

namespace OdysseyLayerStackSelection {

template<class T>
class FSelectedObjectsUndo
    : public FCommandChange
    , public FGCObject
{
public:
    ~FSelectedObjectsUndo() {}
    FSelectedObjectsUndo() {}
    FSelectedObjectsUndo(TArray<T*> InOldSelectedObjects, TArray<T*> InNewSelectedObjects)
        : OldSelectedObjects(InOldSelectedObjects)
        , NewSelectedObjects(InNewSelectedObjects)
    {
    }

    /** Makes the change to the object */
    //REDO
    virtual void Apply( UObject* Object ) override
    {
        OdysseyLayerStackSelection::Get()->BeginBatchSelectOperation();
        OdysseyLayerStackSelection::Get()->DeselectAll();
        for (T* SelectedObject : NewSelectedObjects)
        {
            OdysseyLayerStackSelection::Get()->Select(SelectedObject);
        }
        OdysseyLayerStackSelection::Get()->EndBatchSelectOperation();
    }

    /** Reverts change to the object */
    //UNDO
    virtual void Revert( UObject* Object ) override
    {
        OdysseyLayerStackSelection::Get()->BeginBatchSelectOperation();
        OdysseyLayerStackSelection::Get()->DeselectAll();
        for (T* SelectedObject : OldSelectedObjects)
        {
            OdysseyLayerStackSelection::Get()->Select(SelectedObject);
        }
        OdysseyLayerStackSelection::Get()->EndBatchSelectOperation();
    }

    /** Describes this change (for debugging) */
    virtual FString ToString() const override
    {
        return TEXT("OdysseyLayerStackSelection::FSelectedObjectsUndo");
    }


    /** FGCObject interface */
    virtual void AddReferencedObjects( FReferenceCollector& Collector ) override
    {
        Collector.AddReferencedObjects( OldSelectedObjects );
        Collector.AddReferencedObjects( NewSelectedObjects );
    }

    virtual FString GetReferencerName() const override
    {
        return "OdysseyLayerStackSelection::FSelectedObjectsUndo";
    }

private:
    TArray<TObjectPtr<T>> OldSelectedObjects;
    TArray<TObjectPtr<T>> NewSelectedObjects;
};

void
OnLayerCellsChanged(UOdysseyLayer* InLayer, bool IsInteractive)
{
    TArray<UOdysseyLayerCell*> SelectedCells = GetSelectedCells(nullptr);
    OdysseyLayerStackSelection::Get()->BeginBatchSelectOperation();
    for (UOdysseyLayerCell* Cell : SelectedCells)
    {
        if (!Cell || Cell->GetIndexInLayer() == INDEX_NONE)
            OdysseyLayerStackSelection::Get()->Deselect(Cell);
    }
    OdysseyLayerStackSelection::Get()->EndBatchSelectOperation();
}

void
OnCurrentLayerChanged(UOdysseyLayerStack* InLayerStack)
{
    GOdysseyLayerStackSelection->DeselectAll();
}

void
OnSelectionChanged(UObject* InSelectionObject)
{
    USelection* Selection = Cast<USelection>(InSelectionObject);
    if (!Selection || Selection != GOdysseyLayerStackSelection)
        return;

    if (!GCellSelectionCursor || !GCellSelectionCursor->IsSelected())
    {
        TArray<UOdysseyLayerCell*> SelectedCells = GetSelectedCells(nullptr);
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
Initialize()
{
    GOdysseyLayerStackSelection = USelection::CreateObjectSelection(GetTransientPackage(), TEXT("GOdysseyLayerStackSelection")/*, RF_Transactional */);
    GOdysseyLayerStackSelection->AddToRoot();
    GOdysseyLayerStackSelection->SetElementSelectionSet(NewObject<UTypedElementSelectionSet>(GOdysseyLayerStackSelection, NAME_None/*, RF_Transactional*/));
    USelection::SelectionChangedEvent.AddStatic(&OnSelectionChanged);

    UOdysseyLayerStack::OnCurrentLayerChanged().AddStatic(&OnCurrentLayerChanged);
    UOdysseyLayer::OnCellsChanged().AddStatic(&OnLayerCellsChanged);

    GIsLayerSelectedInEditor = [](const UObject* InObject)
    {
        return GOdysseyLayerStackSelection->IsSelected(InObject);
    };

    GIsCellSelectedInEditor = [](const UObject* InObject)
    {
        return GOdysseyLayerStackSelection->IsSelected(InObject);
    };
}

TArray<UOdysseyLayerCell*>
GetSelectedCells(UOdysseyLayer* InOdysseyLayer)
{
    TArray<UOdysseyLayerCell*> SelectedCells;
    GOdysseyLayerStackSelection->GetSelectedObjects(SelectedCells);

    if (InOdysseyLayer)
    {
        SelectedCells = SelectedCells.FilterByPredicate(
            [InOdysseyLayer](UOdysseyLayerCell* InCell)
            {
                return InCell->GetLayer() == InOdysseyLayer;
            }
        );
    }

    return SelectedCells;
}

TArray<UOdysseyLayer*>
GetSelectedLayers(UOdysseyLayerStack* InOdysseyLayerStack, bool InIncludeCurrentLayer)
{
    TArray<UOdysseyLayer*> SelectedLayers;
    GOdysseyLayerStackSelection->GetSelectedObjects(SelectedLayers);

    if (InOdysseyLayerStack)
    {
        SelectedLayers = SelectedLayers.FilterByPredicate(
            [InOdysseyLayerStack](UOdysseyLayer* InLayer)
            {
                return InLayer->GetLayerStack() == InOdysseyLayerStack;
            }
        );

        if (InIncludeCurrentLayer)
        {
            SelectedLayers.AddUnique(InOdysseyLayerStack->GetCurrentLayer());
        }
    }

    return SelectedLayers;
}

USelection*
Get()
{
    return GOdysseyLayerStackSelection;
}

UOdysseyLayerCell*
GetCellSelectionCursor()
{
    return GCellSelectionCursor;
}

void
SetCellSelectionCursor(UOdysseyLayerCell* InCell)
{
    GCellSelectionCursor = InCell;
}

void
RegisterUndo(const TArray<UOdysseyLayerCell*>& InOldCells, const TArray<UOdysseyLayerCell*>& InNewCells)
{
    if( GEditor && GUndo && GEditor->IsTransactionActive())
    {
        TUniquePtr<FSelectedObjectsUndo<UOdysseyLayerCell>> Undo = MakeUnique<FSelectedObjectsUndo<UOdysseyLayerCell>>(InOldCells, InNewCells);
        GUndo->StoreUndo( GOdysseyLayerStackSelection, MoveTemp(Undo) );
    }
}

void
RegisterUndo(const TArray<UOdysseyLayer*>& InOldLayers, const TArray<UOdysseyLayer*>& InNewLayers)
{
    if( GEditor && GUndo && GEditor->IsTransactionActive())
    {
        TUniquePtr<FSelectedObjectsUndo<UOdysseyLayer>> Undo = MakeUnique<FSelectedObjectsUndo<UOdysseyLayer>>(InOldLayers, InNewLayers);
        GUndo->StoreUndo( GOdysseyLayerStackSelection, MoveTemp(Undo) );
    }
}

}

#endif
