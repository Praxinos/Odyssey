// IDDN FR.001.250001.004.S.X.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         Polygon.h
* @author       Thomas Schmitt
* @brief        This file provides the Polygon points generation methods
* @copyright    Copyright 2018-2021 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"

ULIS_NAMESPACE_BEGIN

static inline void GeneratePolygonPoints(
    const std::vector< FVec2I >& iPoints
    , TArray<FVec2I>& ioPolygonPoints )
{
    if(iPoints.size() < 3)
        return;

    for(int i = 0; i < iPoints.size() - 1; i++)
    {
        GenerateLinePoints(iPoints.at(i),iPoints.at(i+1),ioPolygonPoints);
    }
    GenerateLinePoints(iPoints.at(iPoints.size() - 1), iPoints.at(0), ioPolygonPoints);

}

ULIS_NAMESPACE_END

