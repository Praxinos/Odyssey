// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "CoreMinimal.h"

#include <ULIS>
#pragma once

#include <ULIS>
#include <blend2d.h>

#include <InbetweenerTag/InterpolatedPoint.h>
#include <InbetweenerTag/InterpolatedSegment.h>

class FOdysseyVectorPath;

class FInterpolatedPath
{
    public:
        virtual ~FInterpolatedPath();
        FInterpolatedPath( FOdysseyVectorPath* iPath
                         , uint32 iInbetweenCount
                         , bool iPolyline );
        std::vector<FInterpolatedPoint>& GetInterpolatedPointBuffer();
        std::vector<FInterpolatedSegment>& GetInterpolatedSegmentBuffer();
        std::vector<::ULIS::FVec2D>& GetInterpolatedPointPositionBuffer();
        FOdysseyVectorPath* GetOriginalPath();

        friend class FOdysseyVectorTagInbetweener;

    protected:
        void Alloc( bool iPolyline );

    // for temporarily saving transform values at commit.
    public:
        double commitTranslationX;
        double commitTranslationY;
        double commitRotation;
        double commitScalingX;
        double commitScalingY;

    protected:
        FOdysseyVectorPath* mOriginalPath;
        std::vector<FInterpolatedPoint> mInterpolatedPointBuffer;
        std::vector<FInterpolatedSegment> mInterpolatedSegmentBuffer;
        // we alloc point position in one single big array.
        std::vector<::ULIS::FVec2D> mInterpolatedPointPositionBuffer;
};
