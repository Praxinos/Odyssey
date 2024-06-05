#pragma once

#include "CoreMinimal.h"
#include <ULIS>

class FOdysseyVectorPoint;


class FInterpolatedPoint
{
    public:
        virtual ~FInterpolatedPoint();
        FInterpolatedPoint( FOdysseyVectorPoint* iPoint
                          , uint32 iIndex
                          , double iU
                          , double iV );

        friend class FOdysseyVectorTagInbetweener;
        friend class FInbetweenerGridFFD;
        friend class FInbetweenerGrid;

    protected:
        FOdysseyVectorPoint* mOriginalPoint;
        uint32 mIndex;
        double mU;
        double mV;
};
