// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.

#pragma once

#include "Interpolation/OdysseyInterpolationBezier.h"
#include "OdysseyInterpolationTypes.generated.h"

UENUM()
enum class EOdysseyInterpolationType : uint8
{
    kBezier     UMETA( DisplayName="Bezier" ),
    kLine       UMETA( DisplayName="Line" ),
};
