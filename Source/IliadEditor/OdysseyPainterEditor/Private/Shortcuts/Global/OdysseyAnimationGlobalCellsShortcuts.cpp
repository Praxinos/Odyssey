// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Shortcuts/Global/OdysseyAnimationGlobalCellsShortcuts.h"
#include "OdysseyPainterEditorAnimationCommands.h"
#include "OdysseyAnimation.h"
#include "OdysseyAnimationLayerStack.h"
#include "OdysseyAnimationLayer.h"
#include "OdysseyAnimationPlayer.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "OdysseyLayerCellSelection.h"
#include "ScopedTransaction.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyAnimationCurrentFrameMutator.h"


#define LOCTEXT_NAMESPACE "AnimationEditor"



FOdysseyAnimationGlobalCellsShortcuts::FOdysseyAnimationGlobalCellsShortcuts(
    const TAttribute<UOdysseyAnimation*>& iAnimation,
    const TAttribute<int>& iCurrentFrame,
    const FOnTransactCurrentFrame& iOnTransactCurrentFrame
)
    : mAnimation(iAnimation)
    , mCurrentFrame(iCurrentFrame)
    , mOnTransactCurrentFrame(iOnTransactCurrentFrame)
{
}

void
FOdysseyAnimationGlobalCellsShortcuts::MapActionsToCommandList(TSharedRef<FUICommandList> iCommandList)
{
    iCommandList->MapAction(
        FOdysseyPainterEditorAnimationCommands::Get().BreakCell,
        FExecuteAction::CreateRaw(this, &FOdysseyAnimationGlobalCellsShortcuts::Action_BreakCell),
        FCanExecuteAction::CreateRaw(this, &FOdysseyAnimationGlobalCellsShortcuts::CanAction_BreakCell)
    );

    iCommandList->MapAction(
        FOdysseyPainterEditorAnimationCommands::Get().BreakAndClearCell,
        FExecuteAction::CreateRaw(this, &FOdysseyAnimationGlobalCellsShortcuts::Action_BreakAndClearCell),
        FCanExecuteAction::CreateRaw(this, &FOdysseyAnimationGlobalCellsShortcuts::CanAction_BreakAndClearCell)
    );

    iCommandList->MapAction(
        FOdysseyPainterEditorAnimationCommands::Get().RemoveCellMark,
        FExecuteAction::CreateRaw(this, &FOdysseyAnimationGlobalCellsShortcuts::Action_RemoveCellMark),
        FCanExecuteAction::CreateRaw(this, &FOdysseyAnimationGlobalCellsShortcuts::CanAction_RemoveCellMark)
    );

    for (int i = 0; i < FOdysseyPainterEditorAnimationCommands::Get().SetCellMark.Num(); i++)
    {
        iCommandList->MapAction(
            FOdysseyPainterEditorAnimationCommands::Get().SetCellMark[i],
            FExecuteAction::CreateRaw(this, &FOdysseyAnimationGlobalCellsShortcuts::Action_SetCellMark, i),
            FCanExecuteAction::CreateRaw(this, &FOdysseyAnimationGlobalCellsShortcuts::CanAction_SetCellMark, i)
        );
    }

    iCommandList->MapAction(
        FOdysseyPainterEditorAnimationCommands::Get().RemoveCellMarkAtFrame,
        FExecuteAction::CreateRaw(this, &FOdysseyAnimationGlobalCellsShortcuts::Action_RemoveCellMarkAtFrame),
        FCanExecuteAction::CreateRaw(this, &FOdysseyAnimationGlobalCellsShortcuts::CanAction_RemoveCellMarkAtFrame)
    );

    for (int i = 0; i < FOdysseyPainterEditorAnimationCommands::Get().SetCellMarkAtFrame.Num(); i++)
    {
        iCommandList->MapAction(
            FOdysseyPainterEditorAnimationCommands::Get().SetCellMarkAtFrame[i],
            FExecuteAction::CreateRaw( this, &FOdysseyAnimationGlobalCellsShortcuts::Action_SetCellMarkAtFrame, FCellMark{ i } ),
            FCanExecuteAction::CreateRaw( this, &FOdysseyAnimationGlobalCellsShortcuts::CanAction_SetCellMarkAtFrame, FCellMark{ i } )
        );
    }
}

void
FOdysseyAnimationGlobalCellsShortcuts::Action_BreakCell()
{
    UOdysseyAnimation* animation = mAnimation.Get();
    if (!animation)
        return;

    UOdysseyAnimationLayerStack* layerStack = Cast<UOdysseyAnimationLayerStack>(animation->GetLayerStack());
    if (!layerStack)
        return;

    UOdysseyAnimationLayer* currentLayer = Cast<UOdysseyAnimationLayer>(layerStack->GetCurrentLayer());
    if (!currentLayer)
        return;

    if (!currentLayer->IsEditable())
        return;

    UOdysseyLayerCell* cell = currentLayer->GetCellAtFrame(mCurrentFrame.Get());
    if (!cell)
        return;

    int frame = mCurrentFrame.Get() - cell->GetFrameRange().GetLowerBoundValue();
    if (frame == 0)
        return;

#if WITH_EDITOR
        FScopedTransaction ScopedTransaction(LOCTEXT("global-cells-shortcuts.transaction.break-cell", "Break Cell"));
#endif
    UOdysseyLayerCell* newCell = cell->Break(frame, false);
    if (!newCell)
        return;

    //Remove mark from the new cell, because we consider the new cell will be modified by the user and will not represent the original cell anymore
    //This is an arbitrary choice, you are free to change this behaviour whenever you want without any side effect
    newCell->SetMark(INDEX_NONE);

    mOnTransactCurrentFrame.ExecuteIfBound(mCurrentFrame.Get());
}

void
FOdysseyAnimationGlobalCellsShortcuts::Action_BreakAndClearCell()
{
    UOdysseyAnimation* animation = mAnimation.Get();
    if (!animation)
        return;

    UOdysseyAnimationLayerStack* layerStack = Cast<UOdysseyAnimationLayerStack>(animation->GetLayerStack());
    if (!layerStack)
        return;

    UOdysseyAnimationLayer* currentLayer = Cast<UOdysseyAnimationLayer>(layerStack->GetCurrentLayer());
    if (!currentLayer)
        return;

    if (!currentLayer->IsEditable())
        return;

    UOdysseyLayerCell* cell = currentLayer->GetCellAtFrame(mCurrentFrame.Get());
    if (!cell)
        return;

    int frame = mCurrentFrame.Get() - cell->GetFrameRange().GetLowerBoundValue();
    if (frame == 0)
        return;

#if WITH_EDITOR
        FScopedTransaction ScopedTransaction(LOCTEXT("global-cells-shortcuts.transaction.break-cell", "Break Cell"));
#endif
    UOdysseyLayerCell* newCell = cell->Break(frame, true);
    if (!newCell)
        return;

    mOnTransactCurrentFrame.ExecuteIfBound(mCurrentFrame.Get());
}

void
FOdysseyAnimationGlobalCellsShortcuts::Action_RemoveCellMark()
{
    UOdysseyAnimation* animation = mAnimation.Get();
    if (!animation)
        return;

    UOdysseyAnimationLayerStack* layerStack = Cast<UOdysseyAnimationLayerStack>(animation->GetLayerStack());
    if (!layerStack)
        return;

    UOdysseyAnimationLayer* currentLayer = Cast<UOdysseyAnimationLayer>(layerStack->GetCurrentLayer());
    if (!currentLayer)
        return;

    if (!currentLayer->IsEditable())
        return;

    TArray<UOdysseyLayerCell*> selectedCells = layerStack->GetCellSelection()->GetSelectedCells();
    if (selectedCells.IsEmpty())
    {
        UOdysseyLayerCell* cell = currentLayer->GetCellAtFrame(mCurrentFrame.Get());
        if (!cell)
            return;

        selectedCells.Add(cell);
    }

#if WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("global-cells-shortcuts.transaction.remove-cell-mark", "Remove Cell Mark"));
#endif
    for (UOdysseyLayerCell* cell : selectedCells)
    {
        cell->SetMark(INDEX_NONE);
    }

    mOnTransactCurrentFrame.ExecuteIfBound(selectedCells[0]->GetFrameRange().GetLowerBoundValue());
}

void
FOdysseyAnimationGlobalCellsShortcuts::Action_SetCellMark(int iMarkId)
{
    UOdysseyAnimation* animation = mAnimation.Get();
    if (!animation)
        return;

    UOdysseyAnimationLayerStack* layerStack = Cast<UOdysseyAnimationLayerStack>(animation->GetLayerStack());
    if (!layerStack)
        return;

    UOdysseyAnimationLayer* currentLayer = Cast<UOdysseyAnimationLayer>(layerStack->GetCurrentLayer());
    if (!currentLayer)
        return;

    if (!currentLayer->IsEditable())
        return;

    TArray<UOdysseyLayerCell*> selectedCells = layerStack->GetCellSelection()->GetSelectedCells();
    if (selectedCells.IsEmpty())
    {
        UOdysseyLayerCell* cell = currentLayer->GetCellAtFrame(mCurrentFrame.Get());
        if (!cell)
            return;

        selectedCells.Add(cell);
    }

#if WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("global-cells-shortcuts.transaction.remove-cell-mark", "Remove Cell Mark"));
#endif
    for (UOdysseyLayerCell* cell : selectedCells)
    {
        cell->SetMark(iMarkId);
    }

    mOnTransactCurrentFrame.ExecuteIfBound(selectedCells[0]->GetFrameRange().GetLowerBoundValue());
}

void
FOdysseyAnimationGlobalCellsShortcuts::Action_RemoveCellMarkAtFrame()
{
    UOdysseyAnimation* animation = mAnimation.Get();
    if (!animation)
        return;

    UOdysseyAnimationLayerStack* layerStack = Cast<UOdysseyAnimationLayerStack>(animation->GetLayerStack());
    if (!layerStack)
        return;

    UOdysseyAnimationLayer* currentLayer = Cast<UOdysseyAnimationLayer>(layerStack->GetCurrentLayer());
    if (!currentLayer)
        return;

    if (!currentLayer->IsEditable())
        return;

    UOdysseyLayerCell* cell = currentLayer->GetCellAtFrame(mCurrentFrame.Get());
    if (!cell)
        return;

    FScopedTransaction ScopedTransaction(LOCTEXT("global-cells-shortcuts.transaction.remove-cell-mark-at-frame", "Remove Cell Mark at Frame"));

    TMap<int, FCellMark> marks = cell->GetMarks();
    int32 index_in_cell = cell->FrameInLayerToIndexInCell( mCurrentFrame.Get() );
    marks.Remove( index_in_cell );
    cell->SetMarks( marks );

    //mOnTransactCurrentFrame.ExecuteIfBound(cell->GetFrameRange().GetLowerBoundValue());
}

void
FOdysseyAnimationGlobalCellsShortcuts::Action_SetCellMarkAtFrame(FCellMark iMarkId)
{
    UOdysseyAnimation* animation = mAnimation.Get();
    if (!animation)
        return;

    UOdysseyAnimationLayerStack* layerStack = Cast<UOdysseyAnimationLayerStack>(animation->GetLayerStack());
    if (!layerStack)
        return;

    UOdysseyAnimationLayer* currentLayer = Cast<UOdysseyAnimationLayer>(layerStack->GetCurrentLayer());
    if (!currentLayer)
        return;

    if (!currentLayer->IsEditable())
        return;

    UOdysseyLayerCell* cell = currentLayer->GetCellAtFrame( mCurrentFrame.Get() );
    if( !cell )
        return;

    FScopedTransaction ScopedTransaction(LOCTEXT("global-cells-shortcuts.transaction.remove-cell-mark-at-frame", "Remove Cell Mark at Frame"));

    TMap<int, FCellMark> marks = cell->GetMarks();
    int32 index_in_cell = cell->FrameInLayerToIndexInCell( mCurrentFrame.Get() );
    marks.Add( index_in_cell, iMarkId );
    cell->SetMarks( marks );

    //mOnTransactCurrentFrame.ExecuteIfBound(cell->GetFrameRange().GetLowerBoundValue());
}

bool
FOdysseyAnimationGlobalCellsShortcuts::CanAction_BreakCell()
{
    return true;
}

bool
FOdysseyAnimationGlobalCellsShortcuts::CanAction_BreakAndClearCell()
{
    return true;
}

bool
FOdysseyAnimationGlobalCellsShortcuts::CanAction_RemoveCellMark()
{
    UOdysseyAnimation* animation = mAnimation.Get();
    if (!animation)
        return false;

    UOdysseyAnimationLayerStack* layerStack = Cast<UOdysseyAnimationLayerStack>(animation->GetLayerStack());
    if (!layerStack)
        return false;

    UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(layerStack->GetCurrentLayer());
    if (!layer)
        return false;

    if (!layer->IsEditable())
        return false;

    TArray<UOdysseyLayerCell*> selectedCells = layerStack->GetCellSelection()->GetSelectedCells();
    if (selectedCells.IsEmpty())
    {
        UOdysseyLayerCell* cell = layer->GetCellAtFrame(mCurrentFrame.Get());
        if (!cell)
            return false;
    }

    return true;
}

bool
FOdysseyAnimationGlobalCellsShortcuts::CanAction_SetCellMark(int iMarkId)
{
    UOdysseyAnimation* animation = mAnimation.Get();
    if (!animation)
        return false;

    UOdysseyAnimationLayerStack* layerStack = Cast<UOdysseyAnimationLayerStack>(animation->GetLayerStack());
    if (!layerStack)
        return false;

    UOdysseyAnimationLayer* currentLayer = Cast<UOdysseyAnimationLayer>(layerStack->GetCurrentLayer());
    if (!currentLayer )
        return false;

    if (!currentLayer->IsEditable())
        return false;

    UOdysseyLayerCell* cell = currentLayer->GetCellAtFrame( mCurrentFrame.Get() );
    if( !cell )
        return false;

    return true;
}

bool
FOdysseyAnimationGlobalCellsShortcuts::CanAction_RemoveCellMarkAtFrame()
{
    UOdysseyAnimation* animation = mAnimation.Get();
    if (!animation)
        return false;

    UOdysseyAnimationLayerStack* layerStack = Cast<UOdysseyAnimationLayerStack>(animation->GetLayerStack());
    if (!layerStack)
        return false;

    UOdysseyAnimationLayer* currentLayer = Cast<UOdysseyAnimationLayer>(layerStack->GetCurrentLayer());
    if (!currentLayer )
        return false;

    if (!currentLayer->IsEditable())
        return false;

    UOdysseyLayerCell* cell = currentLayer->GetCellAtFrame( mCurrentFrame.Get() );
    if( !cell )
        return false;

    return true;
}

bool
FOdysseyAnimationGlobalCellsShortcuts::CanAction_SetCellMarkAtFrame(FCellMark iMarkId)
{
    UOdysseyAnimation* animation = mAnimation.Get();
    if (!animation)
        return false;

    UOdysseyAnimationLayerStack* layerStack = Cast<UOdysseyAnimationLayerStack>(animation->GetLayerStack());
    if (!layerStack)
        return false;

    UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(layerStack->GetCurrentLayer());
    if (!layer)
        return false;

    if (!layer->IsEditable())
        return false;

    TArray<UOdysseyLayerCell*> selectedCells = layerStack->GetCellSelection()->GetSelectedCells();
    if (selectedCells.IsEmpty())
    {
        UOdysseyLayerCell* cell = layer->GetCellAtFrame(mCurrentFrame.Get());
        if (!cell)
            return false;
    }

    return true;
}

#undef LOCTEXT_NAMESPACE
