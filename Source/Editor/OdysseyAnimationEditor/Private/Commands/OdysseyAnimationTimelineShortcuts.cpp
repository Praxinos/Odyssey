// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Commands/OdysseyAnimationTimelineShortcuts.h"

#include "Widgets/LayerStack/SOdysseyAnimationLayerStackTreeView.h"
#include "LayerStack/OdysseyAnimationLayerStack.h"
#include "OdysseyAnimationEditorTimeline.h"
#include "LayerStack/Layers/OdysseyAnimationLayer.h"
#include "LayerStack/Cells/OdysseyAnimationCellClipboardData.h"
#include "Framework/Commands/GenericCommands.h"

#define LOCTEXT_NAMESPACE "FOdysseyLayerStackShortcuts"

FOdysseyAnimationTimelineShortcuts::FOdysseyAnimationTimelineShortcuts(TSharedPtr<SOdysseyAnimationLayerStackTreeView> iTreeView, UOdysseyAnimationLayerStack* iLayerStack, FOdysseyAnimationEditorTimeline* iTimelineContext)
    : mCommandList(MakeShared<FUICommandList>())
    , mTreeView(iTreeView)
    , mLayerStack(iLayerStack)
    , mTimelineContext(iTimelineContext)
{
    MapActionsToCommandList();
}

TSharedRef<FUICommandList>
FOdysseyAnimationTimelineShortcuts::GetCommandList() const
{
    return mCommandList;
}

void
FOdysseyAnimationTimelineShortcuts::MapActionsToCommandList()
{
    mCommandList->MapAction(
        FGenericCommands::Get().Copy,
        FExecuteAction::CreateRaw(this, &FOdysseyAnimationTimelineShortcuts::Action_Copy),
        FCanExecuteAction::CreateRaw(this, &FOdysseyAnimationTimelineShortcuts::CanAction_Copy)
    );

    mCommandList->MapAction(
        FGenericCommands::Get().Paste,
        FExecuteAction::CreateRaw(this, &FOdysseyAnimationTimelineShortcuts::Action_Paste),
        FCanExecuteAction::CreateRaw(this, &FOdysseyAnimationTimelineShortcuts::CanAction_Paste)
    );

    mCommandList->MapAction(
        FGenericCommands::Get().Cut,
        FExecuteAction::CreateRaw(this, &FOdysseyAnimationTimelineShortcuts::Action_Cut),
        FCanExecuteAction::CreateRaw(this, &FOdysseyAnimationTimelineShortcuts::CanAction_Cut)
    );

    mCommandList->MapAction(
        FGenericCommands::Get().SelectAll,
        FExecuteAction::CreateRaw(this, &FOdysseyAnimationTimelineShortcuts::Action_SelectAll),
        FCanExecuteAction::CreateRaw(this, &FOdysseyAnimationTimelineShortcuts::CanAction_SelectAll)
    );

    mCommandList->MapAction(
        FGenericCommands::Get().Delete,
        FExecuteAction::CreateRaw(this, &FOdysseyAnimationTimelineShortcuts::Action_Delete),
        FCanExecuteAction::CreateRaw(this, &FOdysseyAnimationTimelineShortcuts::CanAction_Delete)
    );

    mCommandList->MapAction(
        FOdysseyAnimationEditorCommands::Get().StaggerCell,
        FExecuteAction::CreateRaw(this, &FOdysseyAnimationTimelineShortcuts::Action_StaggerCell),
        FCanExecuteAction::CreateRaw(this, &FOdysseyAnimationTimelineShortcuts::CanAction_StaggerCell)
    );
}

void
FOdysseyAnimationTimelineShortcuts::Action_Copy()
{
    UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(mLayerStack->CurrentLayer.Get());
    if (!layer)
        return;

    FInt32Range selectedFrames = mTimelineContext->GetSelectedFrames();
    if (selectedFrames.IsEmpty())
        return;

    TSharedPtr<FOdysseyAnimationCellClipboardData> clipboardData = MakeShared<FOdysseyAnimationCellClipboardData>(layer, selectedFrames);
    FOdysseyClipboard::Get().SetData(clipboardData);
}

void
FOdysseyAnimationTimelineShortcuts::Action_Cut()
{
    UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(mLayerStack->CurrentLayer.Get());
    if (!layer)
        return;

    FInt32Range selectedFrames = mTimelineContext->GetSelectedFrames();
    if (selectedFrames.IsEmpty())
        return;

#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("Timeline", "Cut Frames"));
#endif
    TSharedPtr<FOdysseyAnimationCellClipboardData> clipboardData = MakeShared<FOdysseyAnimationCellClipboardData>(layer, selectedFrames);
    FOdysseyClipboard::Get().SetData(clipboardData);
    Action_Delete();
}

void
FOdysseyAnimationTimelineShortcuts::Action_Paste()
{
    UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(mLayerStack->CurrentLayer.Get());
    if (!layer)
        return;

    TSharedPtr<FOdysseyAnimationCellsContainer> cellsContainer = layer->GetCellsContainer();
    if (!cellsContainer)
        return;

    TSharedPtr<FOdysseyAnimationCellClipboardData> clipboardData = FOdysseyClipboard::Get().GetData<FOdysseyAnimationCellClipboardData>();
    if (!clipboardData)
        return;
    
    if (!clipboardData->CanPaste(layer))
        return;

#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("Timeline", "Paste Frames"));
#endif

    clipboardData->Paste(layer, layer->GetAnimation()->CurrentFrame);
}

void
FOdysseyAnimationTimelineShortcuts::Action_SelectAll()
{
    UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(mLayerStack->CurrentLayer.Get());
    if (!layer)
        return;

    FInt32Range frameRange = layer->GetFrameRange();
    mTimelineContext->SetSelectedFrames(frameRange);
}

void
FOdysseyAnimationTimelineShortcuts::Action_Delete()
{
    UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(mLayerStack->CurrentLayer.Get());
    if (!layer)
        return;

    TSharedPtr<FOdysseyAnimationCellsContainer> cellsContainer = layer->GetCellsContainer();
    if (!cellsContainer)
        return;

    FInt32Range selectedFrames = mTimelineContext->GetSelectedFrames();
    if (selectedFrames.IsEmpty())
        return;

#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("Layer", "Remove Frames"));
#endif

    FOdysseyAnimationCellsMutator mutator(layer, cellsContainer);
    mutator.RemoveFrameRange(selectedFrames);
    mutator.Commit();
}



void
FOdysseyAnimationTimelineShortcuts::Action_StaggerCell()
{
    /* UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(mLayerStack->CurrentLayer.Get());
    if (!layer)
        return;

    TSharedPtr<FOdysseyAnimationCellsContainer> cellsContainer = layer->GetCellsContainer();
    if (!cellsContainer)
        return;

    FInt32Range selectedFrames = mTimelineContext->GetSelectedFrames();
    if (selectedFrames.IsEmpty())
        return;

#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("Layer", "Remove Frames"));
#endif

    FOdysseyAnimationCellsMutator mutator(layer, cellsContainer);
    mutator.RemoveFrameRange(selectedFrames);
    mutator.Commit(); */
}

bool
FOdysseyAnimationTimelineShortcuts::CanAction_Copy()
{
    UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(mLayerStack->CurrentLayer.Get());
    if (!layer)
        return false;

    TSharedPtr<FOdysseyAnimationCellsContainer> cellsContainer = layer->GetCellsContainer();
    if (!cellsContainer)
        return false;

    FInt32Range selectedFrames = mTimelineContext->GetSelectedFrames();
    if (selectedFrames.IsEmpty())
        return false;

    return true;
}

bool
FOdysseyAnimationTimelineShortcuts::CanAction_Cut()
{
    UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(mLayerStack->CurrentLayer.Get());
    if (!layer)
        return false;

    TSharedPtr<FOdysseyAnimationCellsContainer> cellsContainer = layer->GetCellsContainer();
    if (!cellsContainer)
        return false;

    FInt32Range selectedFrames = mTimelineContext->GetSelectedFrames();
    if (selectedFrames.IsEmpty())
        return false;

    return true;
}

bool
FOdysseyAnimationTimelineShortcuts::CanAction_Paste()
{
    UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(mLayerStack->CurrentLayer.Get());
    if (!layer)
        return false;

    TSharedPtr<FOdysseyAnimationCellClipboardData> clipboardData = FOdysseyClipboard::Get().GetData<FOdysseyAnimationCellClipboardData>();
    if (!clipboardData)
        return false;

    return clipboardData->CanPaste(layer);
}

bool
FOdysseyAnimationTimelineShortcuts::CanAction_SelectAll()
{
    UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(mLayerStack->CurrentLayer.Get());
    if (!layer)
        return false;
        
    //Authorize SelectAll only if there is already an active selection
    if (mTimelineContext->GetSelectedFrames().IsEmpty())
        return false;

    return true;
}

bool
FOdysseyAnimationTimelineShortcuts::CanAction_Delete()
{
    UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(mLayerStack->CurrentLayer.Get());
    if (!layer)
        return false;

    TSharedPtr<FOdysseyAnimationCellsContainer> cellsContainer = layer->GetCellsContainer();
    if (!cellsContainer)
        return false;

    FInt32Range selectedFrames = mTimelineContext->GetSelectedFrames();
    if (selectedFrames.IsEmpty())
        return false;

    return true;
}

bool
FOdysseyAnimationTimelineShortcuts::CanAction_StaggerCell()
{
    /* UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(mLayerStack->CurrentLayer.Get());
    if (!layer)
        return false;

    TSharedPtr<FOdysseyAnimationCellsContainer> cellsContainer = layer->GetCellsContainer();
    if (!cellsContainer)
        return false;

    FInt32Range selectedFrames = mTimelineContext->GetSelectedFrames();
    if (selectedFrames.IsEmpty())
        return false;

    return true; */
    return true;
}

#undef LOCTEXT_NAMESPACE