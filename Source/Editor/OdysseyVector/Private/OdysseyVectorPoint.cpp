#include "OdysseyVectorPoint.h"

UOdysseyVectorPoint::~UOdysseyVectorPoint()
{
}

UOdysseyVectorPoint::UOdysseyVectorPoint()
{
    Set( 0.0f,0.0f );
    SetRadius( 1.0f );
}

void
UOdysseyVectorPoint::Init( double iX, double iY, double iRadius )
{
    Set( iX, iY );
    SetRadius( iRadius );
}

//static
UOdysseyVectorPoint*
UOdysseyVectorPoint::New( double iX, double iY, double iRadius )
{
    UOdysseyVectorPoint* point = NewObject<UOdysseyVectorPoint>();

    point->Init ( iX, iY, iRadius );

    return point;
}

::ULIS::FVec2D&
UOdysseyVectorPoint::GetCoords()
{
    return mCoords;
}

double 
UOdysseyVectorPoint::GetX()
{
    return mCoords.x;
}

double 
UOdysseyVectorPoint::GetY()
{
    return mCoords.y;
}

void 
UOdysseyVectorPoint::SetX( double iX )
{
    mCoords.x  = iX;
}

void 
UOdysseyVectorPoint::SetY( double iY )
{
    mCoords.y = iY;
}

void 
UOdysseyVectorPoint::Set( double iX, double iY )
{
    SetX( iX );
    SetY( iY );
}

double
UOdysseyVectorPoint::GetRadius()
{
    return mRadius;
}

void
UOdysseyVectorPoint::SetRadius( double iRadius )
{
    mRadius = iRadius;
}

void
UOdysseyVectorPoint::SetID( uint32 iID )
{
    mID = iID;
}

uint32
UOdysseyVectorPoint::GetID()
{
    return mID;
}

