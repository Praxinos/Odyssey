// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "OdysseyLayerStackEditorBrushContext.h"

//---

FOdysseyLayerStackEditorBrushContext::~FOdysseyLayerStackEditorBrushContext()
{}

FOdysseyLayerStackEditorBrushContext::FOdysseyLayerStackEditorBrushContext( UOdysseyLayerStack* iLayerStack )
    : FOdysseyBrushContext( "FOdysseyLayerStackEditorBrushContext" )
    , mLayerStack( iLayerStack )
{
}

//---

UOdysseyLayerStack*
FOdysseyLayerStackEditorBrushContext::GetLayerStack() const
{
    return mLayerStack;
}

void
FOdysseyLayerStackEditorBrushContext::SetLayerStack(UOdysseyLayerStack* iLayerStack)
{
    mLayerStack = iLayerStack;
}
