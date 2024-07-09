#pragma once

#include "CoreMinimal.h"
#include <ULIS>

#include "InbetweenerTag/InbetweenerHandleTrajectory.h"

class FInbetweenerQuad;
class FOdysseyVectorTagInbetweener;

class ODYSSEYVECTOR_API FInbetweenerTrajectory
{
    public:
        virtual ~FInbetweenerTrajectory();
        FInbetweenerTrajectory( FOdysseyVectorTagInbetweener* iInbetweenerTag
                              , uint32 iQuadIndex
                              , double iQuadU
                              , double iQuadV );
        void Init( uint32 iQuadIndex
                 , double iQuadU
                 , double iQuadV );
        FInbetweenerHandleTrajectory* GetHandle( uint32 index );
        FInbetweenerQuad* GetQuad();
        uint32 GetQuadIndex();
        double GetQuadU();
        double GetQuadV();
        void Update();
        ::ULIS::FVec2D* GetCubicBezier();
        FOdysseyVectorTagInbetweener* GetInbetweenerTag();

    private:
        FOdysseyVectorTagInbetweener* mInbetweenerTag;
        ::ULIS::FVec2D mCubicBezier[4];
        FInbetweenerHandleTrajectory mHandle[2];
        uint32 mQuadIndex;
        double mQuadU;
        double mQuadV;
};
