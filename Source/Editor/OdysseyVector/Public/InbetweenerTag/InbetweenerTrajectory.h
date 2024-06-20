#pragma once

#include "CoreMinimal.h"
#include <ULIS>

#include "InbetweenerTag/InbetweenerHandleTrajectory.h"

class FInbetweenerQuad;
class FInbetweenerGrid;

class ODYSSEYVECTOR_API FInbetweenerTrajectory
{
    public:
        virtual ~FInbetweenerTrajectory();
        FInbetweenerTrajectory( FInbetweenerGrid* iGrid
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
        FInbetweenerGrid* GetGrid();

    private:
        FInbetweenerGrid* mGrid;
        ::ULIS::FVec2D mCubicBezier[4];
        FInbetweenerHandleTrajectory mHandle[2];
        uint32 mQuadIndex;
        double mQuadU;
        double mQuadV;
};
