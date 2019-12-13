// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "Interpolation/OdysseyInterpolationBezier.h"
#include "OdysseyInterpolationTypes.generated.h"

UENUM()
enum class EOdysseyInterpolationType : uint8
{
    kBezier     UMETA( DisplayName="Bezier" ),
    kLine       UMETA( DisplayName="Line" ),
};
