// IDDN FR.001.250001.004.S.X.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         RotatedEllipse.h
* @author       Thomas Schmitt
* @brief        This file provides the Rotated Ellipse points generation methods
* @copyright    Copyright 2018-2021 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"

ULIS_NAMESPACE_BEGIN

static inline void GenerateRotatedEllipsePoints(
      const FVec2I&            iCenter
    , const int                iA
    , const int                iB
    , const int                iRotationDegrees
    , TArray<FVec2I>&          ioRotatedEllipsePoints )
{
    if (iA <= 0 || iB <= 0)
        return;

    if(iRotationDegrees % 180 == 0)
    {
        GenerateEllipsePoints(iCenter,iA,iB,ioRotatedEllipsePoints); //Don't bother to use the rotated ellipse algorithm if the ellipse is not rotated
        return;
    }
    if(iRotationDegrees % 90 == 0)
    {
        GenerateEllipsePoints(iCenter,iB,iA,ioRotatedEllipsePoints); //Don't bother to use the rotated ellipse algorithm if the ellipse is not rotated
        return;
    }

    int a = iA;
    int b = iB;

    float dx = (float)iA*iA;
    float dy = (float)iB*iB;
    float s = std::sin(FMath::DegToRadF(float(iRotationDegrees)));
    float dz = (dx - dy) * s;
    dx = std::sqrt(dx - dz * s);
    dy = std::sqrt(dy + dz * s);
    a = int(dx + 0.5f);
    b = int(dy + 0.5f);
    dz = dz * a * b / (dx * dy);

    int x0 = iCenter.x - a;
    int y0 = iCenter.y - b;
    int x1 = iCenter.x + a;
    int y1 = iCenter.y + b;
    dz = (4.f * dz * std::cos(FMath::DegToRadF(float(iRotationDegrees))));


    dx = float(x1 - x0);
    dy = float(y1 - y0);
    float w = dx * dy;
    if(w != 0.0)
        w = (w - dz) / (w + w);

    if(w > 1 || w < 0)
        return;

    dx = std::floor(dx * w + 0.5f);
    dy = std::floor(dy * w + 0.5f);

    InternalGenerateQuadraticBezierSegPoints(x1, int(y1 - dy), x1, y0, int(x0 + dx), y0, w, ioRotatedEllipsePoints, true, ioRotatedEllipsePoints.Size()); //1st quadrant
    InternalGenerateQuadraticBezierSegPoints(x1, int(y1 - dy), x1, y1, int(x1 - dx), y1, 1.f - w, ioRotatedEllipsePoints, false, ioRotatedEllipsePoints.Size() ); //2nd quadrant
    InternalGenerateQuadraticBezierSegPoints(x0, int(y0 + dy), x0, y1, int(x1 - dx), y1, w, ioRotatedEllipsePoints, true, ioRotatedEllipsePoints.Size()); // 3rd quadrant
    InternalGenerateQuadraticBezierSegPoints(x0, int(y0 + dy), x0, y0, int(x0 + dx), y0, 1.f - w, ioRotatedEllipsePoints, false, ioRotatedEllipsePoints.Size()); // 4th quadrant
}

ULIS_NAMESPACE_END

