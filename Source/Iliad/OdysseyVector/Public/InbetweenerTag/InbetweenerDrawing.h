// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

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

    double translationX;
    double translationY;
    double rotation;
    double scalingX;
    double scalingY;


    FInbetweenerDrawing( FOdysseyVectorTagInbetweener* iInbetweenerTag );
    uint32 GetIndex();
    int32 GetCellIndex();
};
