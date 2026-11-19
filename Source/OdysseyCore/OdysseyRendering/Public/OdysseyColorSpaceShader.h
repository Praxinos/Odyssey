// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#pragma once

#include "CoreMinimal.h"
#include "RenderGraphFwd.h"
#include "RHIFeatureLevel.h"

#include "OdysseyColorConversion.h"
#include "OdysseyTextureRenderingAbility.h"

/**
 * Converts straight alpha colors between linear and sRGB encoded values, alpha excepted, with the exact sRGB transfer functions.
 */
class ODYSSEYRENDERING_API FOdysseyColorSpaceShader
{
public:
    /**
     * Converts Rect of Source into the same pixels of Destination. Source and Destination must be different textures.
     * Nothing is drawn with EOdysseyColorConversion::None.
     */
    static void Convert(FRDGBuilder& GraphBuilder, ERHIFeatureLevel::Type FeatureLevel, FRDGTextureRef Source,
        FRDGTextureRef Destination, const FIntRect& Rect, EOdysseyColorConversion Conversion);

    /**
     * Wraps a render function so that its output is converted: the function renders into an intermediate float texture,
     * whose destination area is then converted into the destination. The function is returned unchanged with None.
     */
    static IOdysseyTextureRenderingAbility::FRenderFunction WrapRenderFunction(
        IOdysseyTextureRenderingAbility::FRenderFunction RenderFunction, EOdysseyColorConversion Conversion);
};
