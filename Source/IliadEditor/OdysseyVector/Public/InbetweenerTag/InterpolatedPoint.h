// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "CoreMinimal.h"
#include <ULIS>

class FOdysseyVectorPoint;
class FInbetweenerQuad;

class FInterpolatedPoint
{
    public:
        virtual ~FInterpolatedPoint();
        FInterpolatedPoint( FOdysseyVectorPoint* iPoint, double iRadius, uint32 iIndex );
        FOdysseyVectorPoint* GetOriginalPoint();
        void SetUV( uint32 iMappedQuadIndex, double iU, double iV );
        uint32 GetMappedQuadIndex();
        void SetU( double iU );
        void SetV( double iV );
        double GetU();
        double GetV();
        uint32 GetIndex();
        double GetRadius();

        friend class FOdysseyVectorTagInbetweener;

    protected:
        uint32 mMappedQuadIndex;
        FOdysseyVectorPoint* mOriginalPoint;
        double mRadius;
        uint32 mIndex;
        double mU;
        double mV;
};
