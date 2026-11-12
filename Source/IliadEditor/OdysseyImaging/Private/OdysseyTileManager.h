// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Compression/CompressedBuffer.h"
#include "Memory/SharedBuffer.h"
#include "PixelFormat.h"
#include "RenderGraphBuilder.h"
#include "RHIGPUReadback.h"
#include "TickableEditorObject.h"

class UTexture;

class FOdysseyTileManager
    : public FTickableEditorObject
{
public:
    static FOdysseyTileManager& Get();

public:
    ~FOdysseyTileManager() = default;

private:
    FOdysseyTileManager() = default;

public:
    //All created tiles are permanently added to the Tiles array
    //which means a tile can be identified by its index in the Tiles array
    //Yet, for convenience, we create a struct FTileId allowing future optimizations
    struct FTileId
    {
        uint32 Index;
    };

    struct FCreateOrUpdateTile
    {
        /**
         * If true, creates a transparent Tile before copying into it
         * If false, copies the tile identified by OldTileId before copying into it
         */
        bool IsNewTile;
        FTileId OldTileId;
        FIntPoint Pos;
    };

    TArray<FTileId> CreateOrUpdateTiles(
        UTexture* InTexture,
        FIntRect InRect,
        FIntPoint InPosition,
        uint32 InTileSize,
        EPixelFormat InTileFormat,
        const TArray<FCreateOrUpdateTile>& InTilesInfos
    );

    bool GetTileBuffer(FTileId InTileId, FSharedBuffer& OutBuffer) const;

private:
    struct FTile
    {
        bool IsCacheInProgress;

        TSharedPtr<FRHIGPUTextureReadback> GPUReadBack;
        FCompressedBuffer CompressedBuffer;

        //mutable because it can be loaded while we read the tile
        //TFuture because GPU Readback is asynchronous
        //Also could help to prefetch tiles from disk (if we ever need to)
        TFuture<FSharedBuffer> UncompressedBuffer;
    };

    void LoadTile(FTileId InTileId);
    void LoadTileFromReadBack(FTileId InTileId);

    void CompressTile(FTileId InTileId);
    void CacheTileOnDisk(FTileId InTileId);
    void EvictUncompressedBufferFromTile(FTileId InTileId);
    void EvictCompressedBufferFromTile(FTileId InTileId);

    static void AddWriteTilePass(FRDGBuilder& GraphBuilder, FRDGTextureRef OutTexture, FSharedBuffer InBuffer);

private:
    // FTickableGameObject implementation
    virtual void Tick(float DeltaTime) override;
    virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(FOdysseyTileManager, STATGROUP_Tickables); }

private:
    TArray<FTile> Tiles;

    //Caching pipeline
    TArray<FTileId> PendingTilesToReadBack;
    TArray<FTileId> PendingTilesToCompress;
    TArray<FTileId> PendingTilesToCacheOnDisk;

    //Tiles eviction pipeline
    TArray<FTileId> TilesToEvictUncompressed;
    TArray<FTileId> TilesToEvictCompressed;

    FCriticalSection PipelineMutex;
};
