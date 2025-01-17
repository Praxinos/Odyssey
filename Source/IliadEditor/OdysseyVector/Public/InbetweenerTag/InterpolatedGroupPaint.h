// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "CoreMinimal.h"

#include <ULIS>
#pragma once

#include <ULIS>
#include <blend2d.h>

#include <InbetweenerTag/InterpolatedObject.h>
#include <InbetweenerTag/InterpolatedPoint.h>

class FOdysseyVectorGroupPaint;
class FOdysseyVectorTagInbetweener;

class FInterpolatedGroupPaint : public FInterpolatedObject
{
    public:
        virtual ~FInterpolatedGroupPaint();
        FInterpolatedGroupPaint( FOdysseyVectorTagInbetweener* iInbetweenerTag
                               , FOdysseyVectorGroupPaint* iPaintgroup );
        FOdysseyVectorGroupPaint* GetOriginalGroupPaint();

    protected:
        void Alloc();
};
