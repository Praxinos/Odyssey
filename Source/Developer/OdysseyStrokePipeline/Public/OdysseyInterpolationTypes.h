// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "Interpolation/OdysseyInterpolationBezier.h"
#include "Interpolation/OdysseyInterpolationCatmullRom.h"
#include "OdysseyInterpolationTypes.generated.h"

UENUM()
enum class EOdysseyInterpolationType : uint8
{
    kBezier     UMETA( DisplayName="Bezier" ),
    kLine       UMETA( DisplayName="Line" ),
    kCatmullRom UMETA( DisplayName="Catmull-Rom" ),
};
