#pragma once

#include <vector>
#include "CoreMinimal.h"
#include <blend2d.h>

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
    uint32 GetAbsoluteIndex();
    int32 GetAnimationCellIndex();
};

class ODYSSEYVECTOR_API FInbetweenerChart
{
    public:
        ~FInbetweenerChart();
        FInbetweenerChart( FInbetweenerBreakdown* iBreakdown );

        void Resize();
        void Reset();

        FInbetweenerBreakdown* GetBreakdown();
        std::vector<FChartDivision>& GetDivisionArray();
        //FInbetweenerDrawing* GetDrawing( uint32 iIndex );
        void GetSpacing( std::vector<float>& oSpacingArray );

    private:
        FInbetweenerBreakdown* mBreakdown;
        std::vector<FChartDivision> mDivisionArray;
};
