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
