#pragma once

#include "CoreMinimal.h"
#include <ULIS>

#include <blend2d.h>
#include "InbetweenerTag/InbetweenerHandleTrajectory.h"
#include "InbetweenerTag/InbetweenerWaypoint.h"

class FInbetweenerQuad;
class FInbetweenerRoute;
class FInbetweenerBreakdown;
class FOdysseyVectorTagInbetweener;

class ODYSSEYVECTOR_API FInbetweenerTrajectory
{
    public:
        virtual ~FInbetweenerTrajectory();
        FInbetweenerTrajectory( FInbetweenerRoute* iRoute
                              , FInbetweenerBreakdown* iBreakdown );
        void Init( uint32 iInbetweenCount );
        FInbetweenerHandleTrajectory* GetHandle( uint32 index );
        FInbetweenerQuad* GetQuad();
        void Update();
        ::ULIS::FVec2D* GetCubicBezier();
        std::vector<FInbetweenerWaypoint>& GetWaypointBuffer();
        void ResetSpacing( uint32 iInbetweenCount );
        FInbetweenerWaypoint* GetWaypoint( uint32 iIndex );
        FInbetweenerRoute* GetRoute();

    private:
        std::vector<FInbetweenerWaypoint> mWaypointBuffer;
        FInbetweenerRoute* mRoute;
        FInbetweenerHandleTrajectory mHandle[2];
        ::ULIS::FVec2D mCubicBezier[4];
        FInbetweenerBreakdown* mBreakdown;
};
