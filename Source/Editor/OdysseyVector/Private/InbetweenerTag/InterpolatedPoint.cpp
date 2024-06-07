#include "InbetweenerTag/InterpolatedPoint.h"
#include "OdysseyVectorTagInbetweener.h"
#include "OdysseyVectorPoint.h"

FInterpolatedPoint::~FInterpolatedPoint()
{
}

FInterpolatedPoint::FInterpolatedPoint( FOdysseyVectorPoint* iPoint, uint32 iIndex  )
    : mOriginalPoint( iPoint )
    , mIndex ( iIndex )
    , mMappedQuad ( nullptr )
    , mU ( 0.0f )
    , mV ( 0.0f )
{
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

FInbetweenerQuad* FInterpolatedPoint::GetMappedQuad()
{
    return mMappedQuad;
}

FOdysseyVectorPoint*
FInterpolatedPoint::GetOriginalPoint()
{
    return mOriginalPoint;
}

void
FInterpolatedPoint::SetUV( FInbetweenerQuad* iMappedQuad, double iU, double iV )
{
    mMappedQuad = iMappedQuad;
    mU = iU;
    mV = iV;
}
