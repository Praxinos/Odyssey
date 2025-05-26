// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include <ULIS>

class FInbetweenerTrajectory;

class ODYSSEYVECTOR_API FInbetweenerHandleTrajectory
{
    public:
        virtual ~FInbetweenerHandleTrajectory(){};
        FInbetweenerHandleTrajectory( FInbetweenerTrajectory* iTrajectory );

        void Set( const ::ULIS::FVec2D& iDirection, double iLengthRatio );
        FInbetweenerTrajectory* GetTrajectory();
        const ::ULIS::FVec2D& GetDirection();
        double GetLengthRatio();

    protected:
        FInbetweenerTrajectory* mTrajectory;
        ::ULIS::FVec2D mDirection;
        double mLengthRatio;
};
