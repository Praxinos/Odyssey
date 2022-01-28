// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         Circle.h
* @author       Thomas Schmitt
* @brief        This file provides the Circle points generation methods
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"

ULIS_NAMESPACE_BEGIN

static inline void GenerateCirclePoints(
      const FVec2I&            iCenter
    , const int                iRadius
    , TArray<FVec2I>&          ioCirclePoints )
{
    int x = 0;
    int y = iRadius; //We start from the top of the circle for the first octant
    int octantStartingIndex = 0;

    //0° is on top and we turn clockwise
    //Octant 1 ------
    int diff = iRadius;

    while (y >= x)
    {
        if (diff < (2 * (iRadius - y)))
        {
            diff += (2 * y - 1);
            y--;
        }

        ioCirclePoints.PushBack(FVec2I(iCenter.x + x, iCenter.y - y));

        if (diff >= (2 * x))
        {
            diff -= (2 * x + 1);
            x++;
        }
        else
        {
            diff += (2 * (y - x - 1));
            y--;
            x++;
        }
    }
    
    octantStartingIndex = static_cast< int >( ioCirclePoints.Size() );

    //Octant 2 ------
    x = 0;
    y = iRadius;
    diff = iRadius;

    while (y >= x)
    {
        if ( diff < ( 2 * ( iRadius - y ) ) )
        {
            diff += ( 2 * y - 1 );
            y--;
        }

        ioCirclePoints.Insert( octantStartingIndex, FVec2I( iCenter.x + y, iCenter.y - x ));

        if( diff >= ( 2 * x ) )
        {
            diff -= ( 2 * x + 1 );
            x++;
        }
        else
        {
            diff += (2 * ( y - x - 1 ) );
            y--;
            x++;
        }
    }

    octantStartingIndex = static_cast< int >( ioCirclePoints.Size() );

    //Octant 3 ------
    x = 0;
    y = iRadius;
    diff = iRadius;

    while (y >= x)
    {
        if ( diff < ( 2 * ( iRadius - y ) ) )
        {
            diff += ( 2 * y - 1 );
            y--;
        }

        ioCirclePoints.PushBack( FVec2I( iCenter.x + y, iCenter.y + x ));

        if( diff >= ( 2 * x ) )
        {
            diff -= ( 2 * x + 1 );
            x++;
        }
        else
        {
            diff += (2 * ( y - x - 1 ) );
            y--;
            x++;
        }
    }

    octantStartingIndex = static_cast< int >( ioCirclePoints.Size() );

    //Octant 4 ------
    x = 0;
    y = iRadius;
    diff = iRadius;

    while (y >= x)
    {
        if ( diff < ( 2 * ( iRadius - y ) ) )
        {
            diff += ( 2 * y - 1 );
            y--;
        }

        ioCirclePoints.Insert( octantStartingIndex, FVec2I( iCenter.x + x, iCenter.y + y ));

        if( diff >= ( 2 * x ) )
        {
            diff -= ( 2 * x + 1 );
            x++;
        }
        else
        {
            diff += (2 * ( y - x - 1 ) );
            y--;
            x++;
        }
    }

    octantStartingIndex = static_cast< int >( ioCirclePoints.Size() );

    //Octant 5 ------
    x = 0;
    y = iRadius;
    diff = iRadius;

    while (y > x)
    {
        if ( diff < ( 2 * ( iRadius - y ) ) )
        {
            diff += ( 2 * y - 1 );
            y--;
        }

        ioCirclePoints.PushBack( FVec2I( iCenter.x - x, iCenter.y + y ));

        if( diff >= ( 2 * x ) )
        {
            diff -= ( 2 * x + 1 );
            x++;
        }
        else
        {
            diff += (2 * ( y - x - 1 ) );
            y--;
            x++;
        }
    }

    octantStartingIndex = static_cast< int >( ioCirclePoints.Size() );

    //Octant 6 ------
    x = 0;
    y = iRadius;
    diff = iRadius;

    while (y > x)
    {
        if ( diff < ( 2 * ( iRadius - y ) ) )
        {
            diff += ( 2 * y - 1 );
            y--;
        }

        ioCirclePoints.Insert( octantStartingIndex, FVec2I( iCenter.x - y, iCenter.y + x ));

        if( diff >= ( 2 * x ) )
        {
            diff -= ( 2 * x + 1 );
            x++;
        }
        else
        {
            diff += (2 * ( y - x - 1 ) );
            y--;
            x++;
        }
    }

    octantStartingIndex = static_cast< int >( ioCirclePoints.Size() );

    //Octant 7 ------
    x = 0;
    y = iRadius;
    diff = iRadius;

    while (y > x)
    {
        if ( diff < ( 2 * ( iRadius - y ) ) )
        {
            diff += ( 2 * y - 1 );
            y--;
        }

        ioCirclePoints.PushBack( FVec2I( iCenter.x - y, iCenter.y - x ));

        if( diff >= ( 2 * x ) )
        {
            diff -= ( 2 * x + 1 );
            x++;
        }
        else
        {
            diff += (2 * ( y - x - 1 ) );
            y--;
            x++;
        }
    }

    octantStartingIndex = static_cast< int >( ioCirclePoints.Size() );

    //Octant 8 ------
    x = 0;
    y = iRadius;
    diff = iRadius;

    while (y > x)
    {
        if ( diff < ( 2 * ( iRadius - y ) ) )
        {
            diff += ( 2 * y - 1 );
            y--;
        }

        ioCirclePoints.Insert( octantStartingIndex, FVec2I( iCenter.x - x, iCenter.y - y) );

        if( diff >= ( 2 * x ) )
        {
            diff -= ( 2 * x + 1 );
            x++;
        }
        else
        {
            diff += (2 * ( y - x - 1 ) );
            y--;
            x++;
        }
    }
}

ULIS_NAMESPACE_END

