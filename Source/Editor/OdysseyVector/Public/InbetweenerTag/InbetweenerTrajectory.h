#pragma once

#include "CoreMinimal.h"
#include <ULIS>

class FInbetweenerQuad;

class ODYSSEYVECTOR_API FInbetweenerTrajectory
{
    public:
        virtual ~FInbetweenerTrajectory();
        FInbetweenerTrajectory( FInbetweenerQuad* iQuad
                              , double iQuadU
                              , double iQuadV );

    private:
        ::ULIS::FVec2D mCubicBezier[4];
        FInbetweenerQuad* mQuad;
        double mQuadU;
        double mQuadV;
};
