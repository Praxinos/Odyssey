// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023
#include "Shortcuts/Global/OdysseyAnimationGlobalTimelineToolsShortcuts.h"
#include "OdysseyPainterEditorAnimationCommands.h"
#include "TimelineTools/OdysseyAnimationTimelineTools.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

FOdysseyAnimationGlobalTimelineToolsShortcuts::FOdysseyAnimationGlobalTimelineToolsShortcuts()
{
}

void
FOdysseyAnimationGlobalTimelineToolsShortcuts::MapActionsToCommandList(TSharedRef<FUICommandList> iCommandList)
{
    //Tools actions
    iCommandList->MapAction(
        FOdysseyPainterEditorAnimationCommands::Get().ActivateTimelineSelectionTool,
        FExecuteAction::CreateRaw(this, &FOdysseyAnimationGlobalTimelineToolsShortcuts::Action_ActivateTimelineSelectionTool)
    );

    iCommandList->MapAction(
        FOdysseyPainterEditorAnimationCommands::Get().ActivateTimelineMoveTool,
        FExecuteAction::CreateRaw(this, &FOdysseyAnimationGlobalTimelineToolsShortcuts::Action_ActivateTimelineMoveTool)
    );

    iCommandList->MapAction(
        FOdysseyPainterEditorAnimationCommands::Get().ActivateTimelineCutTool,
        FExecuteAction::CreateRaw(this, &FOdysseyAnimationGlobalTimelineToolsShortcuts::Action_ActivateTimelineCutTool)
    );
}

void
FOdysseyAnimationGlobalTimelineToolsShortcuts::Action_ActivateTimelineSelectionTool()
{
    FOdysseyAnimationTimelineTools::Get().SetCurrentTool(EOdysseyTimelineTool::Selection);
}

void
FOdysseyAnimationGlobalTimelineToolsShortcuts::Action_ActivateTimelineMoveTool()
{
    FOdysseyAnimationTimelineTools::Get().SetCurrentTool(EOdysseyTimelineTool::Move);
}

void
FOdysseyAnimationGlobalTimelineToolsShortcuts::Action_ActivateTimelineCutTool()
{
    FOdysseyAnimationTimelineTools::Get().SetCurrentTool(EOdysseyTimelineTool::Cut);
}

#undef LOCTEXT_NAMESPACE
