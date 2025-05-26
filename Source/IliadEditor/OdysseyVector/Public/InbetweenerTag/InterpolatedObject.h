// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "CoreMinimal.h"

#include <ULIS>
#pragma once

#include <ULIS>
#include <blend2d.h>

#include <InbetweenerTag/InterpolatedPoint.h>

class FOdysseyVectorObject;
class FOdysseyVectorTagInbetweener;

class ODYSSEYVECTOR_API FInterpolatedObject
{
    public:
        struct PointGeometry
        {
            ::ULIS::FVec2D position;
            double radius;
        };

    public:
        virtual ~FInterpolatedObject();
        FInterpolatedObject( FOdysseyVectorTagInbetweener* iInbetweenerTag
                           , FOdysseyVectorObject* iObject );
        FOdysseyVectorObject* GetOriginalObject();
        BLMatrix2D& GetRelativeMatrix();
        std::vector<FInterpolatedPoint>& GetInterpolatedPointBuffer();
        std::vector<FInterpolatedObject::PointGeometry>& GetInterpolatedPointGeometryBuffer();

        friend class FOdysseyVectorTagInbetweener;

    // for temporarily saving transform values at commit.
    public:
        double commitTranslationX;
        double commitTranslationY;
        double commitRotation;
        double commitScalingX;
        double commitScalingY;

    protected:
        FOdysseyVectorObject* mOriginalObject;
        BLMatrix2D mRelativeMatrix; // matrix relative to the inbetweener tag's owner
        std::vector<FInterpolatedPoint> mInterpolatedPointBuffer;
        // we alloc point position in one single big array.
        std::vector<PointGeometry> mInterpolatedPointGeometryBuffer;
};
