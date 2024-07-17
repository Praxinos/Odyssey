#pragma once

#include "CoreMinimal.h"
#include <ULIS>

class FInbetweenerTrajectory;

class ODYSSEYVECTOR_API FInbetweenerWaypoint
{
    public:
        virtual ~FInbetweenerWaypoint(){};
        FInbetweenerWaypoint( FInbetweenerTrajectory* iTrajectory );

        void SetT( float iT );
        float GetT();
        FInbetweenerTrajectory* GetTrajectory();
        void SetRatio( float iRatio );
        float GetRatio();

    protected:
        FInbetweenerTrajectory* mTrajectory;
        float mRatio;
};
