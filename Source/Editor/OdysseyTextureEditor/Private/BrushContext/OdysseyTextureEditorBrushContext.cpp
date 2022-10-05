// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "BrushContext/OdysseyTextureEditorBrushContext.h"

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
