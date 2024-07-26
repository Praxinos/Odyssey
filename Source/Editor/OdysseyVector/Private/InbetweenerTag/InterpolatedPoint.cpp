#include "InbetweenerTag/InterpolatedPoint.h"
#include "OdysseyVectorTagInbetweener.h"
#include "OdysseyVectorPoint.h"

FInterpolatedPoint::~FInterpolatedPoint()
{
}

FInterpolatedPoint::FInterpolatedPoint( FOdysseyVectorPoint* iPoint, uint32 iIndex  )
    : mOriginalPoint( iPoint )
    , mIndex ( iIndex )
    , mMappedQuadIndex ( 0 )
    , mU ( 0.0f )
    , mV ( 0.0f )
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
