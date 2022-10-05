// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"

#include "OdysseyStylusInputDriver.generated.h"

// The available stylus drivers
UENUM()
enum EOdysseyStylusInputDriver
{
    OdysseyStylusInputDriver_None       UMETA( DisplayName = "None" ),
    OdysseyStylusInputDriver_Ink        UMETA( DisplayName = "Ink (Windows)" ),
    OdysseyStylusInputDriver_Wintab     UMETA( DisplayName = "Wintab (Windows)" ),
    OdysseyStylusInputDriver_NSEvent    UMETA( DisplayName = "NSEvent (MacOS)" ),
};
