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

    //Generic tool shortcuts

    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().ActivateColorPickerTool,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivateColorPickerTool),
        FCanExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::CanAction_ActivateColorPickerTool)
    );
    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().ActivateDrawingTool,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivateDrawingTool),
        FCanExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::CanAction_ActivateDrawingTool)
    );
    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().ActivateSelectionTool,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivateSelectionTool),
        FCanExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::CanAction_ActivateSelectionTool)
    );
    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().ActivatePaintBucketTool,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivatePaintBucketTool),
        FCanExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::CanAction_ActivatePaintBucketTool)
    );
    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().ActivateTransformTool,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivateTransformTool),
        FCanExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::CanAction_ActivateTransformTool)
    );
    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().ActivatePrimitiveDrawingTool,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivatePrimitiveDrawingTool),
        FCanExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::CanAction_ActivatePrimitiveDrawingTool)
    );
    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().ActivateEraserTool,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivateEraserTool),
        FCanExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::CanAction_ActivateEraserTool)
    );
    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().ActivateWarpTool,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivateWarpTool),
        FCanExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::CanAction_ActivateWarpTool)
    );

    //Specific tool shortcuts
    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().ActivateRasterDrawingTool,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivateTool, Cast<UOdysseyPainterEditorTool>(editor->GetRasterDrawingTool())),
        FCanExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::CanAction_ActivateTool, Cast<UOdysseyPainterEditorTool>(editor->GetRasterDrawingTool()))
    );
    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().ActivateRasterEraserTool,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivateTool, Cast<UOdysseyPainterEditorTool>(editor->GetRasterEraserTool())),
        FCanExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::CanAction_ActivateTool, Cast<UOdysseyPainterEditorTool>(editor->GetRasterEraserTool()))
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

void
FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivateColorPickerTool()
{
    TSharedPtr<FOdysseyPainterEditor> editor = mEditor.Pin();
    if (!editor)
        return;

    if (CanAction_ActivateTool(editor->GetColorPickerTool()))
    {
        Action_ActivateTool(editor->GetColorPickerTool());
        return;
    }
}

void
FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivateDrawingTool()
{
    TSharedPtr<FOdysseyPainterEditor> editor = mEditor.Pin();
    if (!editor)
        return;

    if (CanAction_ActivateTool(editor->GetRasterDrawingTool()))
    {
        Action_ActivateTool(editor->GetRasterDrawingTool());
        return;
    }

    if (CanAction_ActivateTool(editor->GetVectorPathDrawingTool()))
    {
        Action_ActivateTool(editor->GetVectorPathDrawingTool());
        return;
    }
}

void
FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivateSelectionTool()
{
    TSharedPtr<FOdysseyPainterEditor> editor = mEditor.Pin();
    if (!editor)
        return;

    /* TODO:
    if (CanAction_ActivateTool(editor->GetRasterSelectionTool()))
    {
        Action_ActivateTool(editor->GetRasterSelectionTool());
        return;
    }
    */

    if (CanAction_ActivateTool(editor->GetVectorSelectionTool()))
    {
        Action_ActivateTool(editor->GetVectorSelectionTool());
        return;
    }
}

void
FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivatePaintBucketTool()
{
    TSharedPtr<FOdysseyPainterEditor> editor = mEditor.Pin();
    if (!editor)
        return;

    if (CanAction_ActivateTool(editor->GetRasterPaintBucketTool()))
    {
        Action_ActivateTool(editor->GetRasterPaintBucketTool());
        return;
    }

    if (CanAction_ActivateTool(editor->GetVectorPaintBucketTool()))
    {
        Action_ActivateTool(editor->GetVectorPaintBucketTool());
        return;
    }
}

void
FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivateTransformTool()
{
    TSharedPtr<FOdysseyPainterEditor> editor = mEditor.Pin();
    if (!editor)
        return;

    if (CanAction_ActivateTool(editor->GetRasterTransformTool()))
    {
        Action_ActivateTool(editor->GetRasterTransformTool());
        return;
    }

    if (CanAction_ActivateTool(editor->GetVectorTransformTool()))
    {
        Action_ActivateTool(editor->GetVectorTransformTool());
        return;
    }
}

void
FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivatePrimitiveDrawingTool()
{
    TSharedPtr<FOdysseyPainterEditor> editor = mEditor.Pin();
    if (!editor)
        return;

    if (CanAction_ActivateTool(editor->GetRasterPrimitiveDrawingTool()))
    {
        Action_ActivateTool(editor->GetRasterPrimitiveDrawingTool());
        return;
    }

    if (CanAction_ActivateTool(editor->GetVectorPrimitiveDrawingTool()))
    {
        Action_ActivateTool(editor->GetVectorPrimitiveDrawingTool());
        return;
    }
}

void
FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivateEraserTool()
{
    TSharedPtr<FOdysseyPainterEditor> editor = mEditor.Pin();
    if (!editor)
        return;

    if (CanAction_ActivateTool(editor->GetRasterEraserTool()))
    {
        Action_ActivateTool(editor->GetRasterEraserTool());
        return;
    }

    if (CanAction_ActivateTool(editor->GetVectorEraserTool()))
    {
        Action_ActivateTool(editor->GetVectorEraserTool());
        return;
    }
}

void
FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivateWarpTool()
{
    TSharedPtr<FOdysseyPainterEditor> editor = mEditor.Pin();
    if (!editor)
        return;

    /* TODO:
    if (CanAction_ActivateTool(editor->GetRasterWarpTool()))
    {
        Action_ActivateTool(editor->GetRasterWarpTool());
        return;
    }
    */

    if (CanAction_ActivateTool(editor->GetVectorGridTool()))
    {
        Action_ActivateTool(editor->GetVectorGridTool());
        return;
    }
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

bool
FOdysseyPainterEditorGlobalToolsShortcuts::CanAction_ActivateColorPickerTool()
{
    TSharedPtr<FOdysseyPainterEditor> editor = mEditor.Pin();
    if (!editor)
        return false;

    if (CanAction_ActivateTool(editor->GetColorPickerTool()))
        return true;

    return false;
}

bool
FOdysseyPainterEditorGlobalToolsShortcuts::CanAction_ActivateDrawingTool()
{
    TSharedPtr<FOdysseyPainterEditor> editor = mEditor.Pin();
    if (!editor)
        return false;

    if (CanAction_ActivateTool(editor->GetRasterDrawingTool()))
        return true;

    if (CanAction_ActivateTool(editor->GetVectorPathDrawingTool()))
        return true;

    return false;
}

bool
FOdysseyPainterEditorGlobalToolsShortcuts::CanAction_ActivateSelectionTool()
{
    TSharedPtr<FOdysseyPainterEditor> editor = mEditor.Pin();
    if (!editor)
        return false;

    /* TODO:
    if (CanAction_ActivateTool(editor->GetRasterSelectionTool()))
        return true;
    */

    if (CanAction_ActivateTool(editor->GetVectorSelectionTool()))
        return true;

    return false;
}

bool
FOdysseyPainterEditorGlobalToolsShortcuts::CanAction_ActivatePaintBucketTool()
{
    TSharedPtr<FOdysseyPainterEditor> editor = mEditor.Pin();
    if (!editor)
        return false;

    if (CanAction_ActivateTool(editor->GetRasterPaintBucketTool()))
        return true;

    if (CanAction_ActivateTool(editor->GetVectorPaintBucketTool()))
        return true;

    return false;
}

bool
FOdysseyPainterEditorGlobalToolsShortcuts::CanAction_ActivateTransformTool()
{
    TSharedPtr<FOdysseyPainterEditor> editor = mEditor.Pin();
    if (!editor)
        return false;

    if (CanAction_ActivateTool(editor->GetRasterTransformTool()))
        return true;

    if (CanAction_ActivateTool(editor->GetVectorTransformTool()))
        return true;

    return false;
}

bool
FOdysseyPainterEditorGlobalToolsShortcuts::CanAction_ActivatePrimitiveDrawingTool()
{
    TSharedPtr<FOdysseyPainterEditor> editor = mEditor.Pin();
    if (!editor)
        return false;

    if (CanAction_ActivateTool(editor->GetRasterPrimitiveDrawingTool()))
        return true;

    if (CanAction_ActivateTool(editor->GetVectorPrimitiveDrawingTool()))
        return true;

    return false;
}

bool
FOdysseyPainterEditorGlobalToolsShortcuts::CanAction_ActivateEraserTool()
{
    TSharedPtr<FOdysseyPainterEditor> editor = mEditor.Pin();
    if (!editor)
        return false;

    if (CanAction_ActivateTool(editor->GetRasterEraserTool()))
        return true;

    if (CanAction_ActivateTool(editor->GetVectorEraserTool()))
        return true;

    return false;
}

bool
FOdysseyPainterEditorGlobalToolsShortcuts::CanAction_ActivateWarpTool()
{
    TSharedPtr<FOdysseyPainterEditor> editor = mEditor.Pin();
    if (!editor)
        return false;

    /* TODO:
    if (CanAction_ActivateTool(editor->GetRasterWarpTool()))
        return true;
    */

    if (CanAction_ActivateTool(editor->GetVectorGridTool()))
        return true;

    return false;
}

#undef LOCTEXT_NAMESPACE