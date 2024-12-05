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

struct ODYSSEYVECTOR_API FChartDivision
{
    float spacing;
    FInbetweenerChart* chart;
    FInbetweenerDrawing *drawing;

    FChartDivision( FInbetweenerChart* iChart );
    uint32 GetIndex();
    uint32 GetIndexInInbetweener();
    int32 GetCellIndex();
    void SetSpacing( float iSpacing );
};

class ODYSSEYVECTOR_API FInbetweenerChart
{
    public:
        ~FInbetweenerChart();
        FInbetweenerChart( FInbetweenerBreakdown* iBreakdown );

        void Resize();
        void Reset( bool iResetPositionning );
        ::ULIS::FVec2D* GetHUDBezier();

        FInbetweenerBreakdown* GetBreakdown();
        std::vector<FChartDivision>& GetDivisionBuffer();
        //FInbetweenerDrawing* GetDrawing( uint32 iIndex );
        void GetSpacing( std::vector<float>& oSpacingArray );

    private:
        FInbetweenerBreakdown* mBreakdown;
        std::vector<FChartDivision> mDivisionBuffer;
        ::ULIS::FVec2D mHUDBezier[3]; // quadratic bezier
};
