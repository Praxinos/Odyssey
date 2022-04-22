// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "IOdysseyHUDViewportElement.h"

IOdysseyHUDViewportElement::IOdysseyHUDViewportElement()
{
    mPaintEngineHUD = nullptr;
}

IOdysseyHUDViewportElement::~IOdysseyHUDViewportElement()
{
}

void 
IOdysseyHUDViewportElement::SetPaintEngineHUD(FOdysseyPaintEngineHUD* iPaintEngineHUD)
{
    mPaintEngineHUD = iPaintEngineHUD;
}

FOdysseyPaintEngineHUD* 
IOdysseyHUDViewportElement::GetPaintEngineHUD()
{
    return mPaintEngineHUD;
}

void IOdysseyHUDViewportElement::SetTransform( FTransform2D iTransform)
{
    mTransform = iTransform;
}

FTransform2D IOdysseyHUDViewportElement::GetTransform()
{
    return mTransform;
}
