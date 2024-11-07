// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         Ellipse.h
* @author       Thomas Schmitt
* @brief        This file provides the Ellipse points generation methods
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"

ULIS_NAMESPACE_BEGIN

static inline void GenerateEllipsePoints(
    const FVec2I& iCenter
    , const int iA
    , const int iB
    , TArray<FVec2I>& ioEllipsePoints
)
{
    if( iA <= 0 || iB <= 0 )
        return;

    int a2 = iA * iA;
    int b2 = iB * iB;
    int fa2 = 4 * a2;
    int fb2 = 4 * b2;
    int x = 0;
    int y = iB;
    int sigma = 2*b2+a2*(-2*iB);
    int octantStartingIndex = 0;

    //0° is on top and we turn clockwise
    //Octant 1 ------

    while(b2 * x <= a2 * y)
    {
        ioEllipsePoints.PushBack( FVec2I( iCenter.x + x,iCenter.y - y ) );

        if(sigma >= 0)
        {
            sigma += fa2 * (1 - y);
            y--;
        }
        sigma += b2*(4 * x + 6);
        x++;
    }

    octantStartingIndex = static_cast< int >( ioEllipsePoints.Size() );

    //Octant 2 ------
    x = iA;
    y = 0;
    sigma = 2*a2+b2*(-2*iA);
    while(a2 * y <= b2 * x)
    {
        ioEllipsePoints.Insert( octantStartingIndex, FVec2I( iCenter.x + x,iCenter.y - y ) );

        if(sigma >= 0)
        {
            sigma += fb2 * (1 - x);
            x--;
        }
        sigma += a2*(4 * y + 6);
        y++;
    }

    octantStartingIndex = static_cast< int >( ioEllipsePoints.Size() );

    //Octant 3 ------
    x = iA;
    y = 0;
    sigma = 2*a2+b2*(-2*iA);

    while(a2 * y <= b2 * x)
    {
        ioEllipsePoints.PushBack(FVec2I( iCenter.x + x,iCenter.y + y ));

        if(sigma >= 0)
        {
            sigma += fb2 * (1 - x);
            x--;
        }
        sigma += a2*(4 * y + 6);
        y++;
    }

    octantStartingIndex = static_cast< int >( ioEllipsePoints.Size() );
   
    //Octant 4 ------
    x = 0;
    y = iB;
    sigma = 2*b2+a2*(-2*iB);

    while (b2 * x <= a2 * y)
    {
        ioEllipsePoints.Insert(octantStartingIndex, FVec2I(iCenter.x + x, iCenter.y + y));

        if (sigma >= 0)
        {
            sigma += fa2 * (1 - y);
            y--;
        }
        sigma += b2 * (4 * x + 6);
        x++;
    }

    octantStartingIndex = static_cast< int >( ioEllipsePoints.Size() );

    //Octant 5 ------
    x = 0;
    y = iB;
    sigma = 2*b2+a2*(-2*iB);

    while(b2 * x <= a2 * y)
    {
        ioEllipsePoints.PushBack( FVec2I(iCenter.x - x, iCenter.y + y));

        if(sigma >= 0)
        {
            sigma += fa2 * (1 - y);
            y--;
        }
        sigma += b2*(4 * x + 6);
        x++;
    }

    octantStartingIndex = static_cast< int >( ioEllipsePoints.Size() );

    //Octant 6 ------
    x = iA;
    y = 0;
    sigma = 2*a2+b2*(-2*iA);

    while (a2 * y <= b2 * x)
    {
        ioEllipsePoints.Insert( octantStartingIndex, FVec2I(iCenter.x - x, iCenter.y + y));

        if (sigma >= 0)
        {
            sigma += fb2 * (1 - x);
            x--;
        }
        sigma += a2 * (4 * y + 6);
        y++;
    }

    octantStartingIndex = static_cast< int >( ioEllipsePoints.Size() );

    //Octant 7 ------
    x = iA;
    y = 0;
    sigma = 2*a2+b2*(-2*iA);

    while (a2 * y <= b2 * x)
    {
        ioEllipsePoints.PushBack(FVec2I(iCenter.x - x, iCenter.y - y));

        if (sigma >= 0)
        {
            sigma += fb2 * (1 - x);
            x--;
        }
        sigma += a2 * (4 * y + 6);
        y++;
    }

    octantStartingIndex = static_cast< int >( ioEllipsePoints.Size() );

    //Octant 8 ------
    x = 0;
    y = iB;
    sigma = 2*b2+a2*(-2*iB);

    while(b2 * x <= a2 * y)
    {
        ioEllipsePoints.Insert( octantStartingIndex, FVec2I(iCenter.x - x,iCenter.y - y));

        if(sigma >= 0)
        {
            sigma += fa2 * (1 - y);
            y--;
        }
        sigma += b2*(4 * x + 6);
        x++;
    }
}

ULIS_NAMESPACE_END

