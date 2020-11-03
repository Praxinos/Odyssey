// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "PainterEditor/OdysseyPainterEditorState.h"

//---

FOdysseyPainterEditorState::FOdysseyPainterEditorState( float iZoom, float iRotationInDegrees, const FVector2D& iPan )
    : FOdysseyDrawingState()
    , mZoom( iZoom )
    , mRotation( iRotationInDegrees )
    , mPan( iPan )
{
}

//static
const FName&
FOdysseyPainterEditorState::GetId()
{
    static FName id( "FOdysseyPainterEditorState" );
    return id;
}

//---

float
FOdysseyPainterEditorState::Zoom()
{
    return mZoom;
}

float
FOdysseyPainterEditorState::RotationInDegrees()
{
    return mRotation;
}

const FVector2D&
FOdysseyPainterEditorState::Pan()
{
    return mPan;
}
