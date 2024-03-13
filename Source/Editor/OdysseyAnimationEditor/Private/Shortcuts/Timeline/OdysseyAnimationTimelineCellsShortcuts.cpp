// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Shortcuts/Timeline/OdysseyAnimationTimelineCellsShortcuts.h"

#include "Widgets/LayerStack/SOdysseyAnimationLayerStackTreeView.h"
#include "LayerStack/OdysseyAnimationLayerStack.h"
#include "OdysseyAnimationEditorTimeline.h"
#include "LayerStack/Layers/OdysseyAnimationLayer.h"
#include "LayerStack/Cells/OdysseyAnimationCellClipboardData.h"
#include "LayerStack/Cells/CellImageStagger/OdysseyAnimationCellImageStagger.h"
#include "Framework/Commands/GenericCommands.h"
#include "OdysseyEditorModule.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

FOdysseyAnimationTimelineCellsShortcuts::FOdysseyAnimationTimelineCellsShortcuts(UOdysseyLayerStack* iLayerStack, FOdysseyAnimationEditorExtension* iAnimationExtension)
    : mLayerStack(iLayerStack)
    , mAnimationExtension(iAnimationExtension)
{
}

void
FOdysseyAnimationTimelineCellsShortcuts::MapActionsToCommandList(TSharedRef<FUICommandList> iCommandList)
{
    iCommandList->MapAction(
        FGenericCommands::Get().Copy,
        FExecuteAction::CreateRaw(this, &FOdysseyAnimationTimelineCellsShortcuts::Action_Copy),
        FCanExecuteAction::CreateRaw(this, &FOdysseyAnimationTimelineCellsShortcuts::CanAction_Copy)
    );

    iCommandList->MapAction(
        FGenericCommands::Get().Paste,
        FExecuteAction::CreateRaw(this, &FOdysseyAnimationTimelineCellsShortcuts::Action_Paste),
        FCanExecuteAction::CreateRaw(this, &FOdysseyAnimationTimelineCellsShortcuts::CanAction_Paste)
    );

    iCommandList->MapAction(
        FGenericCommands::Get().Cut,
        FExecuteAction::CreateRaw(this, &FOdysseyAnimationTimelineCellsShortcuts::Action_Cut),
        FCanExecuteAction::CreateRaw(this, &FOdysseyAnimationTimelineCellsShortcuts::CanAction_Cut)
    );

    iCommandList->MapAction(
        FGenericCommands::Get().SelectAll,
        FExecuteAction::CreateRaw(this, &FOdysseyAnimationTimelineCellsShortcuts::Action_SelectAll),
        FCanExecuteAction::CreateRaw(this, &FOdysseyAnimationTimelineCellsShortcuts::CanAction_SelectAll)
    );

    iCommandList->MapAction(
        FGenericCommands::Get().Delete,
        FExecuteAction::CreateRaw(this, &FOdysseyAnimationTimelineCellsShortcuts::Action_Delete),
        FCanExecuteAction::CreateRaw(this, &FOdysseyAnimationTimelineCellsShortcuts::CanAction_Delete)
    );

    iCommandList->MapAction(
        FOdysseyAnimationEditorCommands::Get().ConvertToStaggerCell,
        FExecuteAction::CreateRaw(this, &FOdysseyAnimationTimelineCellsShortcuts::Action_ConvertToStaggerCell),
        FCanExecuteAction::CreateRaw(this, &FOdysseyAnimationTimelineCellsShortcuts::CanAction_ConvertToStaggerCell)
    );
}

void
FOdysseyAnimationTimelineCellsShortcuts::Action_Copy()
{
    const TArray<TSharedPtr<FOdysseyAnimationCell>> selectedCells = mAnimationExtension->Timeline()->GetSelectedCells();
    if (selectedCells.IsEmpty())
        return;

    FOdysseyEditorModule& odysseyEditorModule = FModuleManager::Get().LoadModuleChecked<FOdysseyEditorModule>(TEXT("OdysseyEditor"));
    TSharedPtr<FOdysseyAnimationCellClipboardData> clipboardData = MakeShared<FOdysseyAnimationCellClipboardData>(selectedCells);
    odysseyEditorModule.GetClipboard()->SetData(clipboardData);
}

void
FOdysseyAnimationTimelineCellsShortcuts::Action_Cut()
{
    UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(mLayerStack->CurrentLayer.Get());
    if (!layer)
        return;

    if (layer->GetIsLocked())
    {
        Action_Copy();
        return;
    }

    const TArray<TSharedPtr<FOdysseyAnimationCell>> selectedCells = mAnimationExtension->Timeline()->GetSelectedCells();
    if (selectedCells.IsEmpty())
        return;

#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("timeline.shortcuts.cut-frame", "Cut Frames"));
#endif
    
    FOdysseyEditorModule& odysseyEditorModule = FModuleManager::Get().LoadModuleChecked<FOdysseyEditorModule>(TEXT("OdysseyEditor"));
    TSharedPtr<FOdysseyAnimationCellClipboardData> clipboardData = MakeShared<FOdysseyAnimationCellClipboardData>(selectedCells);
    odysseyEditorModule.GetClipboard()->SetData(clipboardData);
    Action_Delete();
}

void
FOdysseyAnimationTimelineCellsShortcuts::Action_Paste()
{
    UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(mLayerStack->CurrentLayer.Get());
    if (!layer)
        return;

    if (layer->GetIsLocked())
        return;

    TSharedPtr<FOdysseyAnimationCellsContainer> cellsContainer = layer->GetCellsContainer();
    if (!cellsContainer)
        return;

    FOdysseyEditorModule& odysseyEditorModule = FModuleManager::Get().LoadModuleChecked<FOdysseyEditorModule>(TEXT("OdysseyEditor"));
    TSharedPtr<FOdysseyAnimationCellClipboardData> clipboardData = odysseyEditorModule.GetClipboard()->GetData<FOdysseyAnimationCellClipboardData>();
    if (!clipboardData)
        return;
    
    if (!clipboardData->CanPaste(layer))
        return;

#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("timeline.shortcuts.paste-frame", "Paste Frames"));
#endif

    clipboardData->Paste(layer, layer->GetAnimation()->CurrentFrame);
}

void
FOdysseyAnimationTimelineCellsShortcuts::Action_SelectAll()
{
    UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(mLayerStack->CurrentLayer.Get());
    if (!layer)
        return;

    TSharedPtr<FOdysseyAnimationCellsContainer> cellsContainer = layer->GetCellsContainer();
    if (!cellsContainer)
        return;

    mAnimationExtension->Timeline()->SetSelectedCells(cellsContainer->GetCells());
}

void
FOdysseyAnimationTimelineCellsShortcuts::Action_Delete()
{
    UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(mLayerStack->CurrentLayer.Get());
    if (!layer)
        return;

    if (layer->GetIsLocked())
        return;

    TSharedPtr<FOdysseyAnimationCellsContainer> cellsContainer = layer->GetCellsContainer();
    if (!cellsContainer)
        return;

    const TArray<TSharedPtr<FOdysseyAnimationCell>> selectedCells = mAnimationExtension->Timeline()->GetSelectedCells();
    if (selectedCells.IsEmpty())
        return;

#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("timeline.shortcuts.remove-frame", "Remove Frames"));
#endif

    FOdysseyAnimationCellsMutator mutator(layer, cellsContainer);
    mutator.Remove(selectedCells);
    mutator.Commit();
}



void
FOdysseyAnimationTimelineCellsShortcuts::Action_ConvertToStaggerCell()
{
    UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(mLayerStack->CurrentLayer.Get());
    if (!layer)
        return;

    if (layer->GetIsLocked())
        return;

    TSharedPtr<FOdysseyAnimationCellsContainer> cellsContainer = layer->GetCellsContainer();
    if (!cellsContainer)
        return;

    TArray<TSharedPtr<FOdysseyAnimationCell>> selectedCells = mAnimationExtension->Timeline()->GetSelectedCells();
    selectedCells.FilterByPredicate(
        [](TSharedPtr<FOdysseyAnimationCell> iCell)
        {
            return iCell->GetType() != FOdysseyAnimationCellImageStagger::StaticType();
        }
    );

    if (selectedCells.IsEmpty())
        return;

#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("timeline-cells.transaction.create-stagger-cell", "Stagger Cell"));
#endif

    FOdysseyAnimationCellsMutator mutator(layer, cellsContainer);
    for (TSharedPtr<FOdysseyAnimationCell> cell : selectedCells)
    {
        int cellIndex = cellsContainer->GetCells().Find(cell);
        if (cellIndex == INDEX_NONE)
            continue;

        TSharedPtr<FOdysseyAnimationCellImageStagger> cellStagger = FOdysseyAnimationCellImageStagger::Create(layer, cell->GetLength());
        mutator.Remove(cellIndex);
        mutator.Add({cellStagger}, cellIndex);
    }
    mutator.Commit();
}

bool
FOdysseyAnimationTimelineCellsShortcuts::CanAction_Copy()
{
    UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(mLayerStack->CurrentLayer.Get());
    if (!layer)
        return false;

    TSharedPtr<FOdysseyAnimationCellsContainer> cellsContainer = layer->GetCellsContainer();
    if (!cellsContainer)
        return false;

    const TArray<TSharedPtr<FOdysseyAnimationCell>> selectedCells = mAnimationExtension->Timeline()->GetSelectedCells();
    if (selectedCells.IsEmpty())
        return false;

    return true;
}

bool
FOdysseyAnimationTimelineCellsShortcuts::CanAction_Cut()
{
    UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(mLayerStack->CurrentLayer.Get());
    if (!layer)
        return false;

    if (layer->GetIsLocked())
        return false;

    TSharedPtr<FOdysseyAnimationCellsContainer> cellsContainer = layer->GetCellsContainer();
    if (!cellsContainer)
        return false;

    const TArray<TSharedPtr<FOdysseyAnimationCell>> selectedCells = mAnimationExtension->Timeline()->GetSelectedCells();
    if (selectedCells.IsEmpty())
        return false;

    return true;
}

bool
FOdysseyAnimationTimelineCellsShortcuts::CanAction_Paste()
{
    UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(mLayerStack->CurrentLayer.Get());
    if (!layer)
        return false;

    if (layer->GetIsLocked())
        return false;

    FOdysseyEditorModule& odysseyEditorModule = FModuleManager::Get().LoadModuleChecked<FOdysseyEditorModule>(TEXT("OdysseyEditor"));
    TSharedPtr<FOdysseyAnimationCellClipboardData> clipboardData = odysseyEditorModule.GetClipboard()->GetData<FOdysseyAnimationCellClipboardData>();
    if (!clipboardData)
        return false;

    return clipboardData->CanPaste(layer);
}

bool
FOdysseyAnimationTimelineCellsShortcuts::CanAction_SelectAll()
{
    UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(mLayerStack->CurrentLayer.Get());
    if (!layer)
        return false;
        
    //Authorize SelectAll only if there is already an active selection
    const TArray<TSharedPtr<FOdysseyAnimationCell>> selectedCells = mAnimationExtension->Timeline()->GetSelectedCells();
    if (selectedCells.IsEmpty())
        return false;

    return true;
}

bool
FOdysseyAnimationTimelineCellsShortcuts::CanAction_Delete()
{
    UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(mLayerStack->CurrentLayer.Get());
    if (!layer)
        return false;

    if (layer->GetIsLocked())
        return false;

    TSharedPtr<FOdysseyAnimationCellsContainer> cellsContainer = layer->GetCellsContainer();
    if (!cellsContainer)
        return false;

    const TArray<TSharedPtr<FOdysseyAnimationCell>> selectedCells = mAnimationExtension->Timeline()->GetSelectedCells();
    if (selectedCells.IsEmpty())
        return false;

    return true;
}

bool
FOdysseyAnimationTimelineCellsShortcuts::CanAction_ConvertToStaggerCell()
{
    
    UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(mLayerStack->CurrentLayer.Get());
    if (!layer)
        return false;

    if (layer->GetIsLocked())
        return false;

    TSharedPtr<FOdysseyAnimationCellsContainer> cellsContainer = layer->GetCellsContainer();
    if (!cellsContainer)
        return false;

    TArray<TSharedPtr<FOdysseyAnimationCell>> selectedCells = mAnimationExtension->Timeline()->GetSelectedCells();
    selectedCells.FilterByPredicate(
        [](TSharedPtr<FOdysseyAnimationCell> iCell)
        {
            return iCell->GetType() != FOdysseyAnimationCellImageStagger::StaticType();
        }
    );

    if (selectedCells.IsEmpty())
        return false;

    return true;
}

#undef LOCTEXT_NAMESPACE