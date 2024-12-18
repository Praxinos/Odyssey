// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

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
