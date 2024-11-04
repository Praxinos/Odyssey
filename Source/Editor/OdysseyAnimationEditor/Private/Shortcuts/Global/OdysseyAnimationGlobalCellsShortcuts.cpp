// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Shortcuts/Global/OdysseyAnimationGlobalCellsShortcuts.h"
#include "AnimationEditor/OdysseyAnimationEditorCommands.h"
#include "AnimationEditor/OdysseyAnimationEditorExtension.h"
#include "LayerStack/OdysseyAnimationLayerStack.h"
#include "LayerStack/Layers/OdysseyAnimationLayer.h"
#include "LayerStack/Cells/OdysseyAnimationCell.h"
#include "OdysseyAnimation.h"
#include "OdysseyAnimationCurrentFrameMutator.h"
#include "UObject/OdysseyObjectEditorUtils.h"


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

    if (currentLayer->IsLockedRecursively())
        return;

    UOdysseyAnimationCell* cell = currentLayer->GetCellAtFrame(animation->CurrentFrame);
    if (!cell)
        return;

    int frame = animation->CurrentFrame - cell->GetFrameRange().GetLowerBoundValue();
    if (frame == 0)
        return;
        
#ifdef WITH_EDITOR
        FScopedTransaction ScopedTransaction(LOCTEXT("global-cells-shortcuts.transaction.break-cell", "Break Cell"));
#endif
    UOdysseyAnimationCell* newCell = cell->Break(frame);
    if (!newCell)
        return;

    //Remove mark from the new cell, because we consider the new cell will be modified by the user and will not represent the original cell anymore
    //This is an arbitrary choice, you are free to change this behaviour whenever you want without any side effect
    FOdysseyObjectEditorUtils::SetPropertyValue(newCell, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationCell, Mark), INDEX_NONE);

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

    if (currentLayer->IsLockedRecursively())
        return;

    TArray<UOdysseyAnimationCell*> selectedCells = layerStack->GetCellSelection()->GetSelectedCells();
    if (selectedCells.IsEmpty())
    {
        UOdysseyAnimationCell* cell = currentLayer->GetCellAtFrame(animation->CurrentFrame);
        if (!cell)
            return;

        selectedCells.Add(cell);
    }
        
#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("global-cells-shortcuts.transaction.remove-cell-mark", "Remove Cell Mark"));
#endif
    for (UOdysseyAnimationCell* cell : selectedCells)
    {
        FOdysseyObjectEditorUtils::SetPropertyValue(cell, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationCell, Mark), INDEX_NONE);
    }

    FOdysseyAnimationCurrentFrameMutator mutator(animation);
    mutator.Set(selectedCells[0]->GetFrameRange().GetLowerBoundValue());
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

    if (currentLayer->IsLockedRecursively())
        return;
        
    TArray<UOdysseyAnimationCell*> selectedCells = layerStack->GetCellSelection()->GetSelectedCells();
    if (selectedCells.IsEmpty())
    {
        UOdysseyAnimationCell* cell = currentLayer->GetCellAtFrame(animation->CurrentFrame);
        if (!cell)
            return;
            
        selectedCells.Add(cell);
    }

#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("global-cells-shortcuts.transaction.remove-cell-mark", "Remove Cell Mark"));
#endif
    for (UOdysseyAnimationCell* cell : selectedCells)
    {
        FOdysseyObjectEditorUtils::SetPropertyValue(cell, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationCell, Mark), iMarkId);
    }

    FOdysseyAnimationCurrentFrameMutator mutator(animation);
    mutator.Set(selectedCells[0]->GetFrameRange().GetLowerBoundValue());
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

    if (layer->IsLockedRecursively())
        return false;

    TArray<UOdysseyAnimationCell*> selectedCells = layerStack->GetCellSelection()->GetSelectedCells();
    if (selectedCells.IsEmpty())
    {
        UOdysseyAnimationCell* cell = layer->GetCellAtFrame(animation->CurrentFrame);
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

    if (layer->IsLockedRecursively())
        return false;

    TArray<UOdysseyAnimationCell*> selectedCells = layerStack->GetCellSelection()->GetSelectedCells();
    if (selectedCells.IsEmpty())
    {
        UOdysseyAnimationCell* cell = layer->GetCellAtFrame(animation->CurrentFrame);
        if (!cell)
            return false;
    }

    return true;
}

#undef LOCTEXT_NAMESPACE
