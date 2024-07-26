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
        void SetUV( uint32 iMappedQuadIndex, double iU, double iV );
        uint32 GetMappedQuadIndex();
        void SetU( double iU );
        void SetV( double iV );
        double GetU();
        double GetV();
        uint32 GetIndex();


        friend class FOdysseyVectorTagInbetweener;

    protected:
        uint32 mMappedQuadIndex;
        FOdysseyVectorPoint* mOriginalPoint;
        uint32 mIndex;
        double mU;
        double mV;
};
