// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Shortcuts/Global/OdysseyPainterEditorGlobalToolsShortcuts.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

FOdysseyPainterEditorGlobalToolsShortcuts::FOdysseyPainterEditorGlobalToolsShortcuts(TSharedPtr<FOdysseyPainterEditor> iEditor)
    : mEditor(iEditor)
{
}

void
FOdysseyPainterEditorGlobalToolsShortcuts::MapActionsToCommandList(TSharedRef<FUICommandList> iCommandList)
{   
    TSharedPtr<FOdysseyPainterEditor> editor = mEditor.Pin();
    if (!editor)
        return;

    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().ActivateColorPickerTool,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivateTool, Cast<UOdysseyPainterEditorTool>(editor->GetColorPickerTool())),
        FCanExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::CanAction_ActivateTool, Cast<UOdysseyPainterEditorTool>(editor->GetColorPickerTool()))
    );
    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().ActivateRasterDrawingTool,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivateTool, Cast<UOdysseyPainterEditorTool>(editor->GetRasterDrawingTool())),
        FCanExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::CanAction_ActivateTool, Cast<UOdysseyPainterEditorTool>(editor->GetRasterDrawingTool()))
    );
    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().ActivateRasterPaintBucketTool,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivateTool, Cast<UOdysseyPainterEditorTool>(editor->GetRasterPaintBucketTool())),
        FCanExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::CanAction_ActivateTool, Cast<UOdysseyPainterEditorTool>(editor->GetRasterPaintBucketTool()))
    );
    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().ActivateRasterPrimitiveDrawingTool,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivateTool, Cast<UOdysseyPainterEditorTool>(editor->GetRasterPrimitiveDrawingTool())),
        FCanExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::CanAction_ActivateTool, Cast<UOdysseyPainterEditorTool>(editor->GetRasterPrimitiveDrawingTool()))
    );
    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().ActivateRasterTransformTool,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivateTool, Cast<UOdysseyPainterEditorTool>(editor->GetRasterTransformTool())),
        FCanExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::CanAction_ActivateTool, Cast<UOdysseyPainterEditorTool>(editor->GetRasterTransformTool()))
    );
    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().ActivateVectorEraserTool,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivateTool, Cast<UOdysseyPainterEditorTool>(editor->GetVectorEraserTool())),
        FCanExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::CanAction_ActivateTool, Cast<UOdysseyPainterEditorTool>(editor->GetVectorEraserTool()))
    );
    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().ActivateVectorGridTool,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivateTool, Cast<UOdysseyPainterEditorTool>(editor->GetVectorGridTool())),
        FCanExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::CanAction_ActivateTool, Cast<UOdysseyPainterEditorTool>(editor->GetVectorGridTool()))
    );
    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().ActivateVectorPaintBucketTool,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivateTool, Cast<UOdysseyPainterEditorTool>(editor->GetVectorPaintBucketTool())),
        FCanExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::CanAction_ActivateTool, Cast<UOdysseyPainterEditorTool>(editor->GetVectorPaintBucketTool()))
    );
    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().ActivateVectorPathCutTool,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivateTool, Cast<UOdysseyPainterEditorTool>(editor->GetVectorPathCutTool())),
        FCanExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::CanAction_ActivateTool, Cast<UOdysseyPainterEditorTool>(editor->GetVectorPathCutTool()))
    );
    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().ActivateVectorPathDrawingTool,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivateTool, Cast<UOdysseyPainterEditorTool>(editor->GetVectorPathDrawingTool())),
        FCanExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::CanAction_ActivateTool, Cast<UOdysseyPainterEditorTool>(editor->GetVectorPathDrawingTool()))
    );
    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().ActivateVectorPathEditTool,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivateTool, Cast<UOdysseyPainterEditorTool>(editor->GetVectorPathEditTool())),
        FCanExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::CanAction_ActivateTool, Cast<UOdysseyPainterEditorTool>(editor->GetVectorPathEditTool()))
    );
    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().ActivateVectorPathPushTool,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivateTool, Cast<UOdysseyPainterEditorTool>(editor->GetVectorPathPushTool())),
        FCanExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::CanAction_ActivateTool, Cast<UOdysseyPainterEditorTool>(editor->GetVectorPathPushTool()))
    );
    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().ActivateVectorPathSmoothTool,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivateTool, Cast<UOdysseyPainterEditorTool>(editor->GetVectorPathSmoothTool())),
        FCanExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::CanAction_ActivateTool, Cast<UOdysseyPainterEditorTool>(editor->GetVectorPathSmoothTool()))
    );
    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().ActivateVectorPathStitchTool,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivateTool, Cast<UOdysseyPainterEditorTool>(editor->GetVectorPathStitchTool())),
        FCanExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::CanAction_ActivateTool, Cast<UOdysseyPainterEditorTool>(editor->GetVectorPathStitchTool()))
    );
    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().ActivateVectorPrimitiveDrawingTool,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivateTool, Cast<UOdysseyPainterEditorTool>(editor->GetVectorPrimitiveDrawingTool())),
        FCanExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::CanAction_ActivateTool, Cast<UOdysseyPainterEditorTool>(editor->GetVectorPrimitiveDrawingTool()))
    );
    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().ActivateVectorScenePanTool,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivateTool, Cast<UOdysseyPainterEditorTool>(editor->GetVectorScenePanTool())),
        FCanExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::CanAction_ActivateTool, Cast<UOdysseyPainterEditorTool>(editor->GetVectorScenePanTool()))
    );
    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().ActivateVectorSelectionTool,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivateTool, Cast<UOdysseyPainterEditorTool>(editor->GetVectorSelectionTool())),
        FCanExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::CanAction_ActivateTool, Cast<UOdysseyPainterEditorTool>(editor->GetVectorSelectionTool()))
    );
    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().ActivateVectorTransformTool,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivateTool, Cast<UOdysseyPainterEditorTool>(editor->GetVectorTransformTool())),
        FCanExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::CanAction_ActivateTool, Cast<UOdysseyPainterEditorTool>(editor->GetVectorTransformTool()))
    );
}

void
FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivateTool(UOdysseyPainterEditorTool* iTool)
{
    TSharedPtr<FOdysseyPainterEditor> editor = mEditor.Pin();
    if (!editor)
        return;

    if (!iTool->IsActivable() || iTool->IsActivated() )
        return;

    editor->SetSelectedTool(iTool);
}

bool
FOdysseyPainterEditorGlobalToolsShortcuts::CanAction_ActivateTool(UOdysseyPainterEditorTool* iTool)
{
    TSharedPtr<FOdysseyPainterEditor> editor = mEditor.Pin();
    if (!editor)
        return false;

    if (!iTool->IsActivable() || iTool->IsActivated() )
        return false;

    return true;
}

#undef LOCTEXT_NAMESPACE