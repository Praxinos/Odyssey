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
#include "OdysseyTileManager.generated.h"

class UTexture;

/**
 * FTileId is an opaque handle to a Tile (FTile)
 * Several FTile can, in fine, point to the same FTileData.
 * An FTile can also be empty.
 *
 * Keeping an FTileId in memory will keep the tiles data available (at least on disk)
 * So releasing the FTileId when you don't need it anymore is recommended.
 */

USTRUCT()
struct FOdysseyTileId
{
    GENERATED_BODY()

    bool IsValid() const { return Index != INDEX_NONE; };

    UPROPERTY()
    uint64 Index = INDEX_NONE;
};

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
    struct FCreatedTile
    {
        FOdysseyTileId Id;
        FIntPoint Pos;

        bool IsEmpty() const { return !Id.IsValid(); }
    };

    DECLARE_DELEGATE_RetVal_OneParam(FOdysseyTileId, FGetExistingTileId, const FIntPoint&)

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

    bool GetTileBuffer(FOdysseyTileId InTileId, FSharedBuffer& OutBuffer);
    bool GetTileBuffer(FOdysseyTileId InTileId, FSharedBuffer& OutBuffer) const;

public:
    //Stats

    int64 GetNumTilesUncompressed() const;
    int64 GetSizeUncompressed() const;
    int64 GetNumTilesCompressed() const;
    int64 GetSizeCompressed() const;
    int64 GetNumTilesOnDisk() const;
    int64 GetSizeOnDisk() const;

private:
    /**
     * FTile contains the real Tile Data after GPU ReadBack
     * A FTile is created asyncronously once GPU ReadBack is done
     * Allowing us to create a FTile only if it contains non empty data
     */
    struct FTileData
    {
        FSharedBuffer UncompressedBuffer;
        FCompressedBuffer CompressedBuffer;
    };

    /**
     * FTile is a reliable structure identifying a Tile
     * It contains the GPU ReadBack Buffers.
     * And the index of the FTileData created after GPU ReadBack
     * FTile is created Synchronously and is directly linked to a FTileId
     */
    struct FTile
    {
        TSharedPtr<FRHIGPUTextureReadback> GPUReadBack;
        TSharedPtr<FRHIGPUBufferReadback> GPUIsEmptyReadBack;

        /**
         * If !TileDataIndex.IsValid()
         * GPUReadBack has not started yet
         *
         * If TileDataIndex.Get() == INDEX_NONE
         * We consider the tile as empty
         */
        TFuture<uint64> TileDataIndex;

        bool IsPendingReadBack() const
        {
            return !TileDataIndex.IsValid();
        };

        bool IsEmpty() const
        {
            check(TileDataIndex.IsValid());
            return TileDataIndex.Get() == INDEX_NONE;
        };
        //bool IsValid() const { return Index != INDEX_NONE && Generation != INDEX_NONE; };
        //bool operator==(const FTileId&) const = default;
    };

    void LoadTileFromReadBack(uint64 InTileIndex);

    void CompressTile(uint64 InTileDataIndex);
    void CacheTileOnDisk(uint64 InTileDataIndex);
    /*
    void EvictUncompressedBufferFromTile(const FTile& InTileId);
    void EvictCompressedBufferFromTile(const FTile& InTileId);
    */

    static void AddWriteTilePass(FRDGBuilder& GraphBuilder, FRDGTextureRef OutTexture, FSharedBuffer InBuffer);

private:
    // FTickableGameObject implementation
    virtual void Tick(float DeltaTime) override;
    virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(FOdysseyTileManager, STATGROUP_Tickables); }

private:
    /**
     * Tiles and FreeTiles are always accessed / modified in GameThread
     */
    TArray64<FTile> Tiles;
    TArray64<uint64> FreeTiles;

    /**
     * TilesData and FreeTilesData can be accessed / modified by any thread
     * So we need a mutex
     */
    FCriticalSection TilesDataMutex;
    TArray64<FTileData> TilesData;
    TArray64<uint64> FreeTilesData;

    //Caching pipeline
    FCriticalSection PipelineMutex;
    TArray<uint64> PendingTilesToReadBack;

    //Tiles eviction pipeline
    //TArray<FOdysseyTileId> TilesToEvictUncompressed;
    //TArray<FTileIdFOdysseyTileId> TilesToEvictCompressed;

    //Stats
    TAtomic<int64> StatNumTilesUncompressed;
    TAtomic<int64> StatSizeUncompressed;
    TAtomic<int64> StatNumTilesCompressed;
    TAtomic<int64> StatSizeCompressed;
    TAtomic<int64> StatNumTilesOnDisk;
    TAtomic<int64> StatSizeOnDisk;
};
