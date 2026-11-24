// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "RenderGraphFwd.h"
#include "ShaderParameterMacros.h"

class FRHIGPUBufferReadback;

namespace Odyssey::Shaders {

    /**
     * Adds a compute pass that tells, for each tile of a texture, whether all its pixels are transparent.
     * Only the alpha is tested: transparent pixels with a color are empty.
     *
     * @param InTexture The texture holding the tiles.
     * @param InTilePositions The top-left corners of the tiles, in texture pixels.
     * @param InTileSize The size of a tile, in pixels; pixels outside the texture are ignored.
     * @param InReadBack Receives one uint32 per tile, in the order of InTilePositions: 1 when the tile is empty, 0 otherwise.
     *                   Read it once the readback is ready (FRHIGPUBufferReadback::IsReady()).
     */
    ODYSSEYRENDERING_API void AddAreTextureTilesEmptyPass(
        FRDGBuilder& GraphBuilder,
        ERHIFeatureLevel::Type InFeatureLevel,
        FRDGTextureRef InTexture,
        const TArray<FIntPoint>& InTilePositions,
        uint32 InTileSize,
        TSharedPtr<FRHIGPUBufferReadback> InReadBack
    );

}
