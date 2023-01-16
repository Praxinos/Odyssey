#include "HUD/OdysseyVectorHUDLine.h"
#include "OdysseyVectorEngine.h"

FOdysseyVectorHUDLine::~FOdysseyVectorHUDLine()
{
}

FOdysseyVectorHUDLine::FOdysseyVectorHUDLine()
{
}

void
FOdysseyVectorHUDLine::SetP0( double x, double y )
{
    mP0.x = x;
    mP0.y = y;
}

void
FOdysseyVectorHUDLine::SetP1( double x, double y )
{
    mP1.x = x;
    mP1.y = y;
}

::ULIS::FVec2D&
FOdysseyVectorHUDLine::GetP0()
{
    return mP0;
}

::ULIS::FVec2D&
FOdysseyVectorHUDLine::GetP1()
{
    return mP1;
}

void
FOdysseyVectorHUDLine::Draw( UOdysseyVectorObject* iObject, ::ULIS::FRectD& iRoi, uint64 iFlags )
{
    BLContext* blctx = iObject->GetRoot()->GetEngine()->GetBLContext();

    blctx->setStrokeStyle( BLRgba32(0xFF0000FF) );
    blctx->setStrokeWidth( 1.0f );
    blctx->strokeLine( mP0.x, mP0.y, mP1.x, mP1.y );
}
