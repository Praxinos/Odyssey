// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "OdysseyInterpolationTypes.generated.h"

UENUM()
enum class EOdysseyInterpolationType : uint8
{
    kBezier     UMETA( DisplayName="Bezier" ),
    kLine       UMETA( DisplayName="Line" ),
    kCatmullRom UMETA( DisplayName="Catmull-Rom" ),
};
