#include "CoreMinimal.h"

#include <ULIS>
#pragma once

#include <ULIS>
#include <blend2d.h>

class FOdysseyVectorPath;
class FInterpolatedPoint;
class FInterpolatedSegmentCubic;

class FInterpolatedPath
{
    public:
        virtual ~FInterpolatedPath();
        FInterpolatedPath( FOdysseyVectorPath* iPath
                         , const ::ULIS::FRectD& iSpaceBBox
                         , const BLMatrix2D& iSpaceInverseMatrix
                         , uint32 iInbetweenCount );
        void Draw( BLContext* iBLContext
                 , const ::ULIS::FRectD& iInvalidationArea
                 , double iAncestorsOpacity
                 , uint64 iDrawingFlags );
        std::vector<FInterpolatedPoint>& GetInterpolatedPointBuffer();
        std::vector<::ULIS::FVec2D>& GetInterpolatedPointPositionBuffer();

        friend class FOdysseyVectorTagInbetweener;

    protected:
        FOdysseyVectorPath* mOriginalPath;
        std::vector<FInterpolatedPoint> mInterpolatedPointBuffer;
        std::vector<FInterpolatedSegmentCubic> mInterpolatedSegmentCubicBuffer;
        // we alloc point position in one single big array.
        std::vector<::ULIS::FVec2D> mInterpolatedPointPositionBuffer;
};
