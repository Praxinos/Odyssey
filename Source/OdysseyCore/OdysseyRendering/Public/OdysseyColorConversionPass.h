// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#pragma once

#include "CoreMinimal.h"
#include "RenderGraphFwd.h"
#include "RHIFeatureLevel.h"

#include "OdysseyColorConversion.h"

struct FOpenColorIORenderPassResources;

/**
 * A color conversion ready to be added to render graphs, prepared on the game thread by FOdysseyColorConversions::Prepare().
 * Copyable, and usable on the render thread.
 */
class ODYSSEYRENDERING_API FOdysseyColorConversionPass
{
public:
    /** Returns false for no conversion, or when the conversion is unavailable: Add() then does nothing. */
    bool IsValid() const;

    /**
     * Converts the area of Input into the area of Output, alpha excepted; the other pixels of Output are kept.
     * Input and Output must be different textures. Call on the render thread.
     */
    void Add(FRDGBuilder& GraphBuilder, ERHIFeatureLevel::Type FeatureLevel,
        FRDGTextureRef Input, const FIntRect& InputRect, FRDGTextureRef Output, const FIntRect& OutputRect) const;

private:
    friend class FOdysseyColorConversions;

    TSharedPtr<const FOpenColorIORenderPassResources, ESPMode::ThreadSafe> Resources;
};

/**
 * Converts colors between linear values and sRGB encoded values with OpenColorIO, alpha excepted.
 *
 * The conversions are the transforms between the "Linear Rec.709 (sRGB)" and "sRGB Encoded Rec.709 (sRGB)" color spaces
 * of the OpenColorIO built-in CG configuration. Their shaders are compiled the first time a conversion is requested:
 * this call waits for the compilation on the game thread (up to two minutes without a shader cache).
 */
class ODYSSEYRENDERING_API FOdysseyColorConversions
{
public:
    /**
     * Prepares a conversion for render graphs. Call on the game thread.
     * The pass is invalid with None, or when the conversion is unavailable, which is logged.
     */
    static FOdysseyColorConversionPass Prepare(EOdysseyColorConversion Conversion);

    /** Returns true when the conversions are available, compiling them on first call. Call on the game thread. */
    static bool IsAvailable();
};
