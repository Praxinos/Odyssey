// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "OdysseyPainterEditorBrushContext.h"

#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorViewportTab.h"
#include "SOdysseyViewport.h"

#include "Tools/RasterDrawingTool/OdysseyPainterEditorRasterDrawingTool.h"
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
    TSharedPtr<FOdysseyPainterEditorViewportTab> viewportTab = mEditor->FindTab<FOdysseyPainterEditorViewportTab>();
    if (!viewportTab)
        return 1.0f;

    return viewportTab->GetViewport()->GetZoom();
}

float
FOdysseyPainterEditorBrushContext::Rotation()
{
    TSharedPtr<FOdysseyPainterEditorViewportTab> viewportTab = mEditor->FindTab<FOdysseyPainterEditorViewportTab>();
    if (!viewportTab)
        return 0.0f;
    return viewportTab->GetViewport()->GetGuiRotationValue();
}

FVector2D
FOdysseyPainterEditorBrushContext::Pan()
{
    TSharedPtr<FOdysseyPainterEditorViewportTab> viewportTab = mEditor->FindTab<FOdysseyPainterEditorViewportTab>();
    if (!viewportTab)
        return FVector2D(0.0f, 0.0f);
    return viewportTab->GetViewport()->GetPan();
}

float
FOdysseyPainterEditorBrushContext::GetStep()
{
    UOdysseyPainterEditorRasterDrawingTool* drawingTool = Cast<UOdysseyPainterEditorRasterDrawingTool>(mEditor->GetCurrentTool());
    if (!drawingTool)
        return 0.f;

    return drawingTool->Step;
}
