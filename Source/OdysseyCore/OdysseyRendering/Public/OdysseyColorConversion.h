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

/** Returns the conversion undoing the given one, e.g. to write back into a content the render it was converted for. */
inline EOdysseyColorConversion InverseColorConversion(EOdysseyColorConversion Conversion)
{
    switch (Conversion)
    {
        case EOdysseyColorConversion::Encode: return EOdysseyColorConversion::Decode;
        case EOdysseyColorConversion::Decode: return EOdysseyColorConversion::Encode;
        default: return EOdysseyColorConversion::None;
    }
}

/**
 * Converts one color channel value on the CPU with the sRGB transfer functions, as the GPU conversions do.
 * Used to compare colors read back from a render, e.g. the colors the paint bucket compares.
 */
inline float ApplyColorConversion(float Value, EOdysseyColorConversion Conversion)
{
    switch (Conversion)
    {
        case EOdysseyColorConversion::Encode:
            return Value <= 0.0031308f ? Value * 12.92f : 1.055f * FMath::Pow(Value, 1.0f / 2.4f) - 0.055f;
        case EOdysseyColorConversion::Decode:
            return Value <= 0.04045f ? Value / 12.92f : FMath::Pow((Value + 0.055f) / 1.055f, 2.4f);
        default:
            return Value;
    }
}
