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

FVector2D
IOdysseyHUDViewportElement::ToViewport(const FVector2D& iPoint) const
{
    FVector2D inverse(1.f, -1.f);
    //FVector2D center = FVector2D(iViewport->GetSizeXY().X / 2.0f, iViewport->GetSizeXY().Y / 2.0f);
    FVector2D pos = iPoint * inverse;

    if (mTransform)
        return (mTransform->TransformPoint(pos) * inverse);
    else
        return (pos * inverse);
}