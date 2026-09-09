// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Shortcuts/Global/OdysseyPainterEditorGlobalToolsShortcuts.h"
#include "OdysseyPainterEditorCommands.h"
#include "OdysseyPainterEditor.h"
#include "Tools/OdysseyPainterEditorTool.h"

#include "OdysseyPainterEditorRasterDrawingTool.h"
#include "Tools/OutOfPegsTool/OdysseyPainterEditorAnimationOutOfPegsTool.h"
#include "Tools/RasterEraserTool/OdysseyPainterEditorRasterEraserTool.h"
#include "Tools/RasterSelectionTool/OdysseyPainterEditorRasterSelectionTool.h"
#include "Tools/RasterTransformTool/OdysseyPainterEditorRasterTransformTool.h"
#include "Tools/RasterPrimitiveDrawingTool/OdysseyPainterEditorRasterPrimitiveDrawingTool.h"
#include "Tools/RasterPaintBucketTool/OdysseyPainterEditorRasterPaintBucketTool.h"
#include "Tools/VectorPrimitiveDrawingTool/OdysseyPainterEditorVectorPrimitiveDrawingTool.h"
#include "Tools/VectorPathDrawingTool/OdysseyPainterEditorVectorPathDrawingTool.h"
#include "Tools/VectorPathEditTool/OdysseyPainterEditorVectorPathEditTool.h"
#include "Tools/VectorSelectionTool/OdysseyPainterEditorVectorSelectionTool.h"
#include "Tools/VectorScenePanTool/OdysseyPainterEditorVectorScenePanTool.h"
#include "Tools/VectorEraserTool/OdysseyPainterEditorVectorEraserTool.h"
#include "Tools/VectorPathPushTool/OdysseyPainterEditorVectorPathPushTool.h"
#include "Tools/VectorPathSmoothTool/OdysseyPainterEditorVectorPathSmoothTool.h"
#include "Tools/VectorPathStitchTool/OdysseyPainterEditorVectorPathStitchTool.h"
#include "Tools/VectorPaintBucketTool/OdysseyPainterEditorVectorPaintBucketTool.h"
#include "Tools/ColorPickerTool/OdysseyPainterEditorColorPickerTool.h"
#include "Tools/VectorGridTool/OdysseyPainterEditorVectorGridTool.h"
#include "Tools/VectorTransformTool/OdysseyPainterEditorVectorTransformTool.h"
#include "Tools/VectorMatchingTool/OdysseyPainterEditorVectorMatchingTool.h"
#include "Tools/VectorChartTool/OdysseyPainterEditorVectorChartTool.h"
#include "Tools/VectorTrajectoryTool/OdysseyPainterEditorVectorTrajectoryTool.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

FOdysseyPainterEditorGlobalToolsShortcuts::FOdysseyPainterEditorGlobalToolsShortcuts(FOdysseyPainterEditor* iEditor)
    : mEditor(iEditor)
{
}

void
FOdysseyPainterEditorGlobalToolsShortcuts::MapActionsToCommandList(TSharedRef<FUICommandList> iCommandList)
{
    if (!mEditor)
        return;

    //Generic tool shortcuts

    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().ActivateColorPickerTool,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivateColorPickerTool)
    );
    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().ActivateDrawingTool,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivateDrawingTool)
    );
    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().ActivateSelectionTool,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivateSelectionTool)
    );
    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().ActivatePaintBucketTool,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivatePaintBucketTool)
    );
    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().ActivateTransformTool,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivateTransformTool)
    );
    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().ActivatePrimitiveDrawingTool,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivatePrimitiveDrawingTool)
    );
    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().ActivateEraserTool,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivateEraserTool)
    );
    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().ActivateWarpTool,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivateWarpTool)
    );

    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().ActivateTemporaryColorPickerTool,
        FExecuteAction::CreateRaw( this, &FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivateTemporaryColorPickerTool )
    );

    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().InactivateTemporaryTool,
        FExecuteAction::CreateRaw( this, &FOdysseyPainterEditorGlobalToolsShortcuts::Action_InactivateTemporaryTool )
    );

    //Specific tool shortcuts
    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().ActivateRasterDrawingTool,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivateTool, Cast<UOdysseyPainterEditorTool>(mEditor->GetRasterDrawingTool()))
    );
    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().ActivateRasterEraserTool,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivateTool, Cast<UOdysseyPainterEditorTool>(mEditor->GetRasterEraserTool()))
    );
    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().ActivateRasterPaintBucketTool,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivateTool, Cast<UOdysseyPainterEditorTool>(mEditor->GetRasterPaintBucketTool()))
    );
    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().ActivateRasterPrimitiveDrawingTool,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivateTool, Cast<UOdysseyPainterEditorTool>(mEditor->GetRasterPrimitiveDrawingTool()))
    );
    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().ActivateRasterTransformTool,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivateTool, Cast<UOdysseyPainterEditorTool>(mEditor->GetRasterSelectionTool()))
    );
    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().ActivateRasterTransformTool,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivateTool, Cast<UOdysseyPainterEditorTool>(mEditor->GetRasterTransformTool()))
    );
    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().ActivateVectorEraserTool,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivateTool, Cast<UOdysseyPainterEditorTool>(mEditor->GetVectorEraserTool()))
    );
    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().ActivateVectorGridTool,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivateTool, Cast<UOdysseyPainterEditorTool>(mEditor->GetVectorGridTool()))
    );
    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().ActivateVectorPaintBucketTool,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivateTool, Cast<UOdysseyPainterEditorTool>(mEditor->GetVectorPaintBucketTool()))
    );
    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().ActivateVectorPathDrawingTool,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivateTool, Cast<UOdysseyPainterEditorTool>(mEditor->GetVectorPathDrawingTool()))
    );
    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().ActivateVectorPathEditTool,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivateTool, Cast<UOdysseyPainterEditorTool>(mEditor->GetVectorPathEditTool()))
    );
    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().ActivateVectorPathPushTool,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivateTool, Cast<UOdysseyPainterEditorTool>(mEditor->GetVectorPathPushTool()))
    );
    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().ActivateVectorPathSmoothTool,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivateTool, Cast<UOdysseyPainterEditorTool>(mEditor->GetVectorPathSmoothTool()))
    );
    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().ActivateVectorPathStitchTool,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivateTool, Cast<UOdysseyPainterEditorTool>(mEditor->GetVectorPathStitchTool()))
    );
    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().ActivateVectorPrimitiveDrawingTool,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivateTool, Cast<UOdysseyPainterEditorTool>(mEditor->GetVectorPrimitiveDrawingTool()))
    );
    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().ActivateVectorScenePanTool,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivateTool, Cast<UOdysseyPainterEditorTool>(mEditor->GetVectorScenePanTool()))
    );
    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().ActivateVectorSelectionTool,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivateTool, Cast<UOdysseyPainterEditorTool>(mEditor->GetVectorSelectionTool()))
    );
    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().ActivateVectorTransformTool,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivateTool, Cast<UOdysseyPainterEditorTool>(mEditor->GetVectorTransformTool()))
    );
    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().SetToolRadius,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::Action_SetToolRadius)
    );
}

void
FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivateTool(UOdysseyPainterEditorTool* iTool)
{
    if (!mEditor)
        return;

    if (!iTool->IsActivable() || iTool->IsActivated() )
        return;

    mEditor->ActivateMainTool(iTool);
}

void
FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivateTemporaryTool( UOdysseyPainterEditorTool* iTool )
{
    if( !mEditor )
        return;

    if( !iTool->IsActivable() || iTool->IsActivated() )
        return;

    mEditor->ActivateTemporaryTool( iTool );
}

void
FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivateColorPickerTool()
{
    if (!mEditor)
        return;

    Action_ActivateTool(mEditor->GetColorPickerTool());
}

void
FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivateDrawingTool()
{
    if (!mEditor)
        return;

    Action_ActivateTool(mEditor->GetRasterDrawingTool());
    Action_ActivateTool(mEditor->GetVectorPathDrawingTool());
}

void
FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivateSelectionTool()
{
    if (!mEditor)
        return;

    Action_ActivateTool(mEditor->GetRasterSelectionTool());
    Action_ActivateTool(mEditor->GetVectorSelectionTool());
}

void
FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivatePaintBucketTool()
{
    if (!mEditor)
        return;

    Action_ActivateTool(mEditor->GetRasterPaintBucketTool());
    Action_ActivateTool(mEditor->GetVectorPaintBucketTool());
}

void
FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivateTransformTool()
{
    if (!mEditor)
        return;

    Action_ActivateTool(mEditor->GetRasterTransformTool());
    Action_ActivateTool(mEditor->GetVectorTransformTool());
}

void
FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivatePrimitiveDrawingTool()
{
    if (!mEditor)
        return;

    Action_ActivateTool(mEditor->GetRasterPrimitiveDrawingTool());
    Action_ActivateTool(mEditor->GetVectorPrimitiveDrawingTool());
}

void
FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivateEraserTool()
{
    if (!mEditor)
        return;

    Action_ActivateTool(mEditor->GetRasterEraserTool());
    Action_ActivateTool(mEditor->GetVectorEraserTool());
}

void
FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivateWarpTool()
{
    if (!mEditor)
        return;

    Action_ActivateTool(mEditor->GetVectorGridTool());
}

void
FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivateTemporaryColorPickerTool()
{
    if( !mEditor )
        return;

    Action_ActivateTemporaryTool( mEditor->GetTemporaryColorPickerTool() );
}

void
FOdysseyPainterEditorGlobalToolsShortcuts::Action_InactivateTemporaryTool()
{
    if (!mEditor)
        return;

    mEditor->InactivateTemporaryTool();
}

void
FOdysseyPainterEditorGlobalToolsShortcuts::Action_SetToolRadius()
{
    if (!mEditor)
        return;

    UOdysseyPainterEditorTool* tool = mEditor->GetCurrentTool();
    if (!tool)
        return;

    if (!tool->HasRadius())
        return;

    tool->StartRadiusInteractiveModifier();
}

#undef LOCTEXT_NAMESPACE
