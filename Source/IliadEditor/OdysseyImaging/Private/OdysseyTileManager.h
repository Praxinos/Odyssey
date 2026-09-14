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
    FOdysseyTileManager()
        : StatNumTilesUncompressed(0)
        , StatSizeUncompressed(0)
        , StatNumTilesCompressed(0)
        , StatSizeCompressed(0)
        , StatNumTilesOnDisk(0)
        , StatSizeOnDisk(0)
    {
    }

public:
    //All created tiles are permanently added to the Tiles array
    //which means a tile can be identified by its index in the Tiles array
    //Yet, for convenience, we create a struct FTileId allowing future optimizations
    struct FTileId
    {
        FTileId()
            : Index(INDEX_NONE) //Invalid Index by default
            , Generation(INDEX_NONE)
        {
        }

        uint32 Index;
        uint32 Generation;
        bool IsValid() const { return Index != INDEX_NONE && Generation != INDEX_NONE; };
        bool operator==(const FTileId&) const = default;
    };

    struct FCreatedTile
    {
        FTileId Id;
        FIntPoint Pos;

        bool IsEmpty() const { return !Id.IsValid(); }
    };

    DECLARE_DELEGATE_RetVal_OneParam(FTileId, FGetExistingTileId, const FIntPoint&)

    /**
     * Creates tiles from the given rect in the given texture
     * @param InTexture Texture to copy from
     * @param InRect The Texture rect to copy from
     * @param InPosition Position where to copy the texture in the tile space in pixels
     * @param InTileSize Width and height of the tile to create in pixels, tiles are always squares
     * @param InTileFormat The tile pixel format
     * @param InTilesInfos Defines infos the created tiles should respect
     */
    TArray<FCreatedTile> CreateOrUpdateTiles(
        UTexture* InTexture,
        FIntRect InRect,
        FIntPoint InPosition,
        uint32 InTileSize,
        EPixelFormat InTileFormat,
        const FGetExistingTileId& InGetExistingTileId = FGetExistingTileId()
    );

    bool GetTileBuffer(FTileId InTileId, FSharedBuffer& OutBuffer) const;

public:
    //Stats

    int64 GetNumTilesUncompressed() const;
    int64 GetSizeUncompressed() const;
    int64 GetNumTilesCompressed() const;
    int64 GetSizeCompressed() const;
    int64 GetNumTilesOnDisk() const;
    int64 GetSizeOnDisk() const;

private:
    struct FTile
    {
        TSharedPtr<FRHIGPUTextureReadback> GPUReadBack;
        TSharedPtr<FRHIGPUBufferReadback> GPUIsEmptyReadBack;
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

    FCriticalSection FreeTilesMutex;
    TArray<FTileId> FreeTiles;

    //Caching pipeline
    FCriticalSection PipelineMutex;
    TArray<FTileId> PendingTilesToReadBack;

    //Tiles eviction pipeline
    TArray<FTileId> TilesToEvictUncompressed;
    TArray<FTileId> TilesToEvictCompressed;

    //Stats
    TAtomic<int64> StatNumTilesUncompressed;
    TAtomic<int64> StatSizeUncompressed;
    TAtomic<int64> StatNumTilesCompressed;
    TAtomic<int64> StatSizeCompressed;
    TAtomic<int64> StatNumTilesOnDisk;
    TAtomic<int64> StatSizeOnDisk;
};
