// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreTypes.h"
#include "UObject/ObjectMacros.h"
#include <ULIS>

#include "OdysseyBrushFormat.generated.h"

/////////////////////////////////////////////////////
// EOdysseyColorModel Enum
UENUM( BlueprintType )
enum  class  EOdysseyColorModel : uint8
{
    kUninitializedColorModel UMETA( DisplayName="kUninitializedColorModel", Hidden ),
    kRGB UMETA( DisplayName="RGB" ),
    kRGBA UMETA(DisplayName = "RGBA"),
    kGrey UMETA( DisplayName="Grey" ),
    kGreyA UMETA(DisplayName = "GreyA"),
    kHSL UMETA( DisplayName="HSL" ),
    kHSLA UMETA(DisplayName = "HSLA"),
    kHSV UMETA( DisplayName="HSV" ),
    kHSVA UMETA(DisplayName = "HSVA"),
	kCMYK UMETA(DisplayName = "CMYK"),
    kCMYKA UMETA(DisplayName = "CMYKA"),
    kLab UMETA( DisplayName="Lab" ),
    kLabA UMETA(DisplayName = "LabA")
};

/////////////////////////////////////////////////////
// EOdysseyColorModel Enum
UENUM( BlueprintType )
enum  class  EOdysseyChannelDepth : uint8
{
    kUninitializedChannelDepth UMETA( DisplayName="kUninitializedChannelDepth", Hidden ),
    k8 UMETA( DisplayName="8 bits" ),
    k16 UMETA( DisplayName="16 bits" ),
    //k32 UMETA( DisplayName="32 bits" ),
    kFloat UMETA( DisplayName="32 bits floating-point" ),
    //kDouble UMETA( DisplayName="64 bits floating-point" )
};

::ULIS::eFormat ULISFormatFromModelAndDepth(EOdysseyColorModel iColorModel, EOdysseyChannelDepth iChannelDepth);
EOdysseyColorModel OdysseyColorModelFromULISFormat(::ULIS::eFormat iFormat);
EOdysseyChannelDepth OdysseyChannelDepthFromULISFormat(::ULIS::eFormat iFormat);