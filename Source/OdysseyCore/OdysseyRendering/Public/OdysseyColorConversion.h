// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#pragma once

#include "CoreMinimal.h"

/** A conversion of colors between linear values and sRGB encoded values. */
enum class EOdysseyColorConversion : uint8
{
    None,
    /** Linear to sRGB encoded values. */
    Encode,
    /** sRGB encoded to linear values. */
    Decode,
};
