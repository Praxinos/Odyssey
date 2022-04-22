// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"

#include "OdysseySmoothingTypes.generated.h"

UENUM()
enum class EOdysseySmoothingMethod : uint8
{
    kAverage    UMETA( DisplayName="Average" ),
    kPull       UMETA( DisplayName="Pull" )
    // kGravity    UMETA( DisplayName="Gravity" ),
};
