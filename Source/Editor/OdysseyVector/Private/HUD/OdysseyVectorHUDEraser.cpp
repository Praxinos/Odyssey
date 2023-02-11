#include "HUD/OdysseyVectorHUDEraser.h"
#include "OdysseyVectorEngine.h"

FOdysseyVectorHUDEraser::~FOdysseyVectorHUDEraser()
{
}

FOdysseyVectorHUDEraser::FOdysseyVectorHUDEraser()
    : mBlending( false )
{
}

void
FOdysseyVectorHUDEraser::Draw( UOdysseyVectorRoot& iScene, ::ULIS::FRectD& iRoi, uint64 iFlags )
{
    BLContext* blctx = iScene.GetEngine()->GetBLContext();
    ::ULIS::FRectD bbox = { 0, 0, 0, 0 };
    BLPath path;
    BLPoint topLeft = { 0, 0 };

    if( mBlending == true )
    {
        blctx->blitImage( topLeft, *iScene.GetEngine()->GetBLMask() );
    }

    // matrix might get altered for displaying the selection rectangle of a single object. Save it.
    blctx->save();
    blctx->resetMatrix();
    blctx->setStrokeStyle(BLRgba32(0xFF0000FF));
    blctx->setStrokeWidth(1.0f);
    blctx->strokeCircle( mX, mY, mRadius );
    blctx->restore();
}

void
FOdysseyVectorHUDEraser::SetPosition( double iX, double iY )
{
    mX = iX;
    mY = iY;
}

void
FOdysseyVectorHUDEraser::SetRadius( double iRadius )
{
    mRadius = iRadius;
}

void
FOdysseyVectorHUDEraser::BlendMask( bool iBlending )
{
    mBlending = iBlending;
}
