#include "OdysseyVectorBucket.h"

#define HANDLERADIUS 8
#define HANDLEDISTANCE 40.0f

FOdysseyVectorBucket::~FOdysseyVectorBucket()
{
}

FOdysseyVectorBucket::FOdysseyVectorBucket( UOdysseyVectorObject& iParent, uint32 iColor, double iX, double iY )
    : mParent ( iParent )
{
    mCtrlPoint = UOdysseyVectorHandleBucket::New( this );

    mCtrlPoint->Set( HANDLEDISTANCE, 0.0f );

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
    BLContext* blctx = mParent.GetRoot()->GetEngine()->GetBLContext();
    BLPoint origin = mParent.GetWorldMatrix().mapPoint( mCoords.x, mCoords.y );

    blctx->save();

    blctx->resetMatrix();
    blctx->setFillStyle(BLRgba32(mColor));
    blctx->fillRect( origin.x - 10, origin.y - 10, 20, 20 );


    blctx->setStrokeWidth( 1.0f );
    blctx->setStrokeStyle(BLRgba32(0xFF000000));
    blctx->strokeRect( origin.x - 10, origin.y - 10, 20, 20 );


    blctx->strokeLine( origin.x, origin.y, origin.x + mCtrlPoint->GetX(), origin.y + mCtrlPoint->GetY() );
    blctx->fillCircle( origin.x + mCtrlPoint->GetX(), origin.y + mCtrlPoint->GetY(), HANDLERADIUS );
    blctx->strokeCircle( origin.x + mCtrlPoint->GetX(), origin.y + mCtrlPoint->GetY(), HANDLERADIUS );

    blctx->restore();
}

bool
FOdysseyVectorBucket::PickHandle( double iX, double iY )
{
    double handleX = mCoords.x + mCtrlPoint->GetX();
    double handleY = mCoords.y + mCtrlPoint->GetY();
    double difX = iX - handleX;
    double difY = iY - handleY;
    double distance = sqrt( ( difX * difX ) + ( difY * difY ) );

    if( distance < HANDLERADIUS )
    {
        return true;
    }

    return false;
}

UOdysseyVectorHandleBucket*
FOdysseyVectorBucket::GetHandle()
{
    return mCtrlPoint;
}

double
FOdysseyVectorBucket::GetHandleDotProduct()
{
    ::ULIS::FVec2D controllerVec( mCtrlPoint->GetX(), mCtrlPoint->GetY() );
    ::ULIS::FVec2D horizontalVec( 1.0f, 0.0f );

    if( controllerVec.DistanceSquared() )
    {
        controllerVec.Normalize();

        return controllerVec.DotProduct( horizontalVec );
    }

    return 0.0f;
}

UOdysseyVectorObject&
FOdysseyVectorBucket::GetParent()
{
    return mParent;
}
