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
    kRGB UMETA( DisplayName="RGB" ),
    kRGBA UMETA( DisplayName="RGBA" ),
    kGrey UMETA( DisplayName="Grey" ),
    kGreyA UMETA( DisplayName="GreyA" ),
    kHSL UMETA( DisplayName="HSL" ),
    kHSLA UMETA( DisplayName="HSLA" ),
    kHSV UMETA( DisplayName="HSV" ),
    kHSVA UMETA( DisplayName="HSVA" ),
    kLab UMETA( DisplayName="Lab" ),
    kLabA UMETA( DisplayName="LabA" )
};

/////////////////////////////////////////////////////
// EOdysseyBlockFormat Enum
UENUM( BlueprintType )
enum  class  EOdysseyBlockFormatPrecision : uint8
{
    kAuto  UMETA( DisplayName="Auto" ),
    k8 UMETA( DisplayName="8 bits" ),
    k16 UMETA( DisplayName="16 bits" ),
    k32 UMETA( DisplayName="32 bits" ),
    kFloat UMETA( DisplayName="32 bits floating-point" ),
    kDouble UMETA( DisplayName="64 bits floating-point" )
};

::ul3::tFormat ULISFormatFromOdysseyBlockFormat(EOdysseyBlockFormat iFormat, EOdysseyBlockFormatPrecision iPrecision, ::ul3::tFormat iAutoFormat);