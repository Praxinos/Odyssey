#include "InbetweenerTag/InterpolatedSegmentCubic.h"
#include "InbetweenerTag/InterpolatedPoint.h"
#include "OdysseyVectorSegmentCubic.h"

FInterpolatedSegmentCubic::~FInterpolatedSegmentCubic()
{
}

FInterpolatedSegmentCubic::FInterpolatedSegmentCubic( FOdysseyVectorSegmentCubic* iCubicSegment
                                                    , FInterpolatedPoint* iInterpolatedPoint0
                                                    , FInterpolatedPoint* iInterpolatedHandle0
                                                    , FInterpolatedPoint* iInterpolatedHandle1
                                                    , FInterpolatedPoint* iInterpolatedPoint1 )
    : FInterpolatedSegment( iCubicSegment, iInterpolatedPoint0, iInterpolatedPoint1 )
    , mInterpolatedHandle{ iInterpolatedHandle0, iInterpolatedHandle1 }
{
}
