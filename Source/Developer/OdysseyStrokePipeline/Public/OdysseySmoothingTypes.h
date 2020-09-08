// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "Smoothing/OdysseySmoothingAverage.h"
#include "OdysseySmoothingTypes.generated.h"

UENUM()
enum class EOdysseySmoothingMethod : uint8
{
    kAverage    UMETA( DisplayName="Average" ),
    kPull       UMETA( DisplayName="Pull" ),
    kGravity    UMETA( DisplayName="Gravity" ),
};
