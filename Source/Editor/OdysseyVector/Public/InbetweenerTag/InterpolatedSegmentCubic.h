#pragma once

#include "CoreMinimal.h"

#include <ULIS>

#include "InbetweenerTag/InterpolatedSegment.h"

class FOdysseyVectorSegmentCubic;
class FInterpolatedPoint;

class FInterpolatedSegmentCubic : public FInterpolatedSegment
{
    public:
        virtual ~FInterpolatedSegmentCubic();
        FInterpolatedSegmentCubic( FOdysseyVectorSegmentCubic* iCubicSegment
                                 , FInterpolatedPoint* iInterpolatedPoint0
                                 , FInterpolatedPoint* iInterpolatedHandle0
                                 , FInterpolatedPoint* iInterpolatedHandle1
                                 , FInterpolatedPoint* iInterpolatedPoint1 );

        friend class FOdysseyVectorTagInbetweener;

    protected:
        FInterpolatedPoint* mInterpolatedHandle[2];
};
