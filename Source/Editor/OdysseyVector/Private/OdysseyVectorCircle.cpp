#include "OdysseyVectorCircle.h"

// https://stackoverflow.com/a/27863181
// https://stackoverflow.com/questions/1734745/how-to-create-circle-with-b%c3%a9zier-curves

#define MAGICRATIO 0.55191502449 // = 4*(sqrt(2)-1)/3

UOdysseyVectorCircle::~UOdysseyVectorCircle()
{
}

UOdysseyVectorCircle::UOdysseyVectorCircle()
{
    SetName( "Circle" );

    mRadiusX = mRadiusY = 0.0f;

    mCubicPoint[0] = UOdysseyVectorVertexCubic::New( 0.0f, 0.0f, 1.0f );
    mCubicPoint[1] = UOdysseyVectorVertexCubic::New( 0.0f, 0.0f, 1.0f );
    mCubicPoint[2] = UOdysseyVectorVertexCubic::New( 0.0f, 0.0f, 1.0f );
    mCubicPoint[3] = UOdysseyVectorVertexCubic::New( 0.0f, 0.0f, 1.0f );

    mCubicSegment[0] = UOdysseyVectorSegmentCubic::New( static_cast<UOdysseyVectorPathCubic*>(this), mCubicPoint[0], mCubicPoint[1] );
    mCubicSegment[1] = UOdysseyVectorSegmentCubic::New( static_cast<UOdysseyVectorPathCubic*>(this), mCubicPoint[1], mCubicPoint[2] );
    mCubicSegment[2] = UOdysseyVectorSegmentCubic::New( static_cast<UOdysseyVectorPathCubic*>(this), mCubicPoint[2], mCubicPoint[3] );
    mCubicSegment[3] = UOdysseyVectorSegmentCubic::New( static_cast<UOdysseyVectorPathCubic*>(this), mCubicPoint[3], mCubicPoint[0] );

    AddVertex ( mCubicPoint[0] );
    AddVertex ( mCubicPoint[1] );
    AddVertex ( mCubicPoint[2] );
    AddVertex ( mCubicPoint[3] );

    AddSegment ( mCubicSegment[0] );
    AddSegment ( mCubicSegment[1] );
    AddSegment ( mCubicSegment[2] );
    AddSegment ( mCubicSegment[3] );
}

void
UOdysseyVectorCircle::Init( std::string iName, double iRadius )
{
    SetName( iName );
    SetRadius( iRadius, iRadius );
}

void
UOdysseyVectorCircle::Init( std::string iName, double iRadiusX, double iRadiusY )
{
    SetName( iName );
    SetRadius( iRadiusX, iRadiusY );
}

void
UOdysseyVectorCircle::UpdateShape()
{
    double ctlDistX = mRadiusX * MAGICRATIO;
    double ctlDistY = mRadiusY * MAGICRATIO;

    mCubicPoint[0]->Set(  0.0f    ,  mRadiusY );
    mCubicPoint[1]->Set(  mRadiusX,  0.0f     );
    mCubicPoint[2]->Set(  0.0f    , -mRadiusY );
    mCubicPoint[3]->Set( -mRadiusX,  0.0f     );

    mCubicSegment[0]->GetControlPoint(0).Set(  ctlDistX,  mRadiusY );
    mCubicSegment[0]->GetControlPoint(1).Set(  mRadiusX,  ctlDistY );

    mCubicSegment[1]->GetControlPoint(0).Set(  mRadiusX, -ctlDistY );
    mCubicSegment[1]->GetControlPoint(1).Set(  ctlDistX, -mRadiusY );

    mCubicSegment[2]->GetControlPoint(0).Set( -ctlDistX, -mRadiusY );
    mCubicSegment[2]->GetControlPoint(1).Set( -mRadiusX, -ctlDistY );

    mCubicSegment[3]->GetControlPoint(0).Set( -mRadiusX,  ctlDistY );
    mCubicSegment[3]->GetControlPoint(1).Set( -ctlDistX,  mRadiusY );

    mCubicSegment[0]->Update();
    mCubicSegment[1]->Update();
    mCubicSegment[2]->Update();
    mCubicSegment[3]->Update();
}

UOdysseyVectorObject*
UOdysseyVectorCircle::CopyShape()
{
    UOdysseyVectorCircle* circleCopy = NewObject<UOdysseyVectorCircle>();

    circleCopy->Init ( mName, mRadiusX, mRadiusY );

    return Cast<UOdysseyVectorObject>( circleCopy );
}

UOdysseyVectorPathCubic*
UOdysseyVectorCircle::Convert()
{
    UOdysseyVectorPathCubic* path = static_cast<UOdysseyVectorPathCubic*>(this->UOdysseyVectorPathCubic::CopyShape());

    this->CopySettings( *path );

    return path;
}

void
UOdysseyVectorCircle::DrawShape( ::ULIS::FRectD &iRoi, uint64 iFlags )
{
    if ( mRadiusX && mRadiusY )
    {
        UOdysseyVectorPathCubic::DrawShape ( iRoi, iFlags );
    }
}

UOdysseyVectorObject*
UOdysseyVectorCircle::PickShape( double iX, double iY, double iRadius )
{
    if( FMath::Sqrt((iX*iX) + (iY*iY)) <= mRadiusX )
    {
        return this;
    }

    return nullptr;
}

void
UOdysseyVectorCircle::SetRadius( double iRadius )
{
    SetRadius( iRadius, iRadius );
}

void
UOdysseyVectorCircle::SetRadius( double iRadiusX, double iRadiusY )
{
    mRadiusX = iRadiusX;
    mRadiusY = iRadiusY;

    mBBox.x = -mRadiusX - mStrokeWidth;
    mBBox.y = -mRadiusY - mStrokeWidth;
    mBBox.w =  ( mRadiusX +  mStrokeWidth ) * 2;
    mBBox.h =  ( mRadiusY +  mStrokeWidth ) * 2;

    UpdateShape();
}

double
UOdysseyVectorCircle::GetRadiusX()
{
    return mRadiusX;
}

double
UOdysseyVectorCircle::GetRadiusY()
{
    return mRadiusY;
}
