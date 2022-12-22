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
}

void
UOdysseyVectorCircle::Init( std::string iName, double iRadiusX, double iRadiusY )
{
    SetName( iName );
    SetRadius( iRadiusX, iRadiusY );

    mCubicVertex[0] = UOdysseyVectorVertexCubic::New( 0.0f, 0.0f, 1.0f );
    mCubicVertex[1] = UOdysseyVectorVertexCubic::New( 0.0f, 0.0f, 1.0f );
    mCubicVertex[2] = UOdysseyVectorVertexCubic::New( 0.0f, 0.0f, 1.0f );
    mCubicVertex[3] = UOdysseyVectorVertexCubic::New( 0.0f, 0.0f, 1.0f );

    mCubicSegment[0] = UOdysseyVectorSegmentCubic::New( static_cast<UOdysseyVectorPathCubic*>(this), mCubicVertex[0], mCubicVertex[1] );
    mCubicSegment[1] = UOdysseyVectorSegmentCubic::New( static_cast<UOdysseyVectorPathCubic*>(this), mCubicVertex[1], mCubicVertex[2] );
    mCubicSegment[2] = UOdysseyVectorSegmentCubic::New( static_cast<UOdysseyVectorPathCubic*>(this), mCubicVertex[2], mCubicVertex[3] );
    mCubicSegment[3] = UOdysseyVectorSegmentCubic::New( static_cast<UOdysseyVectorPathCubic*>(this), mCubicVertex[3], mCubicVertex[0] );

    AddVertex ( mCubicVertex[0] );
    AddVertex ( mCubicVertex[1] );
    AddVertex ( mCubicVertex[2] );
    AddVertex ( mCubicVertex[3] );

    AddSegment ( mCubicSegment[0] );
    AddSegment ( mCubicSegment[1] );
    AddSegment ( mCubicSegment[2] );
    AddSegment ( mCubicSegment[3] );
}

//static
UOdysseyVectorCircle*
UOdysseyVectorCircle::New( std::string iName, double iRadiusX, double iRadiusY )
{
    UOdysseyVectorCircle* circle = NewObject<UOdysseyVectorCircle>();

    circle->Init( iName, iRadiusX, iRadiusY );

    return circle;
}

void
UOdysseyVectorCircle::UpdateShape()
{
    double ctlDistX = mRadiusX * MAGICRATIO;
    double ctlDistY = mRadiusY * MAGICRATIO;

    mCubicVertex[0]->Set(  0.0f    ,  mRadiusY );
    mCubicVertex[1]->Set(  mRadiusX,  0.0f     );
    mCubicVertex[2]->Set(  0.0f    , -mRadiusY );
    mCubicVertex[3]->Set( -mRadiusX,  0.0f     );

    mCubicSegment[0]->GetControlPoint(0)->Set(  ctlDistX,  mRadiusY );
    mCubicSegment[0]->GetControlPoint(1)->Set(  mRadiusX,  ctlDistY );

    mCubicSegment[1]->GetControlPoint(0)->Set(  mRadiusX, -ctlDistY );
    mCubicSegment[1]->GetControlPoint(1)->Set(  ctlDistX, -mRadiusY );

    mCubicSegment[2]->GetControlPoint(0)->Set( -ctlDistX, -mRadiusY );
    mCubicSegment[2]->GetControlPoint(1)->Set( -mRadiusX, -ctlDistY );

    mCubicSegment[3]->GetControlPoint(0)->Set( -mRadiusX,  ctlDistY );
    mCubicSegment[3]->GetControlPoint(1)->Set( -ctlDistX,  mRadiusY );

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
