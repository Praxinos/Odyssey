#include "OdysseyVectorBucket.h"

#define HANDLERADIUS 8
#define HANDLEDISTANCE 40.0f

FOdysseyVectorBucket::~FOdysseyVectorBucket()
{
}

void
FOdysseyVectorBucket::Reshape()
{
    bucketRect.x = mCoords.x - 10;
    bucketRect.y = mCoords.y - 10;
    bucketRect.w = 20;
    bucketRect.h = 20;

    crossRect.x = mCoords.x + 10;
    crossRect.y = mCoords.y + 10;
    crossRect.w = 10;
    crossRect.h = 10;
}

FOdysseyVectorBucket::FOdysseyVectorBucket( UOdysseyVectorObject& iParent, double iX, double iY )
    : mParent ( iParent )
    , mIsGradient ( false )
{
    mCtrlPoint = UOdysseyVectorHandleBucket::New( this );

    mCtrlPoint->Set( HANDLEDISTANCE, 0.0f );

    SetCoords( iX, iY );
    SetColor( 128, 128, 128, 255 );
}

bool
FOdysseyVectorBucket::IsGradient()
{
    return mIsGradient;
}

FColor&
FOdysseyVectorBucket::GetGradientColor0()
{
    return mGradientColor0;
}

FColor&
FOdysseyVectorBucket::GetGradientColor1()
{
    return mGradientColor1;
}

void
FOdysseyVectorBucket::SetGradient( bool iIsGradient )
{
    mIsGradient = iIsGradient;
}

void
FOdysseyVectorBucket::SetGradientColors( uint8 iR0, uint8 iG0, uint8 iB0, uint8 iA0
                                       , uint8 iR1, uint8 iG1, uint8 iB1, uint8 iA1 )
{
    mGradientColor0.R = iR0;
    mGradientColor0.G = iG0;
    mGradientColor0.B = iB0;
    mGradientColor0.A = iA0;

    mGradientColor1.R = iR1;
    mGradientColor1.G = iG1;
    mGradientColor1.B = iB1;
    mGradientColor1.A = iA1;
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

    Reshape();
}

void
FOdysseyVectorBucket::SetColor( uint8 iR, uint8 iG, uint8 iB, uint8 iA )
{
    mColor.R = iR;
    mColor.G = iG;
    mColor.B = iB;
    mColor.A = iA;
}

FColor&
FOdysseyVectorBucket::GetColor()
{
    return mColor;
}

void
FOdysseyVectorBucket::DrawCross( double iX, double iY, double iSize )
{
    BLContext* blctx = mParent.GetRoot()->GetEngine()->GetBLContext();

    blctx->setFillStyle( BLRgba32( 0xFF808080 ) );
    blctx->fillRect( iX, iY, 10, 10 );

    blctx->setStrokeWidth( 1.0f );
    blctx->setStrokeStyle( BLRgba32( 0xFF000000 ) );
    blctx->strokeRect( iX, iY, 10, 10 );

    blctx->setStrokeWidth( 1.0f );
    blctx->setStrokeStyle( BLRgba32( 0xFF000000 ) );
    blctx->strokeLine( iX        , iY, iX + iSize, iY + iSize );
    blctx->strokeLine( iX + iSize, iY, iX        , iY + iSize );
}

void
FOdysseyVectorBucket::Draw( ::ULIS::FRectD& iRoi, uint64 iFlags )
{
    BLContext* blctx = mParent.GetRoot()->GetEngine()->GetBLContext();
    BLPoint origin = mParent.GetWorldMatrix().mapPoint( mCoords.x, mCoords.y );
    BLRgba32 fillColor;

    // Note: Blend2D color format is 0xAARRGGBB
    fillColor.r = mColor.B;
    fillColor.g = mColor.G;
    fillColor.b = mColor.R;
    fillColor.a = mColor.A;

    blctx->save();

    blctx->resetMatrix();
    blctx->setFillStyle( fillColor );
    blctx->fillRect( origin.x - 10, origin.y - 10, 20, 20 );

    blctx->setStrokeWidth( 1.0f );
    blctx->setStrokeStyle( BLRgba32(0xFF000000) );
    blctx->strokeRect( origin.x - 10, origin.y - 10, 20, 20 );

    blctx->strokeLine( origin.x, origin.y, origin.x + mCtrlPoint->GetX(), origin.y + mCtrlPoint->GetY() );
    blctx->fillCircle( origin.x + mCtrlPoint->GetX(), origin.y + mCtrlPoint->GetY(), HANDLERADIUS );
    blctx->strokeCircle( origin.x + mCtrlPoint->GetX(), origin.y + mCtrlPoint->GetY(), HANDLERADIUS );

    DrawCross( origin.x + 10.0f, origin.y + 10.0f, 10.0f );

    blctx->restore();
}

uint32
FOdysseyVectorBucket::Pick( double iX, double iY )
{
    ::ULIS::FVec2D pt = ::ULIS::FVec2D( iX, iY );

    if( bucketRect.HitTest( pt ) )
    {
        return FOdysseyVectorBucket::PICKBUCKET;
    }

    if( crossRect.HitTest( pt ) )
    {
        return FOdysseyVectorBucket::PICKCROSS;
    }

    if ( PickHandle( iX, iY ) )
    {
        return FOdysseyVectorBucket::PICKHANDLE;
    }

    return FOdysseyVectorBucket::PICKNONE;

}

UOdysseyVectorHandleBucket*
FOdysseyVectorBucket::PickHandle( double iX, double iY )
{
    double handleX = mCoords.x + mCtrlPoint->GetX();
    double handleY = mCoords.y + mCtrlPoint->GetY();
    double difX = iX - handleX;
    double difY = iY - handleY;
    double distance = sqrt( ( difX * difX ) + ( difY * difY ) );

    if( distance < HANDLERADIUS )
    {
        return mCtrlPoint;
    }

    return nullptr;
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
