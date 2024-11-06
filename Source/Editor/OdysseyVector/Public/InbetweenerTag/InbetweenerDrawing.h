#pragma once

#include <vector>
#include "CoreMinimal.h"
#include <blend2d.h>

class FOdysseyVectorTagInbetweener;
class FInbetweenerBreakdown;
class FInbetweenerChart;

struct ODYSSEYVECTOR_API FInbetweenerDrawing
{
    FOdysseyVectorTagInbetweener* inbetweenerTag;
    BLMatrix2D localMatrix;
    BLMatrix2D worldMatrix;
    BLMatrix2D inverseMatrix;
    BLMatrix2D inverseWorldMatrix;

    FInbetweenerDrawing( FOdysseyVectorTagInbetweener* iInbetweenerTag );
    uint32 GetIndex();
    int32 GetCellIndex();
};
