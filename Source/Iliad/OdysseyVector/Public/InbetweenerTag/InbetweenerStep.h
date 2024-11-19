// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "CoreMinimal.h"
#include <ULIS>

class FInbetweenerRoute;
class FInbetweenerTrajectory;

class ODYSSEYVECTOR_API FInbetweenerStep
{
    public:
        virtual ~FInbetweenerStep(){};
        FInbetweenerStep( FInbetweenerRoute *iRoute );

        void SetAligned( bool iIsAligned );
        bool IsAligned();

        void AddTrajectory( FInbetweenerTrajectory* iTrajectory );

        FInbetweenerRoute* GetRoute();

    public:
        static const uint32 ALIGNED = ( 1UL << 0 );

    protected:
        uint32 mFlags;
        FInbetweenerRoute* mRoute;
        std::vector<FInbetweenerTrajectory*> mTrajectoryArray;
};
