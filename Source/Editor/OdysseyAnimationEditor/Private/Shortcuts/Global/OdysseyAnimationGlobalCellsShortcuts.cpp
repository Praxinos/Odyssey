// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Shortcuts/Global/OdysseyAnimationGlobalCellsShortcuts.h"

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

    if (currentLayer->IsLocked)
        return;

    TSharedPtr<FOdysseyAnimationCellsContainer> cellsContainer = currentLayer->GetCellsContainer();
    if (!cellsContainer)
        return;
        
#ifdef WITH_EDITOR
        FScopedTransaction ScopedTransaction(LOCTEXT("timeline.move-tool.transaction.set-offset", "Change Layer Offset"));
#endif
    TSharedRef<FOdysseyAnimationCellsMutator> cellsMutator = MakeShared<FOdysseyAnimationCellsMutator>(currentLayer, cellsContainer.ToSharedRef());
    cellsMutator->BreakCellAtFrame(animation->CurrentFrame);
}

bool
FOdysseyAnimationGlobalCellsShortcuts::CanAction_BreakCell()
{
    return true;
}

#undef LOCTEXT_NAMESPACE