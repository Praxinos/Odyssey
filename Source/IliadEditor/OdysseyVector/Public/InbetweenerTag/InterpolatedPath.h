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
class FOdysseyVectorTagInbetweener;

class FInterpolatedPath
{
    public:
        struct PointGeometry
        {
            ::ULIS::FVec2D position;
            double radius;
        };

    public:
        virtual ~FInterpolatedPath();
        FInterpolatedPath( FOdysseyVectorTagInbetweener* iInbetweenerTag
                         , FOdysseyVectorPath* iPath
                         , uint32 iInbetweenCount
                         , bool iPolyline );
        std::vector<FInterpolatedPoint>& GetInterpolatedPointBuffer();
        std::vector<FInterpolatedSegment>& GetInterpolatedSegmentBuffer();
        std::vector<FInterpolatedPath::PointGeometry>& GetInterpolatedPointGeometryBuffer();
        FOdysseyVectorPath* GetOriginalPath();
        BLMatrix2D& GetRelativeMatrix();

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
        std::vector<PointGeometry> mInterpolatedPointGeometryBuffer;
        BLMatrix2D mRelativeMatrix; // matrix relative to the inbetweener tag's owner
};
