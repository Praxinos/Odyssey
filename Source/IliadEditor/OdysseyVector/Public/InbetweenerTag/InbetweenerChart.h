// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include <vector>
#include "CoreMinimal.h"
#include <blend2d.h>
#include <ULIS>

class FOdysseyVectorTagInbetweener;
class FInbetweenerBreakdown;
class FInbetweenerChart;
struct FInbetweenerDrawing;

class ODYSSEYVECTOR_API FInbetweenerChart
{
    public:
        class ODYSSEYVECTOR_API Inbetween
        {
            public:
                Inbetween( FInbetweenerChart* iChart );
                uint32 GetIndex();
                uint32 GetIndexInInbetweener();
                int32 GetCellIndex();
                void SetSpacing( float iSpacing );
                float GetSpacing();
                void SetDrawing( FInbetweenerDrawing* iDrawing );
                FInbetweenerDrawing* GetDrawing();
                FInbetweenerChart* GetChart();

            public:
                FInbetweenerChart* mChart;
                float mSpacing;
                FInbetweenerDrawing *mDrawing;
        };

        // quadratic bezier
        class ODYSSEYVECTOR_API HUDBezier
        {
            public:
                struct Fraction
                {
                    float linearT0;
                    float linearT1;
                    float quadraticT0;
                    float quadraticT1;
                };

                struct ODYSSEYVECTOR_API Point
                {
                    public:
                        Point( HUDBezier* iHUDBezier );
                        void SetPosition( double iX, double iY );
                        ::ULIS::FVec2D GetPosition();
                        HUDBezier* GetHUDBezier();

                    private:
                        HUDBezier* mHUDBezier;
                        ::ULIS::FVec2D mPosition;
                };

            public:
                HUDBezier( FInbetweenerChart* iChart );
                void Invalidate();
                void Update();
                // returns spacing T
                double HitTest( const ::ULIS::FVec2D& iWorldPt, uint32 iWorldRadius );
                double GetQuadraticT( float iSpacingT );
                std::vector<Fraction>& GetFractionBuffer();
                std::vector<::ULIS::FVec2D>& GetFractionPointBuffer();
                Point *GetPoints();
                bool IsInvalidated();
                FInbetweenerChart* GetChart();

            private:
                std::vector<::ULIS::FVec2D> mFractionPointBuffer;
                std::vector<Fraction> mFractionBuffer;
                FInbetweenerChart* mChart;
                bool bInvalidated;
                Point mPoint[3];
        };

    public:
        ~FInbetweenerChart();
        FInbetweenerChart( FInbetweenerBreakdown* iBreakdown );

        void Resize();
        void Reset( bool iResetPositionning );
        HUDBezier* GetHUDBezier();

        FInbetweenerBreakdown* GetBreakdown();
        std::vector<FInbetweenerChart::Inbetween>& GetInbetweenBuffer();
        //FInbetweenerDrawing* GetDrawing( uint32 iIndex );
        void GetSpacing( std::vector<float>& oSpacingArray );

        static const uint32 DEFAULT_POSITION_P0_X = 200;
        static const uint32 DEFAULT_POSITION_P0_Y = 40;
        static const uint32 DEFAULT_POSITION_P1_X = 399;
        static const uint32 DEFAULT_POSITION_P1_Y = 40;
        static const uint32 DEFAULT_POSITION_P2_X = 599;
        static const uint32 DEFAULT_POSITION_P2_Y = 40;

    private:
        FInbetweenerBreakdown* mBreakdown;
        std::vector<FInbetweenerChart::Inbetween> mInbetweenBuffer;
        HUDBezier mHUDBezier;
};
