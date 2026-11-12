// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"

class UTexture;

// This is a public interface that we define so outside code can invoke our compute shader.
class ODYSSEYRENDERING_API FOdysseyTiledRLE
{

public:
    struct FCompressionParams
    {
        uint32 TextureHeight;
        uint32 TextureWidth;
        uint32 TileWidth;
        uint32 TileHeight;
        EPixelFormat PixelFormat;
    };

    struct FRLEBuffer
    {
        //TODO: Add a data buffer here
    };

public:
    // Dispatches this shader. Can be called from any thread
    static FRLEBuffer Compress(UTexture* iTexture, const FCompressionParams& Params);

    // Executes this shader on the render thread
    static FRDGTextureRef DecompressRenderThread(FRDGBuilder& GraphBuilder, const FRLEBuffer& iBuffer, const FCompressionParams& Params);

    // Executes this shader on the render thread from the game thread via EnqueueRenderThreadCommand
    static void DecompressGameThread(const FRLEBuffer& iBuffer, const FCompressionParams& Params, UTextureRenderTarget2D* oTexture);
};
