// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "FreehandShape/Interpolation/OdysseyInterpolationBezier.h"
#include "FreehandShape/Interpolation/OdysseyInterpolationCatmullRom.h"
#include "OdysseyInterpolationTypes.generated.h"

UENUM()
enum class EOdysseyInterpolationType : uint8
{
    kBezier     UMETA( DisplayName="Bezier" ),
    kLine       UMETA( DisplayName="Line" ),
    kCatmullRom UMETA( DisplayName="Catmull-Rom" ),
};
