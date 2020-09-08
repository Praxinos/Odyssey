// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

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
