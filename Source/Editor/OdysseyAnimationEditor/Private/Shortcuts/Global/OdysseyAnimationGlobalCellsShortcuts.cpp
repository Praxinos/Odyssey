// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Shortcuts/Global/OdysseyAnimationGlobalCellsShortcuts.h"
#include "AnimationEditor/OdysseyAnimationEditorCommands.h"
#include "AnimationEditor/OdysseyAnimationEditorExtension.h"
#include "LayerStack/OdysseyAnimationLayerStack.h"
#include "LayerStack/Layers/OdysseyAnimationLayer.h"
#include "LayerStack/Cells/OdysseyAnimationCellsMutator.h"
#include "OdysseyAnimation.h"
#include "OdysseyAnimationCurrentFrameMutator.h"
#include "LayerStack/Cells/OdysseyAnimationCellsContainer.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

FOdysseyAnimationGlobalCellsShortcuts::FOdysseyAnimationGlobalCellsShortcuts(TSharedPtr<FOdysseyAnimationEditorExtension> iExtension)
    : mExtension(iExtension)
{
}

void
FOdysseyAnimationGlobalCellsShortcuts::MapActionsToCommandList(TSharedRef<FUICommandList> iCommandList)
{
    iCommandList->MapAction(
        FOdysseyAnimationEditorCommands::Get().BreakCell,
        FExecuteAction::CreateRaw(this, &FOdysseyAnimationGlobalCellsShortcuts::Action_BreakCell),
        FCanExecuteAction::CreateRaw(this, &FOdysseyAnimationGlobalCellsShortcuts::CanAction_BreakCell)
    );

    iCommandList->MapAction(
        FOdysseyAnimationEditorCommands::Get().RemoveCellMark,
        FExecuteAction::CreateRaw(this, &FOdysseyAnimationGlobalCellsShortcuts::Action_RemoveCellMark),
        FCanExecuteAction::CreateRaw(this, &FOdysseyAnimationGlobalCellsShortcuts::CanAction_RemoveCellMark)
    );

    for (int i = 0; i < FOdysseyAnimationEditorCommands::Get().SetCellMark.Num(); i++)
    {
        iCommandList->MapAction(
            FOdysseyAnimationEditorCommands::Get().SetCellMark[i],
            FExecuteAction::CreateRaw(this, &FOdysseyAnimationGlobalCellsShortcuts::Action_SetCellMark, i),
            FCanExecuteAction::CreateRaw(this, &FOdysseyAnimationGlobalCellsShortcuts::CanAction_SetCellMark, i)
        );
    }

    
}

void
FOdysseyAnimationGlobalCellsShortcuts::Action_BreakCell()
{
    TSharedPtr<FOdysseyAnimationEditorExtension> extension = mExtension.Pin();
    if (!extension)
        return;

    UOdysseyAnimation* animation = extension->Animation();
    if (!animation)
        return;

    UOdysseyAnimationLayerStack* layerStack = extension->LayerStack();
    if (!layerStack)
        return;

    UOdysseyAnimationLayer* currentLayer = Cast<UOdysseyAnimationLayer>(layerStack->CurrentLayer.Get());
    if (!currentLayer)
        return;

    if (currentLayer->GetIsLockedRecursively())
        return;

    TSharedPtr<FOdysseyAnimationCellsContainer> cellsContainer = currentLayer->GetCellsContainer();
    if (!cellsContainer)
        return;
        
#ifdef WITH_EDITOR
        FScopedTransaction ScopedTransaction(LOCTEXT("global-cells-shortcuts.transaction.break-cell", "Break Cell"));
#endif
    TSharedRef<FOdysseyAnimationCellsMutator> cellsMutator = MakeShared<FOdysseyAnimationCellsMutator>(currentLayer, cellsContainer.ToSharedRef());
    cellsMutator->BreakCellAtFrame(animation->CurrentFrame);

    FOdysseyAnimationCurrentFrameMutator mutator(animation);
    mutator.Set(animation->CurrentFrame);
    mutator.Commit();
}

void
FOdysseyAnimationGlobalCellsShortcuts::Action_RemoveCellMark()
{
    TSharedPtr<FOdysseyAnimationEditorExtension> extension = mExtension.Pin();
    if (!extension)
        return;

    UOdysseyAnimation* animation = extension->Animation();
    if (!animation)
        return;

    UOdysseyAnimationLayerStack* layerStack = extension->LayerStack();
    if (!layerStack)
        return;

    UOdysseyAnimationLayer* currentLayer = Cast<UOdysseyAnimationLayer>(layerStack->CurrentLayer.Get());
    if (!currentLayer)
        return;

    if (currentLayer->GetIsLockedRecursively())
        return;

    TSharedPtr<FOdysseyAnimationCellsContainer> cellsContainer = currentLayer->GetCellsContainer();
    if (!cellsContainer)
        return;

    TArray<TSharedPtr<FOdysseyAnimationCell>> selectedCells = extension->Timeline()->GetSelectedCells();
    if (selectedCells.IsEmpty())
    {
        TSharedPtr<FOdysseyAnimationCell> cell = cellsContainer->GetCellAtFrame(animation->CurrentFrame);
        if (!cell)
            return;

        selectedCells.Add(cell);
    }
        
#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("global-cells-shortcuts.transaction.remove-cell-mark", "Remove Cell Mark"));
#endif
    FOdysseyAnimationCellsMutator cellsMutator(currentLayer, cellsContainer.ToSharedRef());

    for (TSharedPtr<FOdysseyAnimationCell> cell : selectedCells)
    {
        cellsMutator.SetMarkId(cell, INDEX_NONE);
    }
    cellsMutator.Commit();

    int frame = cellsContainer->GetCellFrame(selectedCells[0]);

    FOdysseyAnimationCurrentFrameMutator mutator(animation);
    mutator.Set(frame);
    mutator.Commit();
}

void
FOdysseyAnimationGlobalCellsShortcuts::Action_SetCellMark(int iMarkId)
{
    TSharedPtr<FOdysseyAnimationEditorExtension> extension = mExtension.Pin();
    if (!extension)
        return;

    UOdysseyAnimation* animation = extension->Animation();
    if (!animation)
        return;

    UOdysseyAnimationLayerStack* layerStack = extension->LayerStack();
    if (!layerStack)
        return;

    UOdysseyAnimationLayer* currentLayer = Cast<UOdysseyAnimationLayer>(layerStack->CurrentLayer.Get());
    if (!currentLayer)
        return;

    if (currentLayer->GetIsLockedRecursively())
        return;

    TSharedPtr<FOdysseyAnimationCellsContainer> cellsContainer = currentLayer->GetCellsContainer();
    if (!cellsContainer)
        return;
        
    TArray<TSharedPtr<FOdysseyAnimationCell>> selectedCells = extension->Timeline()->GetSelectedCells();
    if (selectedCells.IsEmpty())
    {
        TSharedPtr<FOdysseyAnimationCell> cell = cellsContainer->GetCellAtFrame(animation->CurrentFrame);
        if (!cell)
            return;
            
        selectedCells.Add(cell);
    }

#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("global-cells-shortcuts.transaction.remove-cell-mark", "Remove Cell Mark"));
#endif
    FOdysseyAnimationCellsMutator cellsMutator(currentLayer, cellsContainer.ToSharedRef());

    for (TSharedPtr<FOdysseyAnimationCell> cell : selectedCells)
    {
        cellsMutator.SetMarkId(cell, iMarkId);
    }
    cellsMutator.Commit();

    int frame = cellsContainer->GetCellFrame(selectedCells[0]);

    FOdysseyAnimationCurrentFrameMutator mutator(animation);
    mutator.Set(frame);
    mutator.Commit();
}

bool
FOdysseyAnimationGlobalCellsShortcuts::CanAction_BreakCell()
{
    return true;
}

bool
FOdysseyAnimationGlobalCellsShortcuts::CanAction_RemoveCellMark()
{
    TSharedPtr<FOdysseyAnimationEditorExtension> extension = mExtension.Pin();
    if (!extension)
        return false;

    UOdysseyAnimation* animation = extension->Animation();
    if (!animation)
        return false;

    UOdysseyAnimationLayerStack* layerStack = extension->LayerStack();
    if (!layerStack)
        return false;

    UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(layerStack->CurrentLayer.Get());
    if (!layer)
        return false;

    if (layer->GetIsLockedRecursively())
        return false;

    TSharedPtr<FOdysseyAnimationCellsContainer> cellsContainer = layer->GetCellsContainer();
    if (!cellsContainer)
        return false;

    TArray<TSharedPtr<FOdysseyAnimationCell>> selectedCells = extension->Timeline()->GetSelectedCells();
    if (selectedCells.IsEmpty())
    {
        TSharedPtr<FOdysseyAnimationCell> cell = cellsContainer->GetCellAtFrame(animation->CurrentFrame);
        if (!cell)
            return false;
    }

    return true;
}

bool
FOdysseyAnimationGlobalCellsShortcuts::CanAction_SetCellMark(int iMarkId)
{
    TSharedPtr<FOdysseyAnimationEditorExtension> extension = mExtension.Pin();
    if (!extension)
        return false;

    UOdysseyAnimation* animation = extension->Animation();
    if (!animation)
        return false;

    UOdysseyAnimationLayerStack* layerStack = extension->LayerStack();
    if (!layerStack)
        return false;
        
    UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(layerStack->CurrentLayer.Get());
    if (!layer)
        return false;

    if (layer->GetIsLockedRecursively())
        return false;

    TSharedPtr<FOdysseyAnimationCellsContainer> cellsContainer = layer->GetCellsContainer();
    if (!cellsContainer)
        return false;

    TArray<TSharedPtr<FOdysseyAnimationCell>> selectedCells = extension->Timeline()->GetSelectedCells();
    if (selectedCells.IsEmpty())
    {
        TSharedPtr<FOdysseyAnimationCell> cell = cellsContainer->GetCellAtFrame(animation->CurrentFrame);
        if (!cell)
            return false;
    }

    return true;
}

#undef LOCTEXT_NAMESPACE