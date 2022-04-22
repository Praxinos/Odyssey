// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyTextureEditorBrushContext.h"

#include "OdysseyLayerStack.h"
#include "OdysseyTextureEditor.h"

//---

FOdysseyTextureEditorBrushContext::FOdysseyTextureEditorBrushContext( FOdysseyTextureEditor* iEditor )
    : FOdysseyBrushContext( "FOdysseyTextureEditorBrushContext" )
    , mEditor( iEditor )
{
}

FOdysseyLayerStack*
FOdysseyTextureEditorBrushContext::LayerStack()
{
    return mEditor->LayerStack();
}
