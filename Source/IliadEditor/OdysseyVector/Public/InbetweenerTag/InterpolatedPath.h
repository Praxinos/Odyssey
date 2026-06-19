// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "CoreMinimal.h"

#include <ULIS>
#pragma once

#include <ULIS>
#include <blend2d.h>

#include <InbetweenerTag/InterpolatedObject.h>
#include <InbetweenerTag/InterpolatedPoint.h>
#include <InbetweenerTag/InterpolatedSegment.h>

class FOdysseyVectorPath;
class FOdysseyVectorTagInbetweener;

class ODYSSEYVECTOR_API FInterpolatedPath : public FInterpolatedObject
{
    public:
        virtual ~FInterpolatedPath();
        FInterpolatedPath( FOdysseyVectorTagInbetweener* iInbetweenerTag
                         , FOdysseyVectorPath* iPath
                         , bool iPolyline );
        std::vector<FInterpolatedSegment>& GetInterpolatedSegmentBuffer();
        FOdysseyVectorPath* GetOriginalPath();

    protected:
        void Alloc( bool iPolyline );

    protected:
        std::vector<FInterpolatedSegment> mInterpolatedSegmentBuffer;
};
