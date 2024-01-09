// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Shortcuts/Global/OdysseyAnimationGlobalTimelineShortcuts.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

FOdysseyAnimationGlobalTimelineShortcuts::FOdysseyAnimationGlobalTimelineShortcuts(TSharedPtr<FOdysseyAnimationEditorExtension> iExtension)
    : mExtension(iExtension)
{
}

void
FOdysseyAnimationGlobalTimelineShortcuts::MapActionsToCommandList(TSharedRef<FUICommandList> iCommandList)
{
    iCommandList->MapAction(
        FOdysseyAnimationEditorCommands::Get().ActivateTimelineSelectionTool,
        FExecuteAction::CreateRaw(this, &FOdysseyAnimationGlobalTimelineShortcuts::Action_ActivateSelectionTool),
        FCanExecuteAction::CreateRaw(this, &FOdysseyAnimationGlobalTimelineShortcuts::CanAction_ActivateSelectionTool)
    );

    iCommandList->MapAction(
        FOdysseyAnimationEditorCommands::Get().ActivateTimelineMoveTool,
        FExecuteAction::CreateRaw(this, &FOdysseyAnimationGlobalTimelineShortcuts::Action_ActivateMoveTool),
        FCanExecuteAction::CreateRaw(this, &FOdysseyAnimationGlobalTimelineShortcuts::CanAction_ActivateMoveTool)
    );
    
    iCommandList->MapAction(
        FOdysseyAnimationEditorCommands::Get().ActivateTimelineCutTool,
        FExecuteAction::CreateRaw(this, &FOdysseyAnimationGlobalTimelineShortcuts::Action_ActivateCutTool),
        FCanExecuteAction::CreateRaw(this, &FOdysseyAnimationGlobalTimelineShortcuts::CanAction_ActivateCutTool)
    );
}

void
FOdysseyAnimationGlobalTimelineShortcuts::Action_ActivateSelectionTool()
{
    TSharedPtr<FOdysseyAnimationEditorExtension> extension = mExtension.Pin();
    if (!extension)
        return;

    extension->Timeline()->SetSelectedTool(EOdysseyTimelineTool::Selection);
}

void
FOdysseyAnimationGlobalTimelineShortcuts::Action_ActivateMoveTool()
{
    TSharedPtr<FOdysseyAnimationEditorExtension> extension = mExtension.Pin();
    if (!extension)
        return;

    extension->Timeline()->SetSelectedTool(EOdysseyTimelineTool::Move);
}

void
FOdysseyAnimationGlobalTimelineShortcuts::Action_ActivateCutTool()
{
    TSharedPtr<FOdysseyAnimationEditorExtension> extension = mExtension.Pin();
    if (!extension)
        return;

    extension->Timeline()->SetSelectedTool(EOdysseyTimelineTool::Cut);
}

bool
FOdysseyAnimationGlobalTimelineShortcuts::CanAction_ActivateSelectionTool()
{
    return true;
}

bool
FOdysseyAnimationGlobalTimelineShortcuts::CanAction_ActivateMoveTool()
{
    return true;
}

bool
FOdysseyAnimationGlobalTimelineShortcuts::CanAction_ActivateCutTool()
{   
    return true;
}

#undef LOCTEXT_NAMESPACE