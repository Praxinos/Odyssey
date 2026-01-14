// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include <ULIS>

class FInbetweenerRoute;
class FInbetweenerTrajectory;
struct FInbetweenerDrawing;

class ODYSSEYVECTOR_API FInbetweenerStep
{
    public:
        virtual ~FInbetweenerStep(){};
        FInbetweenerStep( FInbetweenerRoute *iRoute );

        void SetAligned( bool iIsAligned );
        bool IsAligned();

        void AddTrajectory( FInbetweenerTrajectory* iTrajectory );
        void SmoothTrajectories();
        FInbetweenerDrawing* GetDrawing();
        FInbetweenerRoute* GetRoute();
        uint32 GetIndex();

    public:
        static const uint32 ALIGNED = ( 1UL << 0 );

    protected:
        uint32 mFlags;
        FInbetweenerRoute* mRoute;
        std::vector<FInbetweenerTrajectory*> mTrajectoryArray;
};
