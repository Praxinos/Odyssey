// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"

#include "OdysseyStylusInputDriver.generated.h"

// The available stylus drivers
UENUM()
enum EOdysseyStylusInputDriver
{
    OdysseyStylusInputDriver_None               UMETA( DisplayName = "None" ),
    OdysseyStylusInputDriver_Ink                UMETA( DisplayName = "Ink (Windows)" ),
    OdysseyStylusInputDriver_Wintab             UMETA( DisplayName = "Wintab (Windows)" ),
    OdysseyStylusInputDriver_NativeWindows      UMETA( DisplayName = "Native (Windows)" ),
    OdysseyStylusInputDriver_NSEvent            UMETA( DisplayName = "NSEvent (MacOS)" ),
};
