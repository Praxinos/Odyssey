// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#pragma once

#include "CoreMinimal.h"

#include "OdysseyColorConversion.h"
#include "OdysseyTextureRenderingAbility.h"

/**
 * Converts colors between linear values and sRGB encoded values with OpenColorIO, alpha excepted.
 *
 * The conversions are the transforms between the "Linear Rec.709 (sRGB)" and "sRGB Encoded Rec.709 (sRGB)" color spaces
 * of the OpenColorIO built-in CG configuration. Their shaders are compiled the first time a conversion is requested.
 */
class ODYSSEYRENDERING_API FOdysseyColorSpaceShader
{
public:
    /**
     * Wraps a render function so that its output is converted: the function renders into an intermediate float texture,
     * whose destination area is then converted into the destination. Call on the game thread.
     * The function is returned unchanged with None, or when the conversion is unavailable, which is logged.
     */
    static IOdysseyTextureRenderingAbility::FRenderFunction WrapRenderFunction(
        IOdysseyTextureRenderingAbility::FRenderFunction RenderFunction, EOdysseyColorConversion Conversion);

    /** Returns true when the conversions are available, compiling them on first call. Call on the game thread. */
    static bool IsAvailable();
};
