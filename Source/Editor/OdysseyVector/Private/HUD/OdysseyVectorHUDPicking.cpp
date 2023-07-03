#include "HUD/OdysseyVectorHUDPicking.h"
#include "OdysseyVectorEngine.h"

FOdysseyVectorHUDPicking::~FOdysseyVectorHUDPicking()
{
}

FOdysseyVectorHUDPicking::FOdysseyVectorHUDPicking()
{
}

void
FOdysseyVectorHUDPicking::Reset(FOdysseyVectorScene* iScene)
{
}

void
FOdysseyVectorHUDPicking::Draw( FOdysseyVectorScene* iScene, uint64 iFlags )
{
    BLContext* blctx = iScene->GetEngine()->GetBLContext();

    // matrix might get altered for displaying the selection rectangle of a single object. Save it.
    blctx->save();
    blctx->resetMatrix();
    blctx->setStrokeStyle(BLRgba32(0xFF0000FF));
    blctx->setStrokeWidth(1.0f);
    blctx->strokeCircle( mX, mY, mRadius );
    blctx->restore();
}

void
FOdysseyVectorHUDPicking::SetPosition( double iX, double iY )
{
    mX = iX;
    mY = iY;
}

void
FOdysseyVectorHUDPicking::SetRadius( double iRadius )
{
    mRadius = iRadius;
}
