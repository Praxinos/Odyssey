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

FOdysseyAnimationTimelineCellsShortcuts::FOdysseyAnimationTimelineCellsShortcuts(TSharedPtr<SOdysseyAnimationLayerStackTreeView> iTreeView)
    : mTreeView(iTreeView)
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
        FOdysseyAnimationEditorCommands::Get().StaggerCell,
        FExecuteAction::CreateRaw(this, &FOdysseyAnimationTimelineCellsShortcuts::Action_StaggerCell),
        FCanExecuteAction::CreateRaw(this, &FOdysseyAnimationTimelineCellsShortcuts::CanAction_StaggerCell)
    );
}

void
FOdysseyAnimationTimelineCellsShortcuts::Action_Copy()
{
    TSharedPtr<SOdysseyAnimationLayerStackTreeView> treeView = mTreeView.Pin();
    if (!treeView)
        return;

    UOdysseyLayerStack* layerstack = treeView->GetLayerStack();
    if(!layerstack)
        return;

    UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(layerstack->CurrentLayer.Get());
    if (!layer)
        return;

    FInt32Range selectedFrames = treeView->GetAnimationEditorExtension()->Timeline()->GetSelectedFrames();
    if (selectedFrames.IsEmpty())
        return;

    FOdysseyEditorModule& odysseyEditorModule = FModuleManager::Get().LoadModuleChecked<FOdysseyEditorModule>(TEXT("OdysseyEditor"));

    TSharedPtr<FOdysseyAnimationCellClipboardData> clipboardData = MakeShared<FOdysseyAnimationCellClipboardData>(layer, selectedFrames);
    odysseyEditorModule.GetClipboard()->SetData(clipboardData);
}

void
FOdysseyAnimationTimelineCellsShortcuts::Action_Cut()
{
    TSharedPtr<SOdysseyAnimationLayerStackTreeView> treeView = mTreeView.Pin();
    if (!treeView)
        return;

    UOdysseyLayerStack* layerstack = treeView->GetLayerStack();
    if(!layerstack)
        return;

    UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(layerstack->CurrentLayer.Get());
    if (!layer)
        return;

    if (layer->GetIsLocked())
    {
        Action_Copy();
        return;
    }

    FInt32Range selectedFrames = treeView->GetAnimationEditorExtension()->Timeline()->GetSelectedFrames();
    if (selectedFrames.IsEmpty())
        return;

#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("timeline.shortcuts.cut-frame", "Cut Frames"));
#endif
    
    FOdysseyEditorModule& odysseyEditorModule = FModuleManager::Get().LoadModuleChecked<FOdysseyEditorModule>(TEXT("OdysseyEditor"));
    TSharedPtr<FOdysseyAnimationCellClipboardData> clipboardData = MakeShared<FOdysseyAnimationCellClipboardData>(layer, selectedFrames);
    odysseyEditorModule.GetClipboard()->SetData(clipboardData);
    Action_Delete();
}

void
FOdysseyAnimationTimelineCellsShortcuts::Action_Paste()
{
    TSharedPtr<SOdysseyAnimationLayerStackTreeView> treeView = mTreeView.Pin();
    if (!treeView)
        return;

    UOdysseyLayerStack* layerstack = treeView->GetLayerStack();
    if(!layerstack)
        return;

    UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(layerstack->CurrentLayer.Get());
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
    TSharedPtr<SOdysseyAnimationLayerStackTreeView> treeView = mTreeView.Pin();
    if (!treeView)
        return;

    UOdysseyLayerStack* layerstack = treeView->GetLayerStack();
    if(!layerstack)
        return;

    UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(layerstack->CurrentLayer.Get());
    if (!layer)
        return;

    FInt32Range frameRange = layer->GetFrameRange();
    treeView->GetAnimationEditorExtension()->Timeline()->SetSelectedFrames(frameRange);
}

void
FOdysseyAnimationTimelineCellsShortcuts::Action_Delete()
{
    TSharedPtr<SOdysseyAnimationLayerStackTreeView> treeView = mTreeView.Pin();
    if (!treeView)
        return;

    UOdysseyLayerStack* layerstack = treeView->GetLayerStack();
    if(!layerstack)
        return;

    UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(layerstack->CurrentLayer.Get());
    if (!layer)
        return;

    if (layer->GetIsLocked())
        return;

    TSharedPtr<FOdysseyAnimationCellsContainer> cellsContainer = layer->GetCellsContainer();
    if (!cellsContainer)
        return;

    FInt32Range selectedFrames = treeView->GetAnimationEditorExtension()->Timeline()->GetSelectedFrames();
    if (selectedFrames.IsEmpty())
        return;

    bool isLowerClosed = selectedFrames.GetLowerBound().IsClosed();
    bool isUpperClosed = selectedFrames.GetUpperBound().IsClosed();

    if (!isLowerClosed || !isUpperClosed)
        return;

    if (FInt32Range::Difference(cellsContainer->GetFrameRange(), selectedFrames).IsEmpty())
    {
        selectedFrames.SetLowerBoundValue(selectedFrames.GetLowerBoundValue() + 1);
        if (selectedFrames.IsEmpty())
            return;
    }

#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("timeline.shortcuts.remove-frame", "Remove Frames"));
#endif

    FOdysseyAnimationCellsMutator mutator(layer, cellsContainer);
    mutator.RemoveFrameRange(selectedFrames);
    mutator.Commit();
}



void
FOdysseyAnimationTimelineCellsShortcuts::Action_StaggerCell()
{
    TSharedPtr<SOdysseyAnimationLayerStackTreeView> treeView = mTreeView.Pin();
    if (!treeView)
        return;

    UOdysseyLayerStack* layerstack = treeView->GetLayerStack();
    if(!layerstack)
        return;

    UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(layerstack->CurrentLayer.Get());
    if (!layer)
        return;

    if (layer->GetIsLocked())
        return;

    TSharedPtr<FOdysseyAnimationCellsContainer> cellsContainer = layer->GetCellsContainer();
    if (!cellsContainer)
        return;

    int cellFrame = cellsContainer->GetCellFrameAtFrame(layer->GetAnimation()->CurrentFrame);
    if (cellFrame == INDEX_NONE || cellFrame == 0)
        return;

    int cellIndex = cellsContainer->GetCellIndexAtFrame(layer->GetAnimation()->CurrentFrame);
    if (cellIndex == INDEX_NONE)
        return;

    TSharedPtr<FOdysseyAnimationCell> cell = cellsContainer->GetCells()[cellIndex];
    if (!cell || cell->GetType() == FOdysseyAnimationCellImageStagger::StaticType())
        return;

#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("timeline-cells.transaction.create-stagger-cell", "Stagger Cell"));
#endif
    
    int cellStaggerLength = cell->GetLength() - cellFrame;
    TSharedPtr<FOdysseyAnimationCellImageStagger> cellStagger = FOdysseyAnimationCellImageStagger::Create(layer, cellStaggerLength);
    
    FOdysseyAnimationCellsMutator mutator(layer, cellsContainer);
    mutator.SetLength( cellIndex, cellFrame );
    mutator.Add({cellStagger}, cellIndex + 1);
    mutator.Commit();
}

bool
FOdysseyAnimationTimelineCellsShortcuts::CanAction_Copy()
{
    TSharedPtr<SOdysseyAnimationLayerStackTreeView> treeView = mTreeView.Pin();
    if (!treeView)
        return false;

    UOdysseyLayerStack* layerstack = treeView->GetLayerStack();
    if(!layerstack)
        return false;

    UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(layerstack->CurrentLayer.Get());
    if (!layer)
        return false;

    TSharedPtr<FOdysseyAnimationCellsContainer> cellsContainer = layer->GetCellsContainer();
    if (!cellsContainer)
        return false;

    FInt32Range selectedFrames = treeView->GetAnimationEditorExtension()->Timeline()->GetSelectedFrames();
    if (selectedFrames.IsEmpty())
        return false;

    return true;
}

bool
FOdysseyAnimationTimelineCellsShortcuts::CanAction_Cut()
{
    TSharedPtr<SOdysseyAnimationLayerStackTreeView> treeView = mTreeView.Pin();
    if (!treeView)
        return false;

    UOdysseyLayerStack* layerstack = treeView->GetLayerStack();
    if(!layerstack)
        return false;

    UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(layerstack->CurrentLayer.Get());
    if (!layer)
        return false;

    if (layer->GetIsLocked())
        return false;

    TSharedPtr<FOdysseyAnimationCellsContainer> cellsContainer = layer->GetCellsContainer();
    if (!cellsContainer)
        return false;

    FInt32Range selectedFrames = treeView->GetAnimationEditorExtension()->Timeline()->GetSelectedFrames();
    if (selectedFrames.IsEmpty())
        return false;

    return true;
}

bool
FOdysseyAnimationTimelineCellsShortcuts::CanAction_Paste()
{
    TSharedPtr<SOdysseyAnimationLayerStackTreeView> treeView = mTreeView.Pin();
    if (!treeView)
        return false;

    UOdysseyLayerStack* layerstack = treeView->GetLayerStack();
    if(!layerstack)
        return false;

    UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(layerstack->CurrentLayer.Get());
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
    TSharedPtr<SOdysseyAnimationLayerStackTreeView> treeView = mTreeView.Pin();
    if (!treeView)
        return false;

    UOdysseyLayerStack* layerstack = treeView->GetLayerStack();
    if(!layerstack)
        return false;

    UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(layerstack->CurrentLayer.Get());
    if (!layer)
        return false;
        
    //Authorize SelectAll only if there is already an active selection
    if (treeView->GetAnimationEditorExtension()->Timeline()->GetSelectedFrames().IsEmpty())
        return false;

    return true;
}

bool
FOdysseyAnimationTimelineCellsShortcuts::CanAction_Delete()
{
    TSharedPtr<SOdysseyAnimationLayerStackTreeView> treeView = mTreeView.Pin();
    if (!treeView)
        return false;

    UOdysseyLayerStack* layerstack = treeView->GetLayerStack();
    if(!layerstack)
        return false;

    UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(layerstack->CurrentLayer.Get());
    if (!layer)
        return false;

    if (layer->GetIsLocked())
        return false;

    TSharedPtr<FOdysseyAnimationCellsContainer> cellsContainer = layer->GetCellsContainer();
    if (!cellsContainer)
        return false;

    FInt32Range selectedFrames = treeView->GetAnimationEditorExtension()->Timeline()->GetSelectedFrames();
    if (selectedFrames.IsEmpty())
        return false;

    return true;
}

bool
FOdysseyAnimationTimelineCellsShortcuts::CanAction_StaggerCell()
{
    TSharedPtr<SOdysseyAnimationLayerStackTreeView> treeView = mTreeView.Pin();
    if (!treeView)
        return false;

    UOdysseyLayerStack* layerstack = treeView->GetLayerStack();
    if(!layerstack)
        return false;

    UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(layerstack->CurrentLayer.Get());
    if (!layer)
        return false;

    if (layer->GetIsLocked())
        return false;

    TSharedPtr<FOdysseyAnimationCellsContainer> cellsContainer = layer->GetCellsContainer();
    if (!cellsContainer)
        return false;

    int cellFrame = cellsContainer->GetCellFrameAtFrame(layer->GetAnimation()->CurrentFrame);
    if (cellFrame == INDEX_NONE || cellFrame == 0)
        return false;

    int cellIndex = cellsContainer->GetCellIndexAtFrame(layer->GetAnimation()->CurrentFrame);
    if (cellIndex == INDEX_NONE)
        return false;

    TSharedPtr<FOdysseyAnimationCell> cell = cellsContainer->GetCells()[cellIndex];
    if (!cell || cell->GetType() == FOdysseyAnimationCellImageStagger::StaticType())
        return false;

    return true;
}

#undef LOCTEXT_NAMESPACE