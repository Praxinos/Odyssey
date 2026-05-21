// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Widgets/Tab/SOdysseyPainterEditorTools.h"

#include "OdysseyPainterEditor.h"
#include "Widgets/Tools/SOdysseyPainterEditorToolsTileView.h"
#include "Widgets/Tools/SOdysseyPainterEditorToolOptions.h"

#include "OdysseyPainterEditorRasterDrawingTool.h"
#include "Tools/RasterEraserTool/OdysseyPainterEditorRasterEraserTool.h"
#include "Tools/RasterSelectionTool/OdysseyPainterEditorRasterSelectionTool.h"
#include "Tools/RasterTransformTool/OdysseyPainterEditorRasterTransformTool.h"
#include "Tools/RasterPrimitiveDrawingTool/OdysseyPainterEditorRasterPrimitiveDrawingTool.h"
#include "Tools/RasterLiquifyTool/OdysseyPainterEditorRasterLiquifyTool.h"
#include "Tools/RasterPaintBucketTool/OdysseyPainterEditorRasterPaintBucketTool.h"
#include "Tools/VectorPrimitiveDrawingTool/OdysseyPainterEditorVectorPrimitiveDrawingTool.h"
#include "Tools/VectorPathDrawingTool/OdysseyPainterEditorVectorPathDrawingTool.h"
#include "Tools/VectorPathEditTool/OdysseyPainterEditorVectorPathEditTool.h"
#include "Tools/VectorSelectionTool/OdysseyPainterEditorVectorSelectionTool.h"
#include "Tools/VectorCutTool/OdysseyPainterEditorVectorCutTool.h"
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

SOdysseyPainterEditorTools::~SOdysseyPainterEditorTools()
{}

SOdysseyPainterEditorTools::SOdysseyPainterEditorTools()
{}

void
SOdysseyPainterEditorTools::Construct(const FArguments& InArgs)
{
    mEditor = InArgs._Editor;

    FOdysseyPainterEditor* editor = mEditor.Get();

    if (!editor)
        return;

    TArray<UOdysseyPainterEditorTool*> tools = {

        // Drawing Tools
        editor->GetRasterDrawingTool(),
        editor->GetVectorPathDrawingTool(),

        // Primitive Tools
        editor->GetRasterPrimitiveDrawingTool(),
        editor->GetVectorPrimitiveDrawingTool(),

        // Eraser Tools
        editor->GetRasterEraserTool(),
        editor->GetVectorEraserTool(),

        // Color Picker Tools
        editor->GetColorPickerTool(),

        // Paint Bucket Tools
        editor->GetRasterPaintBucketTool(),
        editor->GetVectorPaintBucketTool(),

        // Selection Tools
        editor->GetRasterSelectionTool(),
        editor->GetVectorSelectionTool(),

        // Transform Tools
        editor->GetRasterTransformTool(),
        editor->GetVectorTransformTool(),

        // Raster Specific Tools
        editor->GetRasterLiquifyTool(),

        //Vector Specific Tools
        //Vector Transformation Tools
        editor->GetVectorScenePanTool(),
        editor->GetVectorGridTool(),
        editor->GetVectorPathPushTool(),

        //Vetcor Path editing tools
        editor->GetVectorPathEditTool(),
        editor->GetVectorPathSmoothTool(),
        editor->GetVectorCutTool(),
        editor->GetVectorPathStitchTool(),

        //Out Of Pegs Tools
        editor->GetVectorMatchingTool(),
        editor->GetVectorChartTool(),
        editor->GetVectorTrajectoryTool()
    };

    tools = tools.FilterByPredicate([](UOdysseyPainterEditorTool* iTool){return !!iTool;});

    ChildSlot
    [
        SNew(SHorizontalBox)
        + SHorizontalBox::Slot()
        [
            SNew(SScrollBox)
            + SScrollBox::Slot()
            .FillSize(1.0f)
            [
                SNew(SOdysseyPainterEditorToolOptions)
                .Tool_Raw(this, &SOdysseyPainterEditorTools::GetCurrentTool)
            ]
        ]

        + SHorizontalBox::Slot()
        .AutoWidth()
        [
            SNew(SScrollBox)
            + SScrollBox::Slot()
            .AutoSize()
            [
                SNew( SOdysseyPainterEditorToolsTileView )
                .Tools(tools)
                .OnToolSelected(this, &SOdysseyPainterEditorTools::OnToolSelected)
            ]
        ]
    ];
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Widget Getters

UOdysseyPainterEditorTool*
SOdysseyPainterEditorTools::GetCurrentTool() const
{

    FOdysseyPainterEditor* editor = mEditor.Get();

    if (!editor)
        return nullptr;

    return editor->GetCurrentTool();
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Event Listeners

void
SOdysseyPainterEditorTools::OnToolSelected(UOdysseyPainterEditorTool* iTool)
{
    FOdysseyPainterEditor* editor = mEditor.Get();

    if (!editor)
        return;

    editor->ActivateMainTool(iTool);
}
