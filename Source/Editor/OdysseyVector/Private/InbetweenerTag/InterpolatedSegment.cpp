#include "InbetweenerTag/InterpolatedSegment.h"
#include "InbetweenerTag/InterpolatedPoint.h"
#include "OdysseyVectorSegment.h"

FInterpolatedSegment::~FInterpolatedSegment()
{
}

FInterpolatedSegment::FInterpolatedSegment( FOdysseyVectorSegment* iSegment
                                          , FInterpolatedPoint* iInterpolatedPoint0
                                          , FInterpolatedPoint* iInterpolatedPoint1 )
    : mOriginalSegment( iSegment )
    , mInterpolatedVertex{ iInterpolatedPoint0, iInterpolatedPoint1 }
{
}

FOdysseyVectorSegment*
FInterpolatedSegment::GetOriginalSegment()
{
    return mOriginalSegment;
} 
