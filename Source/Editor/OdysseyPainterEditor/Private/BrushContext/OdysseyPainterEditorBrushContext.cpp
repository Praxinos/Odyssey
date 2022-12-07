// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "BrushContext/OdysseyPainterEditorBrushContext.h"

#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorViewportTab.h"
#include "SOdysseySurfaceViewport.h"

#include "Tools/RasterDrawingTool/OdysseyRasterDrawingTool.h"
#include "FreehandShape/OdysseyFreehandShape.h"

//---

FOdysseyPainterEditorBrushContext::~FOdysseyPainterEditorBrushContext()
{}

FOdysseyPainterEditorBrushContext::FOdysseyPainterEditorBrushContext( FOdysseyPainterEditor* iEditor )
    : FOdysseyBrushContext( "FOdysseyPainterEditorBrushContext" )
    , mEditor( iEditor )
{
}

//---

FOdysseyPainterEditor*
FOdysseyPainterEditorBrushContext::Editor()
{
    return mEditor;
}

float
FOdysseyPainterEditorBrushContext::Zoom()
{
    if (!mEditor->GetGUI()->GetViewportTab())
        return 1.0f;
    return mEditor->GetGUI()->GetViewportTab()->GetViewport()->GetZoom();
}

float
FOdysseyPainterEditorBrushContext::Rotation()
{
    if (!mEditor->GetGUI()->GetViewportTab())
        return 0.0f;
    return mEditor->GetGUI()->GetViewportTab()->GetViewport()->GetGuiRotationValue();
}

FVector2D
FOdysseyPainterEditorBrushContext::Pan()
{
    if (!mEditor->GetGUI()->GetViewportTab())
        return FVector2D(0.0f, 0.0f);
    return mEditor->GetGUI()->GetViewportTab()->GetViewport()->GetPan();
}

float
FOdysseyPainterEditorBrushContext::GetStep()
{
    UOdysseyRasterDrawingTool* drawingTool = Cast<UOdysseyRasterDrawingTool>(mEditor->GetSelectedTool());
    if (!drawingTool)
        return 0.f;

    UOdysseyFreehandShape* freehandShape = Cast<UOdysseyFreehandShape>(drawingTool->GetSelectedShapeInstance());
    if (!freehandShape)
        return 0.f;

    return  freehandShape->GetStep();
}
