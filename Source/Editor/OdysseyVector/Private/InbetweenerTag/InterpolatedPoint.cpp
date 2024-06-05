#include "InbetweenerTag/InterpolatedPoint.h"
#include "OdysseyVectorTagInbetweener.h"
#include "OdysseyVectorPoint.h"

FInterpolatedPoint::~FInterpolatedPoint()
{
}

FInterpolatedPoint::FInterpolatedPoint( FOdysseyVectorPoint* iPoint
                                      , uint32 iIndex
                                      , double iU
                                      , double iV  )
    : mOriginalPoint( iPoint )
    , mIndex ( iIndex )
    , mU( iU )
    , mV( iV )
{
}
