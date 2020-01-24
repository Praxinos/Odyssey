// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"

#include "OdysseyStylusInputDriver.generated.h"


UENUM()
enum EOdysseyStylusInputDriver
{
    OdysseyStylusInputDriver_None       UMETA( DisplayName = "None" ),
    OdysseyStylusInputDriver_NSEvent     UMETA( DisplayName = "NSEvent" ),
};

