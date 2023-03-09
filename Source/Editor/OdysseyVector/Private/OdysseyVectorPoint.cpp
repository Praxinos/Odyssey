#include "OdysseyVectorPoint.h"

FOdysseyVectorPoint::~FOdysseyVectorPoint()
{
}

FOdysseyVectorPoint::FOdysseyVectorPoint()
{
    Set( 0.0f,0.0f );
    SetRadius( 1.0f );
}

void
FOdysseyVectorPoint::Init( double iX, double iY, double iRadius )
{
    Set( iX, iY );
    SetRadius( iRadius );
}

//static
FOdysseyVectorPoint*
FOdysseyVectorPoint::New( double iX, double iY, double iRadius )
{
    FOdysseyVectorPoint* point = new FOdysseyVectorPoint();

    point->Init ( iX, iY, iRadius );

    return point;
}

double 
FOdysseyVectorPoint::GetX()
{
    return mCoords.x;
}

double 
FOdysseyVectorPoint::GetY()
{
    return mCoords.y;
}

void 
FOdysseyVectorPoint::SetX( double iX )
{
    mCoords.x  = iX;
}

void 
FOdysseyVectorPoint::SetY( double iY )
{
    mCoords.y = iY;
}

void 
FOdysseyVectorPoint::Set( double iX, double iY )
{
    SetX( iX );
    SetY( iY );
}

double
FOdysseyVectorPoint::GetRadius()
{
    return mRadius;
}

void
FOdysseyVectorPoint::SetRadius( double iRadius )
{
    mRadius = iRadius;
}

void
FOdysseyVectorPoint::SetID( uint32 iID )
{
    mID = iID;
}

uint32
FOdysseyVectorPoint::GetID()
{
    return mID;
}

