// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyPainterEditorFunctionLibrary.h"

#include "OdysseyBrushAssetBase.h"
#include "OdysseyPainterEditorDrawingState.h"

//---

namespace
{
static
FOdysseyPainterEditorDrawingState*
GetState( UOdysseyBrushAssetBase* BrushContext )
{
    if( !BrushContext )
        return nullptr;

    FOdysseyDrawingState* istate = BrushContext->FindState(FOdysseyPainterEditorDrawingState::GetId() );
    if( !istate )
        return nullptr;

    FOdysseyPainterEditorDrawingState* state = static_cast<FOdysseyPainterEditorDrawingState*>( istate );
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

    FOdysseyPainterEditorDrawingState* state = GetState( iBrushContext );
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

    FOdysseyPainterEditorDrawingState* state = GetState( iBrushContext );
    if( !state )
        return 0.f;

    //---

    return state->Rotation();
}

//static
FVector2D
UOdysseyPainterEditorFunctionLibrary::GetViewportPan( UOdysseyBrushAssetBase* iBrushContext )
{
    if( !iBrushContext )
        return FVector2D::ZeroVector;

    //---

    FOdysseyPainterEditorDrawingState* state = GetState( iBrushContext );
    if( !state )
        return FVector2D::ZeroVector;

    //---

    return state->Pan();
}
