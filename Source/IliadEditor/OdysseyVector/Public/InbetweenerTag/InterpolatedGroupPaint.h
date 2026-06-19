// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "CoreMinimal.h"

#include <ULIS>
#pragma once

#include <ULIS>
#include <blend2d.h>

#include <InbetweenerTag/InterpolatedObject.h>
#include <InbetweenerTag/InterpolatedPoint.h>

class FOdysseyVectorGroupPaint;
class FOdysseyVectorTagInbetweener;

class ODYSSEYVECTOR_API FInterpolatedGroupPaint : public FInterpolatedObject
{
    public:
        virtual ~FInterpolatedGroupPaint();
        FInterpolatedGroupPaint( FOdysseyVectorTagInbetweener* iInbetweenerTag
                               , FOdysseyVectorGroupPaint* iPaintgroup );
        FOdysseyVectorGroupPaint* GetOriginalGroupPaint();

    protected:
        void Alloc();
};
