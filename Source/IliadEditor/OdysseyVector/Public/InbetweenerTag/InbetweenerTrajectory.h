// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

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
        struct Fraction
        {
            float linearT0;
            float linearT1;
            float cubicT0;
            float cubicT1;
        };

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
        void Resize( uint32 iDrawingCount );
        FInbetweenerWaypoint* GetWaypoint( uint32 iIndex );
        FInbetweenerStep* GetStep( uint32 iIndex );
        FInbetweenerRoute* GetRoute();
        void Import( FInbetweenerTrajectory& iImportFrom );
        FInbetweenerBreakdown* GetBreakdown();
        FInbetweenerTrajectory* GetNext();
        FInbetweenerTrajectory* GetPrev();
        void FitBezier( const std::vector<::ULIS::FVec2D> &data );
        ::ULIS::FVec2D GetPoint( float iSpacingT );
        void FitBezier( const std::vector<::ULIS::FVec2D> &data
                      , const std::vector<float> &u );
        void Reset();
        double GetCubicT( float iLinearT );
        double GetLinearT( float iCubicT );

    private:
        std::vector<FInbetweenerWaypoint> mWaypointBuffer;
        FInbetweenerRoute* mRoute;
        FInbetweenerStep* mStep[2];
        FInbetweenerHandleTrajectory mHandle[2];
        ::ULIS::FVec2D mCubicBezier[4];
        FInbetweenerBreakdown* mBreakdown;
        std::vector<Fraction> mFractionBuffer;
};
