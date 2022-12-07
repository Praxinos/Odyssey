#include "OdysseyVectorCircle.h"

// https://stackoverflow.com/a/27863181
// https://stackoverflow.com/questions/1734745/how-to-create-circle-with-b%c3%a9zier-curves

#define MAGICRATIO 0.55191502449 // = 4*(sqrt(2)-1)/3

FOdysseyVectorCircle::~FOdysseyVectorCircle()
{
}

FOdysseyVectorCircle::FOdysseyVectorCircle( std::string iName )
    : FOdysseyVectorPathCubic(iName)
{
    mRadiusX = mRadiusY = 0.0f;

    mCubicPoint[0] = new FOdysseyVectorPointCubic( 0.0f, 0.0f );
    mCubicPoint[1] = new FOdysseyVectorPointCubic( 0.0f, 0.0f );
    mCubicPoint[2] = new FOdysseyVectorPointCubic( 0.0f, 0.0f );
    mCubicPoint[3] = new FOdysseyVectorPointCubic( 0.0f, 0.0f );

    mCubicSegment[0] = new FOdysseyVectorSegmentCubic( static_cast<FOdysseyVectorPathCubic&>(*this), mCubicPoint[0], mCubicPoint[1] );
    mCubicSegment[1] = new FOdysseyVectorSegmentCubic( static_cast<FOdysseyVectorPathCubic&>(*this), mCubicPoint[1], mCubicPoint[2] );
    mCubicSegment[2] = new FOdysseyVectorSegmentCubic( static_cast<FOdysseyVectorPathCubic&>(*this), mCubicPoint[2], mCubicPoint[3] );
    mCubicSegment[3] = new FOdysseyVectorSegmentCubic( static_cast<FOdysseyVectorPathCubic&>(*this), mCubicPoint[3], mCubicPoint[0] );

    AddPoint ( mCubicPoint[0] );
    AddPoint ( mCubicPoint[1] );
    AddPoint ( mCubicPoint[2] );
    AddPoint ( mCubicPoint[3] );

    AddSegment ( mCubicSegment[0] );
    AddSegment ( mCubicSegment[1] );
    AddSegment ( mCubicSegment[2] );
    AddSegment ( mCubicSegment[3] );
}

FOdysseyVectorCircle::FOdysseyVectorCircle( std::string iName, double iRadius )
    : FOdysseyVectorCircle(iName)
{
    SetRadius ( iRadius, iRadius );
}

FOdysseyVectorCircle::FOdysseyVectorCircle( std::string iName, double iRadiusX, double iRadiusY )
    : FOdysseyVectorCircle(iName)
{
    SetRadius ( iRadiusX, iRadiusY );
}

void
FOdysseyVectorCircle::UpdateShape()
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

FOdysseyVectorObject*
FOdysseyVectorCircle::CopyShape()
{
    FOdysseyVectorCircle* circleCopy = new FOdysseyVectorCircle ( mName, mRadiusX, mRadiusY );

    return static_cast<FOdysseyVectorObject*>( circleCopy );
}

FOdysseyVectorPathCubic*
FOdysseyVectorCircle::Convert()
{
    FOdysseyVectorPathCubic* path = static_cast<FOdysseyVectorPathCubic*>(this->FOdysseyVectorPathCubic::CopyShape());

    this->CopySettings( *path );

    return path;
}

void
FOdysseyVectorCircle::DrawShape( ::ULIS::FRectD &iRoi, uint64 iFlags )
{
    if ( mRadiusX && mRadiusY )
    {
        FOdysseyVectorPathCubic::DrawShape ( iRoi, iFlags );
    }
}

FOdysseyVectorObject*
FOdysseyVectorCircle::PickShape( double iX, double iY, double iRadius )
{
    if( FMath::Sqrt((iX*iX) + (iY*iY)) <= mRadiusX )
    {
        return this;
    }

    return nullptr;
}

void
FOdysseyVectorCircle::SetRadius( double iRadius )
{
    SetRadius( iRadius, iRadius );
}

void
FOdysseyVectorCircle::SetRadius( double iRadiusX, double iRadiusY )
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
FOdysseyVectorCircle::GetRadiusX()
{
    return mRadiusX;
}

double
FOdysseyVectorCircle::GetRadiusY()
{
    return mRadiusY;
}
