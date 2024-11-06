#pragma once

#include "CoreMinimal.h"
#include <ULIS>

class FOdysseyVectorSegment;
class FInterpolatedPoint;

class FInterpolatedSegment
{
    public:
        virtual ~FInterpolatedSegment();
        FInterpolatedSegment( FOdysseyVectorSegment* iSegment );
        FInterpolatedSegment( FOdysseyVectorSegment* iSegment
                            , const std::vector<FInterpolatedPoint*>& iPolylinePointArray );
        FOdysseyVectorSegment* GetOriginalSegment();
        std::vector<FInterpolatedPoint*>& GetInterpolatedPointArray();

        friend class FOdysseyVectorTagInbetweener;

        void Thicken();

    protected:
        FOdysseyVectorSegment* mOriginalSegment;
        std::vector<FInterpolatedPoint*> mInterpolatedPointArray;
        //std::vector<FOdysseyVectorPoint> mThicknessPointBuffer[2];
};
