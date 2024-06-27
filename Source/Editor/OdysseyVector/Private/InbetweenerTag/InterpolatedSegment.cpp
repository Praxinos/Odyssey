#include "InbetweenerTag/InterpolatedSegment.h"
#include "InbetweenerTag/InterpolatedPoint.h"
#include "OdysseyVectorSegment.h"

FInterpolatedSegment::~FInterpolatedSegment()
{
}

FInterpolatedSegment::FInterpolatedSegment( FOdysseyVectorSegment* iSegment )
    : mOriginalSegment( iSegment )
{
}

FInterpolatedSegment::FInterpolatedSegment( FOdysseyVectorSegment* iSegment
                                          , const std::vector<FInterpolatedPoint*>& iPolylinePointArray )
    : mOriginalSegment( iSegment )
    , mInterpolatedPointArray( iPolylinePointArray )
{
}

FOdysseyVectorSegment*
FInterpolatedSegment::GetOriginalSegment()
{
    return mOriginalSegment;
} 
