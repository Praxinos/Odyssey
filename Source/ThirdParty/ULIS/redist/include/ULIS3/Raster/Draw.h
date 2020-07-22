// Copyright 2018-2020 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000
/*
*
*   ULIS3
*__________________
*
* @file         Draw.h
* @author       Clement Berthaud
* @brief        This file provides the declaration for the raster draw entry point functions.
* @copyright    Copyright 2018-2020 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"
#include "Maths/Geometry.h"

ULIS3_NAMESPACE_BEGIN
ULIS3_API void DrawDotNoAA( FBlock* iDst, const FPixelValue& iColor, const FVec2I iPos );
ULIS3_API void DrawHorizontalLineNoAA(  FBlock* iDst, const FPixelValue& iColor, int iX1, int iX2, int iY );
ULIS3_API void DrawVerticalLineNoAA(    FBlock* iDst, const FPixelValue& iColor, int iY1, int iY2, int iX );
ULIS3_API void DrawRectOutlineNoAA( FBlock* iDst, const FPixelValue& iColor, const FRect& iRect );
ULIS3_API void DrawUniformGridOutlineNoAA( FBlock* iDst, const FPixelValue& iColor, const FRect& iRect, int iNumSubdiv );
/*
ULIS3_API void DrawDot();
ULIS3_API void DrawDotNoAA();
ULIS3_API void DrawDotAA();
ULIS3_API void DrawRect();
ULIS3_API void DrawRectNoAA();
ULIS3_API void DrawRectAA();
ULIS3_API void DrawLine();
ULIS3_API void DrawLineNoAA();
ULIS3_API void DrawLineAA();
ULIS3_API void DrawPolyLine();
ULIS3_API void DrawPolyLineNoAA();
ULIS3_API void DrawPolyLineAA();
ULIS3_API void DrawPolygon();
ULIS3_API void DrawPolygonNoAA();
ULIS3_API void DrawPolygonAA();
ULIS3_API void DrawCircleAndres();
ULIS3_API void DrawCircleAndresNoAA();
ULIS3_API void DrawCircleAndresAA();
ULIS3_API void DrawCircleBresenham();
ULIS3_API void DrawCircleBresenhamNoAA();
ULIS3_API void DrawCircleBresenhamAA();
ULIS3_API void DrawCircleParametric();
ULIS3_API void DrawCircleParametricNoAA();
ULIS3_API void DrawCircleParametricAA();
ULIS3_API void DrawDiskAndres();
ULIS3_API void DrawDiskAndresNoAA();
ULIS3_API void DrawDiskAndresAA();
ULIS3_API void DrawDiskBresenham();
ULIS3_API void DrawDiskBresenhamNoAA();
ULIS3_API void DrawDiskBresenhamAA();
ULIS3_API void DrawDiskParametric();
ULIS3_API void DrawDiskParametricNoAA();
ULIS3_API void DrawDiskParametricAA();
ULIS3_API void DrawEllipseRational();
ULIS3_API void DrawEllipseRationalNoAA();
ULIS3_API void DrawEllipseRationalAA();
ULIS3_API void DrawEllipseParametric();
ULIS3_API void DrawEllipseParametricNoAA();
ULIS3_API void DrawEllipseParametricAA();
ULIS3_API void DrawFilledEllipseRational();
ULIS3_API void DrawFilledEllipseRationalNoAA();
ULIS3_API void DrawFilledEllipseRationalAA();
ULIS3_API void DrawFilledEllipseParametric();
ULIS3_API void DrawFilledEllipseParametricNoAA();
ULIS3_API void DrawFilledEllipseParametricAA();
ULIS3_API void DrawBezierQuadraticParametric();
ULIS3_API void DrawBezierQuadraticParametricNoAA();
ULIS3_API void DrawBezierQuadraticParametricAA();
ULIS3_API void DrawBezierQuadraticRational();
ULIS3_API void DrawBezierQuadraticRationalNoAA();
ULIS3_API void DrawBezierQuadraticRationalAA();
ULIS3_API void DrawBezierCubicRational();
ULIS3_API void DrawBezierCubicRationalNoAA();
ULIS3_API void DrawBezierCubicRationalAA();
ULIS3_API void DrawTriangleBarycentric();
ULIS3_API void DrawTriangleBarycentricNoAA();
ULIS3_API void DrawTriangleBarycentricAA();
ULIS3_API void DrawTrianglePoly();
ULIS3_API void DrawTrianglePolyNoAA();
ULIS3_API void DrawTrianglePolyAA();
*/
ULIS3_NAMESPACE_END

