// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Misc/EnumRange.h"
#include "OdysseyBlendMode.generated.h"

/**
 * EOdysseyColorBlendMode
 * Only defines the Color channels part of pixel blending.
 * Which means no, it affects everything but transparency
 */

UENUM( BlueprintType )
enum class EOdysseyColorBlendMode : uint8
{
    Normal,
    Behind,

    Top,
    Back,

    Darken,
    Multiply,
    ColorBurn,
    LinearBurn,
    DarkerColor,

    Lighten,
    Screen,
    ColorDodge,
    LinearDodge,
    LighterColor,

    Overlay,
    SoftLight,
    HardLight,
    VividLight,
    LinearLight,
    PinLight,
    HardMix,

    Phoenix,
    Reflect,
    Glow,

    Difference,
    Exclusion,
    Add,
    Substract,
    Divide,
    Average,
    Negation,

    Hue,
    Saturation,
    Color,
    Luminosity,

    PartialDerivative,
    WhiteOut,
    AngleCorrected,

    Dissolve,
    BayerDither8x8,

    ColorBlendMode_Count      UMETA(Hidden)
};

ENUM_RANGE_BY_COUNT(EOdysseyColorBlendMode, EOdysseyColorBlendMode::ColorBlendMode_Count)

/**
 * EOdysseyAlphaBlendMode
 * Only defines the Alpha channel part of pixel blending.
 * Which means no, it affects only transparency
 */

UENUM( BlueprintType )
enum class EOdysseyAlphaBlendMode : uint8
{
    Normal,

    Mask,
    Stencil,

    Top,
    Back,

    Sub,
    Add,
    Mul,

    Min,
    Max,

    Dissolve,
    BayerDither8x8,

    AlphaBlendMode_Count      UMETA(Hidden)
};

ENUM_RANGE_BY_COUNT(EOdysseyAlphaBlendMode, EOdysseyAlphaBlendMode::AlphaBlendMode_Count)

/**
 * EOdysseyBlendMode defines common Blending modes
 * Each Blend Mode is actually a composition of :
 * - 1 EOdysseyColorBlendMode
 * - 1 EOdysseyAlphaBlendMode
 *
 * Tip :
 * To use only some enum values use :
 * - UPROPERTY(meta=(ValidEnumValues="[EnumValues]"))
 * - UPROPERTY(meta=(InvalidEnumValues="[EnumValues]"))
 * - UPROPERTY(meta=(GetRestrictedEnumValues="[FunctionName]"))
 */

UENUM( BlueprintType )
enum class EOdysseyBlendMode : uint8
{
    Normal,
    Behind,

    Mask,
    Stencil,

    Darken,
    Multiply,
    ColorBurn,
    LinearBurn,
    DarkerColor,

    Lighten,
    Screen,
    ColorDodge,
    LinearDodge,
    LighterColor,

    Overlay,
    SoftLight,
    HardLight,
    VividLight,
    LinearLight,
    PinLight,
    HardMix,

    Phoenix,
    Reflect,
    Glow,

    Difference,
    Exclusion,
    Add,
    Substract,
    Divide,
    Average,
    Negation,

    Hue,
    Saturation,
    Color,
    Luminosity,

    PartialDerivative,
    WhiteOut,
    AngleCorrected,

    Dissolve,
    BayerDither8x8,

    BlendMode_Count      UMETA(Hidden)
};

ENUM_RANGE_BY_COUNT(EOdysseyBlendMode, EOdysseyBlendMode::BlendMode_Count)

ODYSSEYRENDERING_API EOdysseyColorBlendMode GetColorBlendModeFromBlendMode(EOdysseyBlendMode InBlendMode);
ODYSSEYRENDERING_API EOdysseyAlphaBlendMode GetAlphaBlendModeFromBlendMode(EOdysseyBlendMode InBlendMode);
