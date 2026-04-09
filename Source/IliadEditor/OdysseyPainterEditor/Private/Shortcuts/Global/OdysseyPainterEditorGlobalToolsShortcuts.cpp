// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Shortcuts/Global/OdysseyPainterEditorGlobalToolsShortcuts.h"
#include "OdysseyPainterEditorCommands.h"
#include "OdysseyPainterEditor.h"
#include "Tools/OdysseyPainterEditorTool.h"

#include "OdysseyPainterEditorRasterDrawingTool.h"
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
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivateTool, Cast<UOdysseyPainterEditorTool>(mEditor->GetRasterDrawingTool())),
        FCanExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::CanAction_ActivateTool, Cast<UOdysseyPainterEditorTool>(mEditor->GetRasterDrawingTool()))
    );
    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().ActivateRasterEraserTool,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivateTool, Cast<UOdysseyPainterEditorTool>(mEditor->GetRasterEraserTool())),
        FCanExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::CanAction_ActivateTool, Cast<UOdysseyPainterEditorTool>(mEditor->GetRasterEraserTool()))
    );
    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().ActivateRasterPaintBucketTool,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivateTool, Cast<UOdysseyPainterEditorTool>(mEditor->GetRasterPaintBucketTool())),
        FCanExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::CanAction_ActivateTool, Cast<UOdysseyPainterEditorTool>(mEditor->GetRasterPaintBucketTool()))
    );
    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().ActivateRasterPrimitiveDrawingTool,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivateTool, Cast<UOdysseyPainterEditorTool>(mEditor->GetRasterPrimitiveDrawingTool())),
        FCanExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::CanAction_ActivateTool, Cast<UOdysseyPainterEditorTool>(mEditor->GetRasterPrimitiveDrawingTool()))
    );
    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().ActivateRasterTransformTool,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivateTool, Cast<UOdysseyPainterEditorTool>(mEditor->GetRasterSelectionTool())),
        FCanExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::CanAction_ActivateTool, Cast<UOdysseyPainterEditorTool>(mEditor->GetRasterSelectionTool()))
    );
    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().ActivateRasterTransformTool,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivateTool, Cast<UOdysseyPainterEditorTool>(mEditor->GetRasterTransformTool())),
        FCanExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::CanAction_ActivateTool, Cast<UOdysseyPainterEditorTool>(mEditor->GetRasterTransformTool()))
    );
    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().ActivateVectorEraserTool,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivateTool, Cast<UOdysseyPainterEditorTool>(mEditor->GetVectorEraserTool())),
        FCanExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::CanAction_ActivateTool, Cast<UOdysseyPainterEditorTool>(mEditor->GetVectorEraserTool()))
    );
    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().ActivateVectorGridTool,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivateTool, Cast<UOdysseyPainterEditorTool>(mEditor->GetVectorGridTool())),
        FCanExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::CanAction_ActivateTool, Cast<UOdysseyPainterEditorTool>(mEditor->GetVectorGridTool()))
    );
    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().ActivateVectorPaintBucketTool,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivateTool, Cast<UOdysseyPainterEditorTool>(mEditor->GetVectorPaintBucketTool())),
        FCanExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::CanAction_ActivateTool, Cast<UOdysseyPainterEditorTool>(mEditor->GetVectorPaintBucketTool()))
    );
    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().ActivateVectorPathDrawingTool,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivateTool, Cast<UOdysseyPainterEditorTool>(mEditor->GetVectorPathDrawingTool())),
        FCanExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::CanAction_ActivateTool, Cast<UOdysseyPainterEditorTool>(mEditor->GetVectorPathDrawingTool()))
    );
    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().ActivateVectorPathEditTool,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivateTool, Cast<UOdysseyPainterEditorTool>(mEditor->GetVectorPathEditTool())),
        FCanExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::CanAction_ActivateTool, Cast<UOdysseyPainterEditorTool>(mEditor->GetVectorPathEditTool()))
    );
    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().ActivateVectorPathPushTool,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivateTool, Cast<UOdysseyPainterEditorTool>(mEditor->GetVectorPathPushTool())),
        FCanExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::CanAction_ActivateTool, Cast<UOdysseyPainterEditorTool>(mEditor->GetVectorPathPushTool()))
    );
    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().ActivateVectorPathSmoothTool,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivateTool, Cast<UOdysseyPainterEditorTool>(mEditor->GetVectorPathSmoothTool())),
        FCanExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::CanAction_ActivateTool, Cast<UOdysseyPainterEditorTool>(mEditor->GetVectorPathSmoothTool()))
    );
    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().ActivateVectorPathStitchTool,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivateTool, Cast<UOdysseyPainterEditorTool>(mEditor->GetVectorPathStitchTool())),
        FCanExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::CanAction_ActivateTool, Cast<UOdysseyPainterEditorTool>(mEditor->GetVectorPathStitchTool()))
    );
    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().ActivateVectorPrimitiveDrawingTool,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivateTool, Cast<UOdysseyPainterEditorTool>(mEditor->GetVectorPrimitiveDrawingTool())),
        FCanExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::CanAction_ActivateTool, Cast<UOdysseyPainterEditorTool>(mEditor->GetVectorPrimitiveDrawingTool()))
    );
    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().ActivateVectorScenePanTool,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivateTool, Cast<UOdysseyPainterEditorTool>(mEditor->GetVectorScenePanTool())),
        FCanExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::CanAction_ActivateTool, Cast<UOdysseyPainterEditorTool>(mEditor->GetVectorScenePanTool()))
    );
    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().ActivateVectorSelectionTool,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivateTool, Cast<UOdysseyPainterEditorTool>(mEditor->GetVectorSelectionTool())),
        FCanExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::CanAction_ActivateTool, Cast<UOdysseyPainterEditorTool>(mEditor->GetVectorSelectionTool()))
    );
    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().ActivateVectorTransformTool,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivateTool, Cast<UOdysseyPainterEditorTool>(mEditor->GetVectorTransformTool())),
        FCanExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::CanAction_ActivateTool, Cast<UOdysseyPainterEditorTool>(mEditor->GetVectorTransformTool()))
    );
    iCommandList->MapAction(
        FOdysseyPainterEditorCommands::Get().SetToolRadius,
        FExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::Action_SetToolRadius),
        FCanExecuteAction::CreateRaw(this, &FOdysseyPainterEditorGlobalToolsShortcuts::CanAction_SetToolRadius)
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
FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivateColorPickerTool()
{
    if (!mEditor)
        return;

    if (CanAction_ActivateTool(mEditor->GetColorPickerTool()))
    {
        Action_ActivateTool(mEditor->GetColorPickerTool());
        return;
    }
}

void
FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivateDrawingTool()
{
    if (!mEditor)
        return;

    if (CanAction_ActivateTool(mEditor->GetRasterDrawingTool()))
    {
        Action_ActivateTool(mEditor->GetRasterDrawingTool());
        return;
    }

    if (CanAction_ActivateTool(mEditor->GetVectorPathDrawingTool()))
    {
        Action_ActivateTool(mEditor->GetVectorPathDrawingTool());
        return;
    }
}

void
FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivateSelectionTool()
{
    if (!mEditor)
        return;

    if (CanAction_ActivateTool(mEditor->GetRasterSelectionTool()))
    {
        Action_ActivateTool(mEditor->GetRasterSelectionTool());
        return;
    }

    if (CanAction_ActivateTool(mEditor->GetVectorSelectionTool()))
    {
        Action_ActivateTool(mEditor->GetVectorSelectionTool());
        return;
    }
}

void
FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivatePaintBucketTool()
{
    if (!mEditor)
        return;

    if (CanAction_ActivateTool(mEditor->GetRasterPaintBucketTool()))
    {
        Action_ActivateTool(mEditor->GetRasterPaintBucketTool());
        return;
    }

    if (CanAction_ActivateTool(mEditor->GetVectorPaintBucketTool()))
    {
        Action_ActivateTool(mEditor->GetVectorPaintBucketTool());
        return;
    }
}

void
FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivateTransformTool()
{
    if (!mEditor)
        return;

    if (CanAction_ActivateTool(mEditor->GetRasterTransformTool()))
    {
        Action_ActivateTool(mEditor->GetRasterTransformTool());
        return;
    }

    if (CanAction_ActivateTool(mEditor->GetVectorTransformTool()))
    {
        Action_ActivateTool(mEditor->GetVectorTransformTool());
        return;
    }
}

void
FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivatePrimitiveDrawingTool()
{
    if (!mEditor)
        return;

    if (CanAction_ActivateTool(mEditor->GetRasterPrimitiveDrawingTool()))
    {
        Action_ActivateTool(mEditor->GetRasterPrimitiveDrawingTool());
        return;
    }

    if (CanAction_ActivateTool(mEditor->GetVectorPrimitiveDrawingTool()))
    {
        Action_ActivateTool(mEditor->GetVectorPrimitiveDrawingTool());
        return;
    }
}

void
FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivateEraserTool()
{
    if (!mEditor)
        return;

    if (CanAction_ActivateTool(mEditor->GetRasterEraserTool()))
    {
        Action_ActivateTool(mEditor->GetRasterEraserTool());
        return;
    }

    if (CanAction_ActivateTool(mEditor->GetVectorEraserTool()))
    {
        Action_ActivateTool(mEditor->GetVectorEraserTool());
        return;
    }
}

void
FOdysseyPainterEditorGlobalToolsShortcuts::Action_ActivateWarpTool()
{
    if (!mEditor)
        return;

    /* TODO:
    if (CanAction_ActivateTool(mEditor->GetRasterWarpTool()))
    {
        Action_ActivateTool(mEditor->GetRasterWarpTool());
        return;
    }
    */

    if (CanAction_ActivateTool(mEditor->GetVectorGridTool()))
    {
        Action_ActivateTool(mEditor->GetVectorGridTool());
        return;
    }
}

void
FOdysseyPainterEditorGlobalToolsShortcuts::Action_SetToolRadius()
{
    if (!mEditor)
        return;

    UOdysseyPainterEditorTool* tool = mEditor->GetCurrentTool();
    if (!tool)
        return;

    tool->StartRadiusInteractiveModifier();
}


bool
FOdysseyPainterEditorGlobalToolsShortcuts::CanAction_ActivateTool(UOdysseyPainterEditorTool* iTool)
{
    if (!mEditor)
        return false;

    if (!iTool->IsActivable() || iTool->IsActivated() )
        return false;

    return true;
}

bool
FOdysseyPainterEditorGlobalToolsShortcuts::CanAction_ActivateColorPickerTool()
{
    if (!mEditor)
        return false;

    if (CanAction_ActivateTool(mEditor->GetColorPickerTool()))
        return true;

    return false;
}

bool
FOdysseyPainterEditorGlobalToolsShortcuts::CanAction_ActivateDrawingTool()
{
    if (!mEditor)
        return false;

    if (CanAction_ActivateTool(mEditor->GetRasterDrawingTool()))
        return true;

    if (CanAction_ActivateTool(mEditor->GetVectorPathDrawingTool()))
        return true;

    return false;
}

bool
FOdysseyPainterEditorGlobalToolsShortcuts::CanAction_ActivateSelectionTool()
{
    if (!mEditor)
        return false;

    if (CanAction_ActivateTool(mEditor->GetRasterSelectionTool()))
        return true;

    if (CanAction_ActivateTool(mEditor->GetVectorSelectionTool()))
        return true;

    return false;
}

bool
FOdysseyPainterEditorGlobalToolsShortcuts::CanAction_ActivatePaintBucketTool()
{
    if (!mEditor)
        return false;

    if (CanAction_ActivateTool(mEditor->GetRasterPaintBucketTool()))
        return true;

    if (CanAction_ActivateTool(mEditor->GetVectorPaintBucketTool()))
        return true;

    return false;
}

bool
FOdysseyPainterEditorGlobalToolsShortcuts::CanAction_ActivateTransformTool()
{
    if (!mEditor)
        return false;

    if (CanAction_ActivateTool(mEditor->GetRasterTransformTool()))
        return true;

    if (CanAction_ActivateTool(mEditor->GetVectorTransformTool()))
        return true;

    return false;
}

bool
FOdysseyPainterEditorGlobalToolsShortcuts::CanAction_ActivatePrimitiveDrawingTool()
{
    if (!mEditor)
        return false;

    if (CanAction_ActivateTool(mEditor->GetRasterPrimitiveDrawingTool()))
        return true;

    if (CanAction_ActivateTool(mEditor->GetVectorPrimitiveDrawingTool()))
        return true;

    return false;
}

bool
FOdysseyPainterEditorGlobalToolsShortcuts::CanAction_ActivateEraserTool()
{
    if (!mEditor)
        return false;

    if (CanAction_ActivateTool(mEditor->GetRasterEraserTool()))
        return true;

    if (CanAction_ActivateTool(mEditor->GetVectorEraserTool()))
        return true;

    return false;
}

bool
FOdysseyPainterEditorGlobalToolsShortcuts::CanAction_ActivateWarpTool()
{
    if (!mEditor)
        return false;

    /* TODO:
    if (CanAction_ActivateTool(mEditor->GetRasterWarpTool()))
        return true;
    */

    if (CanAction_ActivateTool(mEditor->GetVectorGridTool()))
        return true;

    return false;
}

bool
FOdysseyPainterEditorGlobalToolsShortcuts::CanAction_SetToolRadius()
{
    if (!mEditor)
        return false;

    UOdysseyPainterEditorTool* tool = mEditor->GetCurrentTool();
    if (!tool)
        return false;

    if (!tool->HasRadius())
        return false;

    return true;
}

#undef LOCTEXT_NAMESPACE
