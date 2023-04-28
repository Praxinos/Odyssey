#include "OdysseyVectorBucket.h"

#define BUCKETRADIUS   10
#define CROSSRADIUS     6
#define HANDLERADIUS    8
#define HANDLEDISTANCE 40.0f
#define PELLETRADIUS    4

#ifndef M_PI
#define M_PI 3.14159265358979323846L
#endif

FOdysseyVectorBucket::~FOdysseyVectorBucket()
{
}

static void
MakeRects( double iWorldX
         , double iWorldY
         , ::ULIS::FRectD& oBucketRect
         , ::ULIS::FRectD& oCrossRect
         , ::ULIS::FRectD& oPropagateRect )
{
    oBucketRect.x = iWorldX - BUCKETRADIUS;
    oBucketRect.y = iWorldY - BUCKETRADIUS;
    oBucketRect.w = BUCKETRADIUS * 2;
    oBucketRect.h = BUCKETRADIUS * 2;

    oCrossRect.x = oBucketRect.x + oBucketRect.w;
    oCrossRect.y = oBucketRect.y + oBucketRect.h;
    oCrossRect.w = CROSSRADIUS * 2;
    oCrossRect.h = CROSSRADIUS * 2;

    oPropagateRect.x = oBucketRect.x;
    oPropagateRect.y = oBucketRect.y + oBucketRect.h;
    oPropagateRect.w = CROSSRADIUS * 2;
    oPropagateRect.h = CROSSRADIUS * 2;
}

FOdysseyVectorBucket::FOdysseyVectorBucket( FOdysseyVectorObject& iParent, double iX, double iY, bool iPropagated )
    : mParent ( iParent )
    , mCtrlPoint ( this )
    , mIsGradient ( false )
    , mPropagated( iPropagated )
{
    mCtrlPoint.Set( HANDLEDISTANCE, 0.0f );

    SetCoords( iX, iY );
    SetColor( 128, 128, 128, 255 );
}

void
FOdysseyVectorBucket::SetPropagated( bool iPropagated )
{
    mPropagated = iPropagated;

    //mParent.Invalidate();
}

bool
FOdysseyVectorBucket::IsPropagated()
{
    return mPropagated;
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
FOdysseyVectorBucket::SetGradientColor0( uint8 iR0, uint8 iG0, uint8 iB0, uint8 iA0 )
{
    mGradientColor0.R = iR0;
    mGradientColor0.G = iG0;
    mGradientColor0.B = iB0;
    mGradientColor0.A = iA0;
}

void
FOdysseyVectorBucket::SetGradientColor1( uint8 iR1, uint8 iG1, uint8 iB1, uint8 iA1 )
{
    mGradientColor1.R = iR1;
    mGradientColor1.G = iG1;
    mGradientColor1.B = iB1;
    mGradientColor1.A = iA1;
}

void
FOdysseyVectorBucket::SetGradientColor0( FColor& iColor )
{
    mGradientColor0 = iColor;
}

void
FOdysseyVectorBucket::SetGradientColor1( FColor& iColor )
{
    mGradientColor1 = iColor; 
}

void
FOdysseyVectorBucket::SetColor( uint8 iR, uint8 iG, uint8 iB, uint8 iA )
{
    mColor.R = iR;
    mColor.G = iG;
    mColor.B = iB;
    mColor.A = iA;
}

void
FOdysseyVectorBucket::SetColor( FColor& iColor )
{
    mColor = iColor;
}

FColor&
FOdysseyVectorBucket::GetColor()
{
    return mColor;
}

void
FOdysseyVectorBucket::Draw( FBucketDrawingFlags iDrawingFlags )
{
    if( iDrawingFlags & FBucketDrawingFlags::BUCKET )
    {
        DrawBucket( iDrawingFlags );
    }

    if( iDrawingFlags & FBucketDrawingFlags::PELLET )
    {
        DrawPellet( iDrawingFlags );
    }
}

void
FOdysseyVectorBucket::DrawPellet( FBucketDrawingFlags iDrawingFlags )
{
    BLContext* blctx = mParent.GetScene()->GetEngine()->GetBLContext();
    BLPoint bucketWorldCoord = mParent.GetWorldMatrix().mapPoint( mCoords.x, mCoords.y );
    BLRgba32 fillColor;

    // Note: Blend2D color format is 0xAARRGGBB
    fillColor.setR( mColor.B );
    fillColor.setG( mColor.G );
    fillColor.setB( mColor.R );
    fillColor.setA( mColor.A );

    blctx->save();
    blctx->resetMatrix();

    // Bucket
    blctx->setFillStyle( fillColor );
    blctx->fillCircle( bucketWorldCoord.x, bucketWorldCoord.y, PELLETRADIUS );

    blctx->setStrokeWidth( 1.0f );
    blctx->setStrokeStyle( mPropagated ? BLRgba32( 0xFF00FF00 ) : BLRgba32( 0xFF000000 ) );
    blctx->strokeCircle( bucketWorldCoord.x, bucketWorldCoord.y, PELLETRADIUS );

    blctx->restore();
}

void
FOdysseyVectorBucket::DrawBucket( FBucketDrawingFlags iDrawingFlags )
{
    BLContext* blctx = mParent.GetScene()->GetEngine()->GetBLContext();
    BLPoint bucketWorldCoord = mParent.GetWorldMatrix().mapPoint( mCoords.x, mCoords.y );
    BLPoint handleWorldCoord = mParent.GetWorldMatrix().mapPoint( mCoords.x + mCtrlPoint.GetX()
                                                                , mCoords.y + mCtrlPoint.GetY() );
    ::ULIS::FRectD bucketRect;
    ::ULIS::FRectD crossRect;
    ::ULIS::FRectD propagateRect;
    BLRgba32 fillColor;

    // TODO: store rectangles as class members.
    MakeRects( bucketWorldCoord.x, bucketWorldCoord.y, bucketRect, crossRect, propagateRect );

    // Note: Blend2D color format is 0xAARRGGBB
    fillColor.setR( mColor.B );
    fillColor.setG( mColor.G );
    fillColor.setB( mColor.R );
    fillColor.setA( mColor.A );

    blctx->save();
    blctx->resetMatrix();

    // Bucket
    blctx->setFillStyle( fillColor );
    blctx->fillRect( bucketRect.x, bucketRect.y, bucketRect.w, bucketRect.h );

    blctx->setStrokeWidth( 1.0f );
    blctx->setStrokeStyle( BLRgba32(0xFF000000) );
    blctx->strokeRect( bucketRect.x, bucketRect.y, bucketRect.w, bucketRect.h );

    //Cross
    blctx->setFillStyle( BLRgba32( 0xFF808080 ) );
    blctx->fillRect( crossRect.x, crossRect.y, crossRect.w, crossRect.h );

    blctx->setStrokeWidth( 1.0f );
    blctx->setStrokeStyle( BLRgba32( 0xFF000000 ) );
    blctx->strokeRect( crossRect.x, crossRect.y, crossRect.w, crossRect.h );

    blctx->setStrokeWidth( 1.0f );
    blctx->setStrokeStyle( BLRgba32( 0xFF000000 ) );
    blctx->strokeLine( crossRect.x              , crossRect.y, crossRect.x + crossRect.h, crossRect.y + crossRect.h );
    blctx->strokeLine( crossRect.x + crossRect.w, crossRect.y, crossRect.x              , crossRect.y + crossRect.h );

    // propagated
    blctx->setFillStyle( mPropagated ? BLRgba32( 0xFF00FF00 ) : BLRgba32( 0xFF808080 ) );
    blctx->fillRect( propagateRect.x, propagateRect.y, propagateRect.w, propagateRect.h );

    blctx->setStrokeWidth( 1.0f );
    blctx->setStrokeStyle( BLRgba32( 0xFF000000 ) );
    blctx->strokeRect( propagateRect.x, propagateRect.y, propagateRect.w, propagateRect.h );

    // Handle
    blctx->setFillStyle( BLRgba32( 0xFF808080 ) );
    blctx->fillCircle( handleWorldCoord.x, handleWorldCoord.y, HANDLERADIUS );
    blctx->setStrokeStyle( BLRgba32( 0xFF000000 ) );
    blctx->strokeLine( bucketWorldCoord.x, bucketWorldCoord.y, handleWorldCoord.x, handleWorldCoord.y );
    blctx->strokeCircle( handleWorldCoord.x, handleWorldCoord.y, HANDLERADIUS );

    blctx->restore();
}

void
FOdysseyVectorBucket::Copy( FOdysseyVectorBucket* iDestinationBucket )
{
    iDestinationBucket->mCoords = mCoords;
    iDestinationBucket->mColor = mColor;

    iDestinationBucket->mIsGradient = mIsGradient;
    iDestinationBucket->mGradientColor0 = mGradientColor0;
    iDestinationBucket->mGradientColor1 = mGradientColor1;
}

uint32
FOdysseyVectorBucket::Pick( double iWorldX, double iWorldY )
{
    BLPoint bucketWorldCoord = mParent.GetWorldMatrix().mapPoint( mCoords.x, mCoords.y );
    ::ULIS::FVec2D pt = ::ULIS::FVec2D( iWorldX, iWorldY );
    ::ULIS::FRectD bucketRect;
    ::ULIS::FRectD crossRect;
    ::ULIS::FRectD propagateRect;

    MakeRects( bucketWorldCoord.x, bucketWorldCoord.y, bucketRect, crossRect, propagateRect );

    if( bucketRect.HitTest( pt ) )
    {
        return FOdysseyVectorBucket::PICKBUCKET;
    }

    if( crossRect.HitTest( pt ) )
    {
        return FOdysseyVectorBucket::PICKCROSS;
    }

    if( propagateRect.HitTest( pt ) )
    {
        return FOdysseyVectorBucket::PICKPROPAGATED;
    }

    if ( PickHandle( iWorldX, iWorldY ) )
    {
        return FOdysseyVectorBucket::PICKHANDLE;
    }

    return FOdysseyVectorBucket::PICKNONE;

}

FOdysseyVectorHandleBucket*
FOdysseyVectorBucket::PickHandle( double iWorldX, double iWorldY )
{
    BLPoint bucketWorldCoord = mParent.GetWorldMatrix().mapPoint( mCoords.x, mCoords.y );
    BLPoint handleWorldCoord = mParent.GetWorldMatrix().mapPoint( mCoords.x + mCtrlPoint.GetX()
                                                                , mCoords.y + mCtrlPoint.GetY() );
    double difX = iWorldX - handleWorldCoord.x;
    double difY = iWorldY - handleWorldCoord.y;
    double distance = sqrt( ( difX * difX ) + ( difY * difY ) );

    if( distance < HANDLERADIUS )
    {
        return &mCtrlPoint;
    }

    return nullptr;
}

FOdysseyVectorHandleBucket*
FOdysseyVectorBucket::GetHandle()
{
    return &mCtrlPoint;
}

double
FOdysseyVectorBucket::GetHandleDotProduct()
{
    ::ULIS::FVec2D controllerVec( mCtrlPoint.GetX(), mCtrlPoint.GetY() );
    ::ULIS::FVec2D horizontalVec( 1.0f, 0.0f );

    if( controllerVec.DistanceSquared() )
    {
        controllerVec.Normalize();

        return controllerVec.DotProduct( horizontalVec );
    }

    return 0.0f;
}

double
FOdysseyVectorBucket::GetGradientRotationInDegrees()
{
    double dot = GetHandleDotProduct();

    return acos(dot) / M_PI * 180.0f;
}

FOdysseyVectorObject&
FOdysseyVectorBucket::GetParent()
{
    return mParent;
}
