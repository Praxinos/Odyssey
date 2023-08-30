#include "OdysseyVectorEllipse.h"

// https://stackoverflow.com/a/27863181
// https://stackoverflow.com/questions/1734745/how-to-create-circle-with-b%c3%a9zier-curves

#define MAGICRATIO 0.55191502449 // = 4*(sqrt(2)-1)/3

FOdysseyVectorEllipse::~FOdysseyVectorEllipse()
{
   // vertices ans segments freed in OdysseyVectorPath::~destructor
}

FOdysseyVectorEllipse::FOdysseyVectorEllipse( const FString& iName
                                            , double iRadiusX
                                            , double iRadiusY
                                            , double iStrokeWidth )
    : FOdysseyVectorPrimitive( iName )
    , mStrokeWidth( iStrokeWidth )
    , mRadiusX( iRadiusX )
    , mRadiusY( iRadiusY )
{
    mCubicVertex[0] = new FOdysseyVectorVertex( this, 0.0f, 0.0f, mStrokeWidth );
    mCubicVertex[1] = new FOdysseyVectorVertex( this, 0.0f, 0.0f, mStrokeWidth );
    mCubicVertex[2] = new FOdysseyVectorVertex( this, 0.0f, 0.0f, mStrokeWidth );
    mCubicVertex[3] = new FOdysseyVectorVertex( this, 0.0f, 0.0f, mStrokeWidth );

    mCubicSegment[0] = new FOdysseyVectorSegmentCubic( this, mCubicVertex[0], mCubicVertex[1], true );
    mCubicSegment[1] = new FOdysseyVectorSegmentCubic( this, mCubicVertex[1], mCubicVertex[2], true );
    mCubicSegment[2] = new FOdysseyVectorSegmentCubic( this, mCubicVertex[2], mCubicVertex[3], true );
    mCubicSegment[3] = new FOdysseyVectorSegmentCubic( this, mCubicVertex[3], mCubicVertex[0], true );

    AddVertex ( mCubicVertex[0] );
    AddVertex ( mCubicVertex[1] );
    AddVertex ( mCubicVertex[2] );
    AddVertex ( mCubicVertex[3] );

    AddSegment ( mCubicSegment[0] );
    AddSegment ( mCubicSegment[1] );
    AddSegment ( mCubicSegment[2] );
    AddSegment ( mCubicSegment[3] );

    UpdateShape( 0 );
}

bool
FOdysseyVectorEllipse::HasBaseClass( uint32 iBaseClassID )
{
    if( mStaticClass == iBaseClassID )
    {
        return true;
    }

    return FOdysseyVectorPath::HasBaseClass( iBaseClassID );
}

void
FOdysseyVectorEllipse::UpdateShape( uint32 iUpdateFlags )
{
    double ctlDistX = mRadiusX * MAGICRATIO;
    double ctlDistY = mRadiusY * MAGICRATIO;

    mCubicVertex[0]->SetRadius( mStrokeWidth );
    mCubicVertex[1]->SetRadius( mStrokeWidth );
    mCubicVertex[2]->SetRadius( mStrokeWidth );
    mCubicVertex[3]->SetRadius( mStrokeWidth );

    mCubicVertex[0]->Set(  0.0f    ,  mRadiusY );
    mCubicVertex[1]->Set(  mRadiusX,  0.0f     );
    mCubicVertex[2]->Set(  0.0f    , -mRadiusY );
    mCubicVertex[3]->Set( -mRadiusX,  0.0f     );

    mCubicSegment[0]->GetHandle(0)->Set(  ctlDistX,  mRadiusY );
    mCubicSegment[0]->GetHandle(1)->Set(  mRadiusX,  ctlDistY );

    mCubicSegment[1]->GetHandle(0)->Set(  mRadiusX, -ctlDistY );
    mCubicSegment[1]->GetHandle(1)->Set(  ctlDistX, -mRadiusY );

    mCubicSegment[2]->GetHandle(0)->Set( -ctlDistX, -mRadiusY );
    mCubicSegment[2]->GetHandle(1)->Set( -mRadiusX, -ctlDistY );

    mCubicSegment[3]->GetHandle(0)->Set( -mRadiusX,  ctlDistY );
    mCubicSegment[3]->GetHandle(1)->Set( -ctlDistX,  mRadiusY );

    FOdysseyVectorPath::UpdateShape( iUpdateFlags );
/*
    mCubicSegment[0]->Update( nullptr );
    mCubicSegment[1]->Update( nullptr );
    mCubicSegment[2]->Update( nullptr );
    mCubicSegment[3]->Update( nullptr );

    // Update the bounding box
    mBBox.x = - mRadiusX - mStrokeWidth;
    mBBox.y = - mRadiusY - mStrokeWidth;
    mBBox.w =  ( mRadiusX +  mStrokeWidth ) * 2;
    mBBox.h =  ( mRadiusY +  mStrokeWidth ) * 2;
*/
}

FOdysseyVectorObject*
FOdysseyVectorEllipse::CopyShape()
{
    FOdysseyVectorEllipse* circleCopy = new FOdysseyVectorEllipse( mObjectParam.Name
                                                                 , mRadiusX
                                                                 , mRadiusY
                                                                 , mStrokeWidth );

    return static_cast<FOdysseyVectorObject*>( circleCopy );
}

void
FOdysseyVectorEllipse::DrawShape( uint64 iFlags )
{
    if ( mRadiusX && mRadiusY )
    {
        FOdysseyVectorPath::DrawShape ( iFlags );
    }
}

void
FOdysseyVectorEllipse::SetRadius( double iRadius )
{
    SetRadius( iRadius, iRadius );
}

void
FOdysseyVectorEllipse::SetRadius( double iRadiusX, double iRadiusY )
{
    mRadiusX = iRadiusX;
    mRadiusY = iRadiusY;

    Invalidate();
}

double
FOdysseyVectorEllipse::GetRadiusX()
{
    return mRadiusX;
}

double
FOdysseyVectorEllipse::GetRadiusY()
{
    return mRadiusY;
}

uint32
FOdysseyVectorEllipse::GetType()
{
    return FOdysseyVectorObject::VECTORELLIPSETYPE;
}
