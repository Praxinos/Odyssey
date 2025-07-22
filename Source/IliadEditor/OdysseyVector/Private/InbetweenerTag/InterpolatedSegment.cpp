// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

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

std::vector<FInterpolatedPoint*>&
FInterpolatedSegment::GetInterpolatedPointArray()
{
    return mInterpolatedPointArray;
}
