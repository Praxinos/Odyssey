// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "CoreMinimal.h"
#include <ULIS>

class FOdysseyVectorSegment;
class FInterpolatedPoint;

class ODYSSEYVECTOR_API FInterpolatedSegment
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
