// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyPainterEditorDrawingState.h"

#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorViewportTab.h"
#include "SOdysseySurfaceViewport.h"

//---

FOdysseyPainterEditorDrawingState::~FOdysseyPainterEditorDrawingState()
{}

FOdysseyPainterEditorDrawingState::FOdysseyPainterEditorDrawingState( FOdysseyPainterEditor* iEditor )
    : FOdysseyDrawingState()
    , mEditor( iEditor )
{
}

//static
const FName&
FOdysseyPainterEditorDrawingState::GetId()
{
    static FName id( "FOdysseyPainterEditorDrawingState" );
    return id;
}

const FName&
FOdysseyPainterEditorDrawingState::Id()
{
    return GetId();
}

//---

float
FOdysseyPainterEditorDrawingState::Zoom()
{
    if (!mEditor->GetGUI()->GetViewportTab())
        return 1.0f;
    return mEditor->GetGUI()->GetViewportTab()->GetViewport()->GetZoom();
}

float
FOdysseyPainterEditorDrawingState::RotationInDegrees()
{
    if (!mEditor->GetGUI()->GetViewportTab())
        return 0.0f;
    return mEditor->GetGUI()->GetViewportTab()->GetViewport()->GetRotationInDegrees();
}

FVector2D
FOdysseyPainterEditorDrawingState::Pan()
{
    if (!mEditor->GetGUI()->GetViewportTab())
        return FVector2D(0.0f, 0.0f);
    return mEditor->GetGUI()->GetViewportTab()->GetViewport()->GetPan();
}
