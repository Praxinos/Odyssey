// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

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
    double skewX;
    double skewY;

    FInbetweenerDrawing( FOdysseyVectorTagInbetweener* iInbetweenerTag );
    uint32 GetIndex();
    int32 GetCellIndex();
};
