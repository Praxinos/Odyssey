// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"

class UTexture;

// This is a public interface that we define so outside code can invoke our compute shader.
class ODYSSEYRENDERING_API FOdysseyTiledRLE
{
private:
    //Weights 32Ko for an empty 4096x4096 image with 64x64 tiles
    struct FTileDescriptor
    {
        int32 Position;
        uint32 Count;
        bool EndsWithRepetition;
    };

public:
    struct FCompressionParams
    {
        uint32 TileWidth;
        uint32 TileHeight;
        uint32 BytesPerComponent;
        uint32 ComponentsPerPixel;
    };

    struct FRLECompressedBuffer
    {
        uint32 TextureHeight;
        uint32 TextureWidth;
        uint32 TileWidth;
        uint32 TileHeight;
        uint32 ComponentsPerPixel;
        uint32 BytesPerComponent;
        TArray<FTileDescriptor> TileDescriptors;
        TArray<int32> RLEPositions;
        TArray<uint8> RLEData;
        TArray<uint8> SequenceData;
    };

public:
    // Dispatches this shader. Can be called from any thread
    static TSharedRef<TPromise<FOdysseyTiledRLE::FRLECompressedBuffer>> Compress(UTexture* iTexture, const FCompressionParams& Params);

    // Executes this shader on the render thread
    static FRDGTextureRef DecompressRenderThread(FRDGBuilder& GraphBuilder, const FRLECompressedBuffer& iBuffer, EPixelFormat iPixelFormat);

    // Executes this shader on the render thread from the game thread via EnqueueRenderThreadCommand
    static void DecompressGameThread(const FRLECompressedBuffer& iBuffer, UTextureRenderTarget2D* oTexture);

    static void TestDecompressTiledRLEShader(uint32 iTextureWidth, uint32 iTextureHeight, FRLECompressedBuffer& oRLEBuffer);
};
