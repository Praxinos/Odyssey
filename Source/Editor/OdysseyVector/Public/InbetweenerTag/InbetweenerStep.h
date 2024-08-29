#pragma once

#include "CoreMinimal.h"
#include <ULIS>

// MACRO check exists in Unreal and conflicts with another one defined in Eigen. We temporarily undefine it.
#pragma push_macro("check")
#undef check

#include <Eigen/Geometry>
#include <Eigen/SparseCore>
#include <Eigen/SparseLU>

#pragma pop_macro("check")

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
