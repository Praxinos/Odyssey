// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"

#include "OdysseyStylusInputDriver.generated.h"


UENUM()
enum EOdysseyStylusInputDriver
{
    OdysseyStylusInputDriver_None       UMETA( DisplayName = "None" ),
#if PLATFORM_WINDOWS
    OdysseyStylusInputDriver_Ink        UMETA( DisplayName = "Ink" ),
    OdysseyStylusInputDriver_Wintab     UMETA( DisplayName = "Wintab" ),
#endif

#if PLATFORM_MAC
    OdysseyStylusInputDriver_NSEvent     UMETA(DisplayName = "NSEvent"),
#endif
};

