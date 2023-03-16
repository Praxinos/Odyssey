// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "BrushContext/OdysseyViewportEditorBrushContext.h"

/*
#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorViewportTab.h"
#include "SOdysseySurfaceViewport.h"

#include "Tools/DrawingTool/OdysseyDrawingTool.h"
#include "FreehandShape/OdysseyFreehandShape.h"
*/

//---

FOdysseyViewportEditorBrushContext::~FOdysseyViewportEditorBrushContext()
{}

FOdysseyViewportEditorBrushContext::FOdysseyViewportEditorBrushContext()
    : FOdysseyBrushContext( "FOdysseyViewportEditorBrushContext" )
{
}

//---

float
FOdysseyViewportEditorBrushContext::GetStep()
{
    return 1.f;
}

float FOdysseyViewportEditorBrushContext::GetSize()
{
    //Todo: Size depending of the viewport position and mesh position
    return 5.f;
}
