// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "RenderGraphFwd.h"
#include "ShaderParameterMacros.h"

class FRHIGPUBufferReadback;

namespace Odyssey::Shaders {

    ODYSSEYRENDERING_API void AddAreTextureTilesEmptyPass(
        FRDGBuilder& GraphBuilder,
        ERHIFeatureLevel::Type InFeatureLevel,
        FRDGTextureRef InTexture,
        const TArray<FIntPoint>& InTilePositions,
        uint32 InTileSize,
        TSharedPtr<FRHIGPUBufferReadback> InReadBack
    );

}
