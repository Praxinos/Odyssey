// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "PainterEditor/OdysseyPainterEditorFunctionLibrary.h"

#include "OdysseyBrushAssetBase.h"
#include "PainterEditor/OdysseyPainterEditorState.h"

//---

namespace
{
static
FOdysseyPainterEditorState*
GetState( UOdysseyBrushAssetBase* BrushContext )
{
    if( !BrushContext )
        return nullptr;

    FOdysseyDrawingState* istate = BrushContext->FindState( FOdysseyPainterEditorState::GetId() );
    if( !istate )
        return nullptr;

    FOdysseyPainterEditorState* state = static_cast<FOdysseyPainterEditorState*>( istate );
    check( state );
    if( !state )
        return nullptr;

    //---

    return state;
}
}

//static
float
UOdysseyPainterEditorFunctionLibrary::GetViewportZoom( UOdysseyBrushAssetBase* iBrushContext )
{
    if( !iBrushContext )
        return 100.f;

    //---

    FOdysseyPainterEditorState* state = GetState( iBrushContext );
    if( !state )
        return 100.f;

    //---

    return state->Zoom();
}

//static
float
UOdysseyPainterEditorFunctionLibrary::GetViewportRotation( UOdysseyBrushAssetBase* iBrushContext )
{
    if( !iBrushContext )
        return 0.f;

    //---

    FOdysseyPainterEditorState* state = GetState( iBrushContext );
    if( !state )
        return 0.f;

    //---

    return state->RotationInDegrees();
}

//static
FVector2D
UOdysseyPainterEditorFunctionLibrary::GetViewportPan( UOdysseyBrushAssetBase* iBrushContext )
{
    if( !iBrushContext )
        return FVector2D::ZeroVector;

    //---

    FOdysseyPainterEditorState* state = GetState( iBrushContext );
    if( !state )
        return FVector2D::ZeroVector;

    //---

    return state->Pan();
}
