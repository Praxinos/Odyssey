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
class FInbetweenerStep;

class ODYSSEYVECTOR_API FInbetweenerTrajectory
{
    public:
        virtual ~FInbetweenerTrajectory();
        FInbetweenerTrajectory( FInbetweenerRoute* iRoute
                              , FInbetweenerStep* iStep0
                              , FInbetweenerStep* iStep1
                              , FInbetweenerBreakdown* iBreakdown );
        void Init( uint32 iDrawingCount );
        FInbetweenerHandleTrajectory* GetHandle( uint32 index );
        FInbetweenerQuad* GetQuad();
        void Update();
        ::ULIS::FVec2D* GetCubicBezier();
        std::vector<FInbetweenerWaypoint>& GetWaypointBuffer();
        void ResetSpacing( uint32 iDrawingCount );
        FInbetweenerWaypoint* GetWaypoint( uint32 iIndex );
        FInbetweenerStep* GetStep( uint32 iIndex );
        FInbetweenerRoute* GetRoute();
        void Import( FInbetweenerTrajectory& iImportFrom );
        FInbetweenerBreakdown* GetBreakdown();
        FInbetweenerTrajectory* GetNext();
        FInbetweenerTrajectory* GetPrev();

    private:
        std::vector<FInbetweenerWaypoint> mWaypointBuffer;
        FInbetweenerRoute* mRoute;
        FInbetweenerStep* mStep[2];
        FInbetweenerHandleTrajectory mHandle[2];
        ::ULIS::FVec2D mCubicBezier[4];
        FInbetweenerBreakdown* mBreakdown;
};
