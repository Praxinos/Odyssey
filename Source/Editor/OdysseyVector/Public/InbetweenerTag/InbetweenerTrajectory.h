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
                              , FInbetweenerQuad* iQuad
                              , double iQuadU
                              , double iQuadV );
        FInbetweenerHandleTrajectory* GetHandle( uint32 index );
        FInbetweenerQuad* GetQuad();
        double GetQuadU();
        double GetQuadV();
        void Update();
        ::ULIS::FVec2D* GetCubicBezier();
        FInbetweenerGrid* GetGrid();

    private:
        FInbetweenerGrid* mGrid;
        ::ULIS::FVec2D mCubicBezier[4];
        FInbetweenerHandleTrajectory mHandle[2];
        FInbetweenerQuad* mQuad;
        double mQuadU;
        double mQuadV;
};
