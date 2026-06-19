// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Misc/EnumRange.h"
#include "OdysseyBlendingMode.generated.h"

UENUM( BlueprintType )
enum class EOdysseyBlendingMode : uint8
{
      kNormal               UMETA( DisplayName = "Normal"               )
    , kTop                  UMETA( DisplayName = "Top"                  )
    , kBack                 UMETA( DisplayName = "Back"                 )
    , kBehind               UMETA( DisplayName = "Behind"               )
    , kDissolve             UMETA( DisplayName = "Dissolve"             )
    , kBayerDither8x8       UMETA( DisplayName = "BayerDither8x8"       )
    , kDarken               UMETA( DisplayName = "Darken"               )
    , kMultiply             UMETA( DisplayName = "Multiply"             )
    , kColorBurn            UMETA( DisplayName = "ColorBurn"            )
    , kLinearBurn           UMETA( DisplayName = "LinearBurn"           )
    , kDarkerColor          UMETA( DisplayName = "DarkerColor"          )
    , kLighten              UMETA( DisplayName = "Lighten"              )
    , kScreen               UMETA( DisplayName = "Screen"               )
    , kColorDodge           UMETA( DisplayName = "ColorDodge"           )
    , kLinearDodge          UMETA( DisplayName = "LinearDodge"          )
    , kLighterColor         UMETA( DisplayName = "LighterColor"         )
    , kOverlay              UMETA( DisplayName = "Overlay"              )
    , kSoftLight            UMETA( DisplayName = "SoftLight"            )
    , kHardLight            UMETA( DisplayName = "HardLight"            )
    , kVividLight           UMETA( DisplayName = "VividLight"           )
    , kLinearLight          UMETA( DisplayName = "LinearLight"          )
    , kPinLight             UMETA( DisplayName = "PinLight"             )
    , kHardMix              UMETA( DisplayName = "HardMix"              )
    , kPhoenix              UMETA( DisplayName = "Phoenix"              )
    , kReflect              UMETA( DisplayName = "Reflect"              )
    , kGlow                 UMETA( DisplayName = "Glow"                 )
    , kDifference           UMETA( DisplayName = "Difference"           )
    , kExclusion            UMETA( DisplayName = "Exclusion"            )
    , kAdd                  UMETA( DisplayName = "Add"                  )
    , kSubstract            UMETA( DisplayName = "Substract"            )
    , kDivide               UMETA( DisplayName = "Divide"               )
    , kAverage              UMETA( DisplayName = "Average"              )
    , kNegation             UMETA( DisplayName = "Negation"             )
    , kHue                  UMETA( DisplayName = "Hue"                  )
    , kSaturation           UMETA( DisplayName = "Saturation"           )
    , kColor                UMETA( DisplayName = "Color"                )
    , kLuminosity           UMETA( DisplayName = "Luminosity"           )
    , kPartialDerivative    UMETA( DisplayName = "PartialDerivative"    )
    , kWhiteOut             UMETA( DisplayName = "WhiteOut"             )
    , kAngleCorrected       UMETA( DisplayName = "AngleCorrected"       )

    , kBlendingMode_Count      UMETA(Hidden)
};

ENUM_RANGE_BY_COUNT(EOdysseyBlendingMode, EOdysseyBlendingMode::kBlendingMode_Count)

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

    , kAlphaMode_Count      UMETA(Hidden)
};

ENUM_RANGE_BY_COUNT(EOdysseyAlphaMode, EOdysseyAlphaMode::kAlphaMode_Count)
