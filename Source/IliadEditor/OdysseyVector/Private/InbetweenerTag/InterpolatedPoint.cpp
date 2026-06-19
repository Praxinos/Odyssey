// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "InbetweenerTag/InterpolatedPoint.h"
#include "OdysseyVectorTagInbetweener.h"
#include "OdysseyVectorPoint.h"

FInterpolatedPoint::~FInterpolatedPoint()
{
}

FInterpolatedPoint::FInterpolatedPoint( FOdysseyVectorPoint* iPoint, double iRadius, uint32 iIndex  )
    : mMappedQuadIndex ( 0 )
    , mOriginalPoint( iPoint )
    , mRadius ( iRadius )
    , mIndex ( iIndex )
    , mU ( 0.0f )
    , mV ( 0.0f )
    , mOriginalCoords( iPoint->GetCoords() )
{
}

void
FInterpolatedPoint::SetU( double iU )
{
    mU = iU;
}

void
FInterpolatedPoint::SetV( double iV )
{
    mV = iV;
}

double
FInterpolatedPoint::GetRadius()
{
    return mRadius;
}

double
FInterpolatedPoint::GetU()
{
    return mU;
}

double
FInterpolatedPoint::GetV()
{
    return mV;
}

uint32 FInterpolatedPoint::GetMappedQuadIndex()
{
    return mMappedQuadIndex;
}

::ULIS::FVec2D&
FInterpolatedPoint::GetOriginalCoords()
{
    return mOriginalCoords;
}

FOdysseyVectorPoint*
FInterpolatedPoint::GetOriginalPoint()
{
    return mOriginalPoint;
}

void
FInterpolatedPoint::SetUV( uint32 iMappedQuadIndex, double iU, double iV )
{
    mMappedQuadIndex = iMappedQuadIndex;
    mU = iU;
    mV = iV;
}

uint32
FInterpolatedPoint::GetIndex()
{
    return mIndex;
}
