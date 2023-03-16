#include "HUD/OdysseyVectorHUDPush.h"
#include "OdysseyVectorEngine.h"

FOdysseyVectorHUDPush::~FOdysseyVectorHUDPush()
{
}

FOdysseyVectorHUDPush::FOdysseyVectorHUDPush()
{
}

void
FOdysseyVectorHUDPush::Draw( FOdysseyVectorScene& iScene, ::ULIS::FRectD& iRoi, uint64 iFlags )
{
    BLContext* blctx = iScene.GetEngine()->GetBLContext();

    // matrix might get altered for displaying the selection rectangle of a single object. Save it.
    blctx->save();
    blctx->resetMatrix();
    blctx->setStrokeStyle(BLRgba32(0xFF0000FF));
    blctx->setStrokeWidth(1.0f);
    blctx->strokeCircle( mX, mY, mRadius );
    blctx->restore();
}

void
FOdysseyVectorHUDPush::SetPosition( double iX, double iY )
{
    mX = iX;
    mY = iY;
}

void
FOdysseyVectorHUDPush::SetRadius( double iRadius )
{
    mRadius = iRadius;
}
