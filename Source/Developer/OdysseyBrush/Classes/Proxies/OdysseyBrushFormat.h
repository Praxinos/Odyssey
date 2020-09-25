// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "CoreTypes.h"
#include "UObject/ObjectMacros.h"
#include <ULIS3>

#include "OdysseyBrushFormat.generated.h"

/////////////////////////////////////////////////////
// EOdysseyBlockFormat Enum
UENUM( BlueprintType )
enum  class  EOdysseyBlockFormat : uint8
{
    kAuto  UMETA( DisplayName="Auto" ),
    kRGBA8 UMETA( DisplayName="RGBA8" ),
    kBGRA8 UMETA( DisplayName="BGRA8" ),
    kGrey8 UMETA( DisplayName="Grey8" )
};

::ul3::tFormat ULISFormatFromOdysseyBlockFormat(EOdysseyBlockFormat iFormat, ::ul3::tFormat iAutoFormat);