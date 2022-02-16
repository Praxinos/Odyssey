// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         CircleInscribedRectangle.h
* @author       Thomas Schmitt
* @brief        This file provides the a method to generate the points of a rectangle inscribed in a circle at a certain angle
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"
#include "Math/ShapeGeneration/Rectangle.h"

ULIS_NAMESPACE_BEGIN

static inline void GenerateCircleInscribedRectanglePoints(
    const FVec2I& iTopLeft
    , const FVec2I& iBottomRight
    , const float iRotationDegrees
    , TArray<FVec2I>& ioRectanglePoints )
{
    if( fmod(iRotationDegrees, 180.f) <= FMath::kEpsilonf )
    {
        GenerateRectanglePoints( iTopLeft, iBottomRight, ioRectanglePoints );
        return;
    }

    FVec2I center = iTopLeft + (iBottomRight - iTopLeft) / 2;
    FVec2I delta = center - iTopLeft;
    double angle = -FMath::RadToDeg( atan2( delta.y, delta.x ) );
    double radius = FMath::Dist( iTopLeft.x, iTopLeft.y, iBottomRight.x, iBottomRight.y ) / 2;

    FVec2I topRight = center + FVec2I(radius * cos(FMath::DegToRad(angle + iRotationDegrees)), radius * sin(FMath::DegToRad(angle + iRotationDegrees)));
    FVec2I bottomLeft = center + FVec2I(radius * cos(FMath::DegToRad(angle + iRotationDegrees + 180)), radius * sin(FMath::DegToRad(angle + iRotationDegrees + 180)));

    GenerateLinePoints( iTopLeft, topRight, ioRectanglePoints ); // Top
    GenerateLinePoints( topRight, iBottomRight, ioRectanglePoints); // Right
    GenerateLinePoints( iBottomRight, bottomLeft, ioRectanglePoints); // Bot
    GenerateLinePoints( bottomLeft, iTopLeft, ioRectanglePoints); // Left
}

ULIS_NAMESPACE_END

