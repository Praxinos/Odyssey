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
