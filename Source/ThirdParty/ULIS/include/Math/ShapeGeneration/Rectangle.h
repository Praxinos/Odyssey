// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         Rectangle.h
* @author       Thomas Schmitt
* @brief        This file provides the Rectangle points generation methods
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"
#include "Math/ShapeGeneration/Line.h"

ULIS_NAMESPACE_BEGIN

static inline void GenerateRectanglePoints(
    const FVec2I& iTopLeft
    , const FVec2I& iBottomRight
    , TArray<FVec2I>& ioRectanglePoints )
{
    const int xmin = FMath::Min(iTopLeft.x,iBottomRight.x);
    const int ymin = FMath::Min(iTopLeft.y,iBottomRight.y);
    const int xmax = FMath::Max(iTopLeft.x,iBottomRight.x);
    const int ymax = FMath::Max(iTopLeft.y,iBottomRight.y);

    GenerateLinePoints(FVec2I(xmin, ymin), FVec2I(xmax, ymin), ioRectanglePoints ); // Top
    GenerateLinePoints(FVec2I(xmax, ymin), FVec2I(xmax, ymax), ioRectanglePoints); // Right
    GenerateLinePoints(FVec2I(xmax, ymax), FVec2I(xmin, ymax), ioRectanglePoints); // Bot
    GenerateLinePoints(FVec2I(xmin, ymax), FVec2I(xmin, ymin), ioRectanglePoints); // Left
}
ULIS_NAMESPACE_END

