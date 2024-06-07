#pragma once

#include "CoreMinimal.h"
#include <ULIS>

class FOdysseyVectorPoint;
class FInbetweenerQuad;

class FInterpolatedPoint
{
    public:
        virtual ~FInterpolatedPoint();
        FInterpolatedPoint( FOdysseyVectorPoint* iPoint, uint32 iIndex );
        FOdysseyVectorPoint* GetOriginalPoint();
        void SetUV( FInbetweenerQuad* iMappedQuad, double iU, double iV );
        FInbetweenerQuad* GetMappedQuad();
        double GetU();
        double GetV();


        friend class FOdysseyVectorTagInbetweener;
        friend class FInbetweenerGridFFD;
        friend class FInbetweenerGrid;

    protected:
        FInbetweenerQuad* mMappedQuad;
        FOdysseyVectorPoint* mOriginalPoint;
        uint32 mIndex;
        double mU;
        double mV;
};
