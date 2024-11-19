// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

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
