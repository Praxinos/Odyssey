// IDDN FR.001.250001.004.S.X.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         Arc.h
* @author       Thomas Schmitt
* @brief        This file provides the Arc points generation methods
* @copyright    Copyright 2018-2021 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"

ULIS_NAMESPACE_BEGIN


static inline void GenerateArcPoints(
    const FVec2I& iCenter
    , const int iRadius
    , const int iStartDegree
    , const int iEndDegree
    , TArray<FVec2I>& ioArcPoints )
{
    if(iRadius == 0)
        return;

    int x = 0;
    int y = iRadius; //We start from the top of the circle for the first octant
    int octantStartingIndex = 0;

    int sizeAngleToDraw = (iEndDegree - iStartDegree + 360) % 360; //Positive modulo
    int currentAngle = iStartDegree;

    int octantsToDraw[8] ={0,0,0,0,0,0,0,0}; // 0: Don't draw the octant. 1: draw fully the octant. 2: draw part of the octant
    int directionToDraw[8][2] ={{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}}; // 1 clockwise, -1 anti-clockwise, 2 both from inside, 3 both from outside, 0 irrelevant, second entry is angle to draw on octant

    if(sizeAngleToDraw < 45 && iStartDegree / 45 == iEndDegree / 45)
    {
        octantsToDraw[currentAngle / 45] = 2;
        directionToDraw[currentAngle / 45][0] = 2;
        directionToDraw[currentAngle / 45][1] = sizeAngleToDraw;
    } 
    else if(currentAngle % 45 == 0)
    {
        octantsToDraw[currentAngle / 45] = 1;
    }
    else
    {
        octantsToDraw[currentAngle / 45] = 2;
        directionToDraw[currentAngle / 45][0] = -1;
        directionToDraw[currentAngle / 45][1] = 45 - (currentAngle % 45);
    }

    sizeAngleToDraw -= (45 - (iStartDegree % 45));

    while(sizeAngleToDraw >= 45)
    {
        currentAngle = (currentAngle + 45) % 360;
        octantsToDraw[currentAngle / 45] = 1;
        sizeAngleToDraw -= 45;
    }

    if(sizeAngleToDraw > 0)
    {
        currentAngle = (currentAngle + 45) % 360;
        octantsToDraw[currentAngle / 45] = 2;
        directionToDraw[currentAngle / 45][1] = sizeAngleToDraw;

        if (directionToDraw[currentAngle / 45][0] == 0)
            directionToDraw[currentAngle / 45][0] = 1;
        else
            directionToDraw[currentAngle / 45][0] = 3;
    }

    bool startPlotting = false; //We start when we find an octant not fully drawn or octants we don't draw at all
    for (int numLoop = 0; numLoop < 2; numLoop++) //We loop twice: once for positionning ourselves at the right octant and start drawing, and the second time to draw what we didn't during the positionning
    {
        //0� is on top and we turn clockwise
        //Octant 1 ------
        x = 0;
        y = iRadius;
        int diff = iRadius;

        if( octantsToDraw[0] != 0 )
        {
            while(y >= x)
            {
                if (diff < (2 * (iRadius - y)))
                {
                    diff += (2 * y - 1);
                    y--;
                }

                if(octantsToDraw[0] == 1 && startPlotting) 
                {
                    ioArcPoints.PushBack( FVec2I( iCenter.x + x, iCenter.y - y ));
                }
                else if(octantsToDraw[0] == 2) //complex case
                {
                    double currentAngleOnFirstOctant = -::ULIS::FMath::RadToDeg(std::acos(double(x) / double(iRadius)) - (FMath::kPId / 2));

                    if (directionToDraw[0][0] == 1 && currentAngleOnFirstOctant < directionToDraw[0][1] && startPlotting) ioArcPoints.PushBack(FVec2I(iCenter.x + x, iCenter.y - y));
                    else if (directionToDraw[0][0] == -1 && 45 - currentAngleOnFirstOctant < directionToDraw[0][1]) ioArcPoints.PushBack(FVec2I(iCenter.x + x, iCenter.y - y));
                    else if (directionToDraw[0][0] == 2 && currentAngleOnFirstOctant > (iStartDegree % 45) && currentAngleOnFirstOctant < (iEndDegree % 45)) ioArcPoints.PushBack(FVec2I(iCenter.x + x, iCenter.y - y));
                    else if (directionToDraw[0][0] == 3 && currentAngleOnFirstOctant < (iEndDegree % 45) && startPlotting) ioArcPoints.PushBack(FVec2I(iCenter.x + x, iCenter.y - y));
                    else if (directionToDraw[0][0] == 3 && currentAngleOnFirstOctant > (iStartDegree % 45) && !startPlotting) ioArcPoints.PushBack(FVec2I(iCenter.x + x, iCenter.y - y));
                }

                if(diff >= (2 * x))
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
        }

        //If we did draw the full octant, we don't draw it on second loop
        if ((octantsToDraw[0] == 1 || directionToDraw[0][0] == 1 ) && startPlotting)
        {
            octantsToDraw[0] = 0;
        }

        //If we did draw the part of the octant which we needed to draw, we don't draw it anymore. Except in the case directionToDraw == 3. Plus, we start the plot for the rest of the arc
        if( octantsToDraw[0] != 1 )
        {
            startPlotting = true;
            if( directionToDraw[0][0] == -1 || directionToDraw[0][0] == 2 )
            {
                octantsToDraw[0] = 0;
            }
        }



        octantStartingIndex = ioArcPoints.Size();

        //Octant 2 ------
        x = 0;
        y = iRadius;
        diff = iRadius;

        if (octantsToDraw[1] != 0)
        {
            while(y >= x)
            {
                if (diff < (2 * (iRadius - y)))
                {
                    diff += (2 * y - 1);
                    y--;
                }

                if(octantsToDraw[1] == 1 && startPlotting ) 
                {
                    ioArcPoints.Insert( octantStartingIndex, FVec2I( iCenter.x + y, iCenter.y - x ));
                }
                else if(octantsToDraw[1] == 2)
                {
                    double currentAngleOnFirstOctant = -::ULIS::FMath::RadToDeg(std::acos(double(x) / double(iRadius)) - (FMath::kPId / 2));

                    if(directionToDraw[1][0] == 1 && 45 - currentAngleOnFirstOctant < directionToDraw[1][1] && startPlotting ) ioArcPoints.Insert( octantStartingIndex, FVec2I( iCenter.x + y, iCenter.y - x ));
                    else if(directionToDraw[1][0] == -1 && currentAngleOnFirstOctant < directionToDraw[1][1]) ioArcPoints.Insert( octantStartingIndex, FVec2I( iCenter.x + y, iCenter.y - x ));
                    else if(directionToDraw[1][0] == 2 && 45 - currentAngleOnFirstOctant > (iStartDegree % 45) && 45 - currentAngleOnFirstOctant < (iEndDegree % 45)) ioArcPoints.Insert( octantStartingIndex, FVec2I( iCenter.x + y, iCenter.y - x ));
                    else if (directionToDraw[1][0] == 3 && 45 - currentAngleOnFirstOctant < (iEndDegree % 45) && startPlotting) ioArcPoints.Insert( octantStartingIndex, FVec2I( iCenter.x + y, iCenter.y - x ));
                    else if (directionToDraw[1][0] == 3 && 45 - currentAngleOnFirstOctant > (iStartDegree % 45) && !startPlotting) ioArcPoints.Insert(octantStartingIndex, FVec2I(iCenter.x + y, iCenter.y - x));
                }

                if(diff >= (2 * x))
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
        }

        if ((octantsToDraw[1] == 1 || directionToDraw[1][0] == 1) && startPlotting)
        {
            octantsToDraw[1] = 0;
        }
        
        if (octantsToDraw[1] != 1)
        {
            startPlotting = true;
            if (directionToDraw[1][0] == -1 || directionToDraw[1][0] == 2)
            {
                octantsToDraw[1] = 0;
            }
        }

        octantStartingIndex = ioArcPoints.Size();

        //Octant 3 ------
        x = 0;
        y = iRadius;
        diff = iRadius;

        if (octantsToDraw[2] != 0)
        {
            while(y >= x)
            {
                if (diff < (2 * (iRadius - y)))
                {
                    diff += (2 * y - 1);
                    y--;
                }

                if(octantsToDraw[2] == 1 && startPlotting ) 
                {
                    ioArcPoints.PushBack( FVec2I(iCenter.x + y, iCenter.y + x) );
                }
                else if(octantsToDraw[2] == 2)
                {
                    double currentAngleOnFirstOctant = -::ULIS::FMath::RadToDeg(std::acos(double(x) / double(iRadius)) - (FMath::kPId / 2));

                    if(directionToDraw[2][0] == 1 && currentAngleOnFirstOctant < directionToDraw[2][1] && startPlotting ) ioArcPoints.PushBack( FVec2I(iCenter.x + y, iCenter.y + x) );
                    else if(directionToDraw[2][0] == -1 && 45 - currentAngleOnFirstOctant < directionToDraw[2][1]) ioArcPoints.PushBack( FVec2I(iCenter.x + y, iCenter.y + x) );
                    else if(directionToDraw[2][0] == 2 && currentAngleOnFirstOctant > (iStartDegree % 45) && currentAngleOnFirstOctant < (iEndDegree % 45)) ioArcPoints.PushBack( FVec2I(iCenter.x + y, iCenter.y + x) );
                    else if (directionToDraw[2][0] == 3 && currentAngleOnFirstOctant < (iEndDegree % 45) && startPlotting) ioArcPoints.PushBack( FVec2I(iCenter.x + y, iCenter.y + x) );
                    else if (directionToDraw[2][0] == 3 && currentAngleOnFirstOctant > (iStartDegree % 45) && !startPlotting) ioArcPoints.PushBack(FVec2I(iCenter.x + y, iCenter.y + x));
                }

                if(diff >= (2 * x))
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
        }

        if ((octantsToDraw[2] == 1 || directionToDraw[2][0] == 1) && startPlotting)
        {
            octantsToDraw[2] = 0;
        }

        if (octantsToDraw[2] != 1)
        {
            startPlotting = true;
            if (directionToDraw[2][0] == -1 || directionToDraw[2][0] == 2 )
            {
                octantsToDraw[2] = 0;
            }
        }

        octantStartingIndex = ioArcPoints.Size();

        //Octant 4 ------
        x = 0;
        y = iRadius;
        diff = iRadius;

        if (octantsToDraw[3] != 0)
        {
            while(y >= x)
            {
                if (diff < (2 * (iRadius - y)))
                {
                    diff += (2 * y - 1);
                    y--;
                }

                if(octantsToDraw[3] == 1 && startPlotting )
                {
                    ioArcPoints.Insert( octantStartingIndex, FVec2I( iCenter.x + x, iCenter.y + y ));
                }
                else if(octantsToDraw[3] == 2)
                {
                    double currentAngleOnFirstOctant = -::ULIS::FMath::RadToDeg(std::acos(double(x) / double(iRadius)) - (FMath::kPId / 2));

                    if(directionToDraw[3][0] == 1 && 45 - currentAngleOnFirstOctant < directionToDraw[3][1] && startPlotting ) ioArcPoints.Insert( octantStartingIndex, FVec2I( iCenter.x + x, iCenter.y + y ));
                    else if(directionToDraw[3][0] == -1 && currentAngleOnFirstOctant < directionToDraw[3][1]) ioArcPoints.Insert( octantStartingIndex, FVec2I( iCenter.x + x, iCenter.y + y ));
                    else if(directionToDraw[3][0] == 2 && 45 - currentAngleOnFirstOctant > (iStartDegree % 45) && 45 - currentAngleOnFirstOctant < (iEndDegree % 45)) ioArcPoints.Insert( octantStartingIndex, FVec2I( iCenter.x + x, iCenter.y + y ));
                    else if (directionToDraw[3][0] == 3 && 45 - currentAngleOnFirstOctant < (iEndDegree % 45) && startPlotting) ioArcPoints.Insert( octantStartingIndex, FVec2I( iCenter.x + x, iCenter.y + y ));
                    else if (directionToDraw[3][0] == 3 && 45 - currentAngleOnFirstOctant > (iStartDegree % 45) && !startPlotting) ioArcPoints.Insert(octantStartingIndex, FVec2I(iCenter.x + x, iCenter.y + y));
                }

                if(diff >= (2 * x))
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
        }

        if ((octantsToDraw[3] == 1 || directionToDraw[3][0] == 1) && startPlotting)
        {
            octantsToDraw[3] = 0;
        }

        if (octantsToDraw[3] != 1)
        {
            startPlotting = true;
            if (directionToDraw[3][0] == -1 || directionToDraw[3][0] == 2)
            {
                octantsToDraw[3] = 0;
            }
        }

        octantStartingIndex = ioArcPoints.Size();

        //Octant 5 ------
        x = 0;
        y = iRadius;
        diff = iRadius;

        if (octantsToDraw[4] != 0)
        {
            while(y >= x)
            {
                if (diff < (2 * (iRadius - y)))
                {
                    diff += (2 * y - 1);
                    y--;
                }

                if(octantsToDraw[4] == 1 && startPlotting )
                {
                    ioArcPoints.PushBack( FVec2I( iCenter.x - x, iCenter.y + y ));
                }
                else if(octantsToDraw[4] == 2)
                {
                    double currentAngleOnFirstOctant = -::ULIS::FMath::RadToDeg(std::acos(double(x) / double(iRadius)) - (FMath::kPId / 2));

                    if(directionToDraw[4][0] == 1 && currentAngleOnFirstOctant < directionToDraw[4][1] && startPlotting ) ioArcPoints.PushBack( FVec2I( iCenter.x - x, iCenter.y + y ));
                    else if(directionToDraw[4][0] == -1 && 45 - currentAngleOnFirstOctant < directionToDraw[4][1]) ioArcPoints.PushBack( FVec2I( iCenter.x - x, iCenter.y + y ));
                    else if(directionToDraw[4][0] == 2 && currentAngleOnFirstOctant > (iStartDegree % 45) && currentAngleOnFirstOctant < (iEndDegree % 45)) ioArcPoints.PushBack( FVec2I( iCenter.x - x, iCenter.y + y ));
                    else if (directionToDraw[4][0] == 3 && currentAngleOnFirstOctant < (iEndDegree % 45) && startPlotting) ioArcPoints.PushBack( FVec2I( iCenter.x - x, iCenter.y + y ));
                    else if (directionToDraw[4][0] == 3 && currentAngleOnFirstOctant > (iStartDegree % 45) && !startPlotting) ioArcPoints.PushBack(FVec2I(iCenter.x - x, iCenter.y + y));
                }

                if(diff >= (2 * x))
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
        }

        if ((octantsToDraw[4] == 1 || directionToDraw[4][0] == 1) && startPlotting)
        {
            octantsToDraw[4] = 0;
        }

        if (octantsToDraw[4] != 1)
        {
            startPlotting = true;
            if (directionToDraw[4][0] == -1 || directionToDraw[4][0] == 2)
            {
                octantsToDraw[4] = 0;
            }
        }

        octantStartingIndex = ioArcPoints.Size();

        //Octant 6 ------
        x = 0;
        y = iRadius;
        diff = iRadius;


        if (octantsToDraw[5] != 0)
        {
            while (y >= x)
            {
                if (diff < (2 * (iRadius - y)))
                {
                    diff += (2 * y - 1);
                    y--;
                }

                if (octantsToDraw[5] == 1 && startPlotting )
                {
                    ioArcPoints.Insert(octantStartingIndex, FVec2I(iCenter.x - y, iCenter.y + x));
                }
                else if (octantsToDraw[5] == 2)
                {
                    double currentAngleOnFirstOctant = -::ULIS::FMath::RadToDeg(std::acos(double(x) / double(iRadius)) - (FMath::kPId / 2));

                    if (directionToDraw[5][0] == 1 && 45 - currentAngleOnFirstOctant < directionToDraw[5][1] && startPlotting ) ioArcPoints.Insert(octantStartingIndex, FVec2I(iCenter.x - y, iCenter.y + x));
                    else if (directionToDraw[5][0] == -1 && currentAngleOnFirstOctant < directionToDraw[5][1]) ioArcPoints.Insert(octantStartingIndex, FVec2I(iCenter.x - y, iCenter.y + x));
                    else if (directionToDraw[5][0] == 2 && 45 - currentAngleOnFirstOctant < (iEndDegree % 45) && startPlotting) ioArcPoints.Insert(octantStartingIndex, FVec2I(iCenter.x - y, iCenter.y + x));
                    else if (directionToDraw[5][0] == 3 && 45 - currentAngleOnFirstOctant > (iStartDegree % 45) && !startPlotting) ioArcPoints.Insert(octantStartingIndex, FVec2I(iCenter.x - y, iCenter.y + x));
                }

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
        }

        if ((octantsToDraw[5] == 1 || directionToDraw[5][0] == 1) && startPlotting)
        {
            octantsToDraw[5] = 0;
        }

        if (octantsToDraw[5] != 1)
        {
            startPlotting = true;
            if (directionToDraw[5][0] == -1 || directionToDraw[5][0] == 2 )
            {
                octantsToDraw[5] = 0;
            }
        }

        octantStartingIndex = ioArcPoints.Size();

        //Octant 7 ------
        x = 0;
        y = iRadius;
        diff = iRadius;

        if (octantsToDraw[6] != 0)
        {
            while(y >= x)
            {
                if (diff < (2 * (iRadius - y)))
                {
                    diff += (2 * y - 1);
                    y--;
                }

                if(octantsToDraw[6] == 1 && startPlotting ) 
                {
                    ioArcPoints.PushBack( FVec2I( iCenter.x - y, iCenter.y - x ));
                }
                else if(octantsToDraw[6] == 2)
                {
                    double currentAngleOnFirstOctant = -::ULIS::FMath::RadToDeg(std::acos(double(x) / double(iRadius)) - (FMath::kPId / 2));

                    if(directionToDraw[6][0] == 1 && currentAngleOnFirstOctant < directionToDraw[6][1] && startPlotting ) ioArcPoints.PushBack( FVec2I( iCenter.x - y, iCenter.y - x ));
                    else if(directionToDraw[6][0] == -1 && 45 - currentAngleOnFirstOctant < directionToDraw[6][1]) ioArcPoints.PushBack( FVec2I( iCenter.x - y, iCenter.y - x ));
                    else if(directionToDraw[6][0] == 2 && currentAngleOnFirstOctant > (iStartDegree % 45) && currentAngleOnFirstOctant < (iEndDegree % 45)) ioArcPoints.PushBack( FVec2I( iCenter.x - y, iCenter.y - x ));
                    else if (directionToDraw[6][0] == 3 && currentAngleOnFirstOctant < (iEndDegree % 45) && startPlotting) ioArcPoints.PushBack( FVec2I( iCenter.x - y, iCenter.y - x ));
                    else if (directionToDraw[6][0] == 3 && currentAngleOnFirstOctant > (iStartDegree % 45) && !startPlotting) ioArcPoints.PushBack(FVec2I(iCenter.x - y, iCenter.y - x));
                }

                if(diff >= (2 * x))
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
        }

        if ((octantsToDraw[6] == 1 || directionToDraw[6][0] == 1) && startPlotting)
        {
            octantsToDraw[6] = 0;
        }

        if (octantsToDraw[6] != 1)
        {
            startPlotting = true;
            if (directionToDraw[6][0] == -1 || directionToDraw[6][0] == 2)
            {
                octantsToDraw[6] = 0;
            }
        }

        octantStartingIndex = ioArcPoints.Size();

        //Octant 8 ------
        x = 0;
        y = iRadius;
        diff = iRadius;

        if (octantsToDraw[7] != 0)
        {
            while (y >= x)
            {
                if (diff < (2 * (iRadius - y)))
                {
                    diff += (2 * y - 1);
                    y--;
                }

                if (octantsToDraw[7] == 1 && startPlotting )
                {
                    ioArcPoints.Insert(octantStartingIndex, FVec2I(iCenter.x - x, iCenter.y - y));
                }
                else if (octantsToDraw[7] == 2)
                {
                    double currentAngleOnFirstOctant = -::ULIS::FMath::RadToDeg(std::acos(double(x) / double(iRadius)) - (FMath::kPId / 2));

                    if (directionToDraw[7][0] == 1 && 45 - currentAngleOnFirstOctant < directionToDraw[7][1] && startPlotting ) ioArcPoints.Insert(octantStartingIndex, FVec2I(iCenter.x - x, iCenter.y - y));
                    else if (directionToDraw[7][0] == -1 && currentAngleOnFirstOctant < directionToDraw[7][1]) ioArcPoints.Insert(octantStartingIndex, FVec2I(iCenter.x - x, iCenter.y - y));
                    else if (directionToDraw[7][0] == 2 && 45 - currentAngleOnFirstOctant < (iEndDegree % 45) && startPlotting) ioArcPoints.Insert(octantStartingIndex, FVec2I(iCenter.x - x, iCenter.y - y));
                    else if (directionToDraw[7][0] == 3 && 45 - currentAngleOnFirstOctant > (iStartDegree % 45) && !startPlotting) ioArcPoints.Insert(octantStartingIndex, FVec2I(iCenter.x - x, iCenter.y - y));
                }

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
        }

        if ((octantsToDraw[7] == 1 || directionToDraw[7][0] == 1) && startPlotting)
        {
            octantsToDraw[7] = 0;
        }

        if (octantsToDraw[7] != 1)
        {
            startPlotting = true;
            if (directionToDraw[7][0] == -1 || directionToDraw[7][0] == 2 )
            {
                octantsToDraw[7] = 0;
            }
        }

        startPlotting = true;
    }
}

ULIS_NAMESPACE_END

