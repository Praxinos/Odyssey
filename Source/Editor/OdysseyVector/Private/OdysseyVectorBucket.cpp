#include "OdysseyVectorBucket.h"

FOdysseyVectorBucket::~FOdysseyVectorBucket()
{
}

FOdysseyVectorBucket::FOdysseyVectorBucket( UOdysseyVectorObject& iParent, uint32 iColor, double iX, double iY )
    : mParent ( iParent )
{
    SetColor( iColor );
    SetCoords( iX, iY );
}

::ULIS::FVec2D&
FOdysseyVectorBucket::GetCoords()
{
    return mCoords;
}

void
FOdysseyVectorBucket::SetCoords( double iX, double iY )
{
    mCoords.x = iX;
    mCoords.y = iY;
}

void
FOdysseyVectorBucket::SetColor( uint32 iColor )
{
    mColor = iColor;
}

uint32
FOdysseyVectorBucket::GetColor()
{
    return mColor;
}

void
FOdysseyVectorBucket::Draw( ::ULIS::FRectD& iRoi, uint64 iFlags )
{
    BLPoint worldPosition = mParent.GetWorldMatrix().mapPoint( mCoords.x, mCoords.y );
    BLContext* blctx = mParent.GetRoot()->GetEngine()->GetBLContext();

    blctx->save();
    blctx->setFillStyle(BLRgba32(mColor));
    blctx->fillRect( worldPosition.x, worldPosition.y, 20, 20 );

    blctx->setStrokeWidth( 1.0f );
    blctx->setStrokeStyle(BLRgba32(0xFF000000));
    blctx->strokeRect( worldPosition.x, worldPosition.y, 20, 20 );
    blctx->restore();
}
