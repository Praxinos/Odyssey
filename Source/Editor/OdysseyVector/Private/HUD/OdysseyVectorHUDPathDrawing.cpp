#include "HUD/OdysseyVectorHUDPathDrawing.h"
#include "OdysseyVectorEngine.h"

FOdysseyVectorHUDPathDrawing::~FOdysseyVectorHUDPathDrawing()
{
}

FOdysseyVectorHUDPathDrawing::FOdysseyVectorHUDPathDrawing()
    : mRadius( 10.0f )
    , mStitchingRadius( 0.0f )
{
}

void
FOdysseyVectorHUDPathDrawing::Reset(FOdysseyVectorScene* iScene)
{
}

void
FOdysseyVectorHUDPathDrawing::Draw( FOdysseyVectorScene* iScene, uint64 iFlags )
{
    BLContext* blctx = iScene->GetEngine()->GetBLContext();

    // matrix might get altered for displaying the selection rectangle of a single object. Save it.
    blctx->save();
    blctx->resetMatrix();

    blctx->setStrokeWidth( 1.0f );

    blctx->setStrokeStyle( BLRgba32( 0xFF0000FF ) );
    blctx->strokeCircle( mX, mY, mRadius );

    if( mStitchingRadius > 0.0f )
    {
        blctx->setStrokeStyle( BLRgba32( 0xFF00FF00 ) );
        blctx->strokeCircle( mX, mY, mStitchingRadius );
    }

    blctx->restore();
}

void
FOdysseyVectorHUDPathDrawing::SetPosition( double iX, double iY )
{
    mX = iX;
    mY = iY;
}

void
FOdysseyVectorHUDPathDrawing::SetRadius( double iRadius )
{
    mRadius = iRadius;
}

void
FOdysseyVectorHUDPathDrawing::SetStitchingRadius( double iStitchingRadius )
{
    mStitchingRadius = iStitchingRadius;
}
