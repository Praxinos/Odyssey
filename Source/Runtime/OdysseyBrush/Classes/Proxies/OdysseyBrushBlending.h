// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include <ULIS_BLENDINGMODES>
#include "OdysseyBrushBlending.generated.h"

UENUM( BlueprintType )
enum class EOdysseyBlendingMode : uint8
{
      kNormal           UMETA( DisplayName = "Normal"           )
    , kErase            UMETA( DisplayName = "Erase"            )
    , kBehind           UMETA( DisplayName = "Behind"           )
    , kDissolve         UMETA( DisplayName = "Dissolve"         )
    , kDarken           UMETA( DisplayName = "Darken"           )
    , kMultiply         UMETA( DisplayName = "Multiply"         )
    , kColorBurn        UMETA( DisplayName = "ColorBurn"        )
    , kLinearBurn       UMETA( DisplayName = "LinearBurn"       )
    , kDarkerColor      UMETA( DisplayName = "DarkerColor"      )
    , kLighten          UMETA( DisplayName = "Lighten"          )
    , kScreen           UMETA( DisplayName = "Screen"           )
    , kColorDodge       UMETA( DisplayName = "ColorDodge"       )
    , kLinearDodge      UMETA( DisplayName = "LinearDodge"      )
    , kLighterColor     UMETA( DisplayName = "LighterColor"     )
    , kOverlay          UMETA( DisplayName = "Overlay"          )
    , kSoftLight        UMETA( DisplayName = "SoftLight"        )
    , kHardLight        UMETA( DisplayName = "HardLight"        )
    , kVividLight       UMETA( DisplayName = "VividLight"       )
    , kLinearLight      UMETA( DisplayName = "LinearLight"      )
    , kPinLight         UMETA( DisplayName = "PinLight"         )
    , kHardMix          UMETA( DisplayName = "HardMix"          )
    , kDifference       UMETA( DisplayName = "Difference"       )
    , kExclusion        UMETA( DisplayName = "Exclusion"        )
    , kSubstract        UMETA( DisplayName = "Substract"        )
    , kDivide           UMETA( DisplayName = "Divide"           )
    , kHue              UMETA( DisplayName = "Hue"              )
    , kSaturation       UMETA( DisplayName = "Saturation"       )
    , kColor            UMETA( DisplayName = "Color"            )
    , kLuminosity       UMETA( DisplayName = "Luminosity"       )
};

UENUM( BlueprintType )
enum class EOdysseyAlphaMode : uint8
{
      kNormal           UMETA( DisplayName = "Normal"   )
    , kErase            UMETA( DisplayName = "Erase"    )
    , kTop              UMETA( DisplayName = "Top"      )
    , kBack             UMETA( DisplayName = "Back"     )
    , kSub              UMETA( DisplayName = "Sub"      )
    , kAdd              UMETA( DisplayName = "Add"      )
    , kMul              UMETA( DisplayName = "Mul"      )
    , kMin              UMETA( DisplayName = "Min"      )
    , kMax              UMETA( DisplayName = "Max"      )
};

/////////////////////////////////////////////////////
// eAlphaMode
enum class eAlphaMode : uint8
{
      kNormal
    , kErase
    , kTop
    , kBack
    , kSub
    , kAdd
    , kMul
    , kMin
    , kMax
    , kNumAlphaModes
};
