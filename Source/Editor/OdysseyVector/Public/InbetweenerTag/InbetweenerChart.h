#pragma once

#include <vector>
#include "CoreMinimal.h"
#include <blend2d.h>

class FOdysseyVectorTagInbetweener;
class FInbetweenerBreakdown;
class FInbetweenerChart;

struct ODYSSEYVECTOR_API FInbetweenerDrawing
{
    float spacing;
    float breakdownSpacing; // spacing relative to the current breakdown
    FInbetweenerBreakdown* breakdown;
    BLMatrix2D localMatrix;
    BLMatrix2D worldMatrix;
    BLMatrix2D inverseMatrix;
    BLMatrix2D inverseWorldMatrix;
    FInbetweenerChart* mChart;

    FInbetweenerDrawing( FInbetweenerChart* iChart );

    uint32 GetIndex();
    int32 GetAnimationCellIndex();
};

class ODYSSEYVECTOR_API FInbetweenerChart
{
    public:
        ~FInbetweenerChart();
        FInbetweenerChart();
        FInbetweenerChart( FOdysseyVectorTagInbetweener* iInbetweenerTag );

        void Resize();
        void Reset();

        FOdysseyVectorTagInbetweener* GetInbetweenerTag();
        std::vector<FInbetweenerDrawing>& GetDrawingBuffer();
        FInbetweenerDrawing* GetDrawing( uint32 iIndex );
        void GetSpacing( std::vector<float>& oSpacingArray );

    private:
        FOdysseyVectorTagInbetweener* mInbetweenerTag;
        std::vector<FInbetweenerDrawing> mDrawingBuffer;
};
