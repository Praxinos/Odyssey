// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "TextureEditor/OdysseyTextureEditorState.h"

#include "OdysseyLayerStack.h"

//---

FOdysseyTextureEditorState::FOdysseyTextureEditorState( FOdysseyLayerStack* iLayerStack )
    : FOdysseyDrawingState()
    , mLayerStack( iLayerStack )
{
}

//static
const FName&
FOdysseyTextureEditorState::GetId()
{
    static FName id( "FOdysseyTextureEditorState" );
    return id;
}

FOdysseyLayerStack*
FOdysseyTextureEditorState::LayerStack()
{
    return mLayerStack;
}
