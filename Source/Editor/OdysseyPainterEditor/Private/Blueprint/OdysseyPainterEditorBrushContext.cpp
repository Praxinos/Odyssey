// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyPainterEditorBrushContext.h"

#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorViewportTab.h"
#include "SOdysseySurfaceViewport.h"

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
