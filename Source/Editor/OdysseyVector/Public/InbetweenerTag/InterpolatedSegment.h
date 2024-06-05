#pragma once

#include "CoreMinimal.h"
#include <ULIS>

class FOdysseyVectorSegment;
class FInterpolatedPoint;

class FInterpolatedSegment
{
    public:
        virtual ~FInterpolatedSegment();
        FInterpolatedSegment( FOdysseyVectorSegment* iSegment
                            , FInterpolatedPoint* iInterpolatedPoint0
                            , FInterpolatedPoint* iInterpolatedPoint1 );

        friend class FOdysseyVectorTagInbetweener;

    protected:
        FOdysseyVectorSegment* mOriginalSegment;
        FInterpolatedPoint* mInterpolatedVertex[2];
};
