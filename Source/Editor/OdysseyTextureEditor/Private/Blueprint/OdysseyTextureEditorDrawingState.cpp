// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyTextureEditorDrawingState.h"

#include "OdysseyLayerStack.h"
#include "OdysseyTextureEditor.h"

//---

FOdysseyTextureEditorDrawingState::FOdysseyTextureEditorDrawingState( FOdysseyTextureEditor* iEditor )
    : FOdysseyDrawingState()
    , mEditor( iEditor )
{
}

//static
const FName&
FOdysseyTextureEditorDrawingState::GetId()
{
    static FName id( "FOdysseyTextureEditorDrawingState" );
    return id;
}

const FName&
FOdysseyTextureEditorDrawingState::Id()
{
    return GetId();
}

FOdysseyLayerStack*
FOdysseyTextureEditorDrawingState::LayerStack()
{
    return mEditor->LayerStack();
}
