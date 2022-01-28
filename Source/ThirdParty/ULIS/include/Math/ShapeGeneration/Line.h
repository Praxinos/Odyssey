// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         Line.h
* @author       Thomas Schmitt
* @brief        This file provides the line points generation methods
* @license      Please refer to LICENSE.md
*/

#pragma once
#include "Core/Core.h"

ULIS_NAMESPACE_BEGIN


static inline void GenerateLinePoints(
      const FVec2I& iP0
    , const FVec2I& iP1
    , TArray<FVec2I>& ioLinePoints
    , int iCustomIndex = -1 )//Custom index at which we begin to generate the points
{
    FVec2I p0 = iP0;
    FVec2I p1 = iP1;

    int startArray = iCustomIndex != -1 ? iCustomIndex : static_cast< int >( ioLinePoints.Size() ); 

    bool pushArray = true; //While inserting the points into the array, if true, we push back, else, we insert at front

    if (::ULIS::FMath::Abs(p1.y - p0.y) < ::ULIS::FMath::Abs(p1.x - p0.x)) // x slope > y slope
    {
        if (p1.x < p0.x)
        {
            pushArray = false;
            FVec2I temp = p0;
            p0 = p1;
            p1 = temp;
        }

        int dx = p1.x - p0.x;
        int dy = p1.y - p0.y;
        int yStep = 1;

        if (dy < 0)
        {
            yStep = -1;
            dy = -dy;
        }

        int slopeDifferential = 2 * dy - dx;
        int y = p0.y;

        for (int x = p0.x; x <= p1.x; x++)
        {
            if ( iCustomIndex == -1 && pushArray )
                ioLinePoints.PushBack( FVec2I( x, y ) );
            else
                ioLinePoints.Insert( startArray, FVec2I( x, y ) );

            if (slopeDifferential > 0)
            {
                y += yStep;
                slopeDifferential -= (2 * dx);
            }
            slopeDifferential += (2 * dy);
        }
    }
    else //y slope > x slope
    {
        if (p1.y < p0.y)
        {
            pushArray = false;
            FVec2I temp = p0;
            p0 = p1;
            p1 = temp;
        }

        int dx = p1.x - p0.x;
        int dy = p1.y - p0.y;
        int xStep = 1;

        if (dx < 0)
        {
            xStep = -1;
            dx = -dx;
        }

        int slopeDifferential = 2 * dx - dy;
        int x = p0.x;

        for (int y = p0.y; y <= p1.y; y++)
        {
            if (iCustomIndex == -1 && pushArray)
                ioLinePoints.PushBack( FVec2I( x, y ) );
            else
                ioLinePoints.Insert(startArray, FVec2I(x, y));

            if (slopeDifferential > 0)
            {
                x += xStep;
                slopeDifferential -= (2 * dy);
            }
            slopeDifferential += (2 * dx);
        }
    }
}


ULIS_NAMESPACE_END

