// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Compression/CompressedBuffer.h"
#include "IO/IoHash.h"
#include "Memory/SharedBuffer.h"
#include "PixelFormat.h"
#include "RenderGraphBuilder.h"
#include "RHIGPUReadback.h"
#include "TickableEditorObject.h"
#include "OdysseyTileManager.generated.h"

class UTexture;

/**
 * FTileId is an opaque handle to a Tile (FTile)
 */

USTRUCT()
struct FOdysseyTileId
{
    GENERATED_BODY()

    bool IsValid() const { return Index != INDEX_NONE && Generation != INDEX_NONE; };

    UPROPERTY()
    uint64 Index = INDEX_NONE;

    UPROPERTY()
    uint64 Generation = INDEX_NONE;

    bool operator==(const FOdysseyTileId&) const = default;
};

class FOdysseyTileManagerStats
{
public:
    FOdysseyTileManagerStats();

public:
    void AddUncompressed(const FSharedBuffer& InBuffer);
    void AddCompressed(const FCompressedBuffer& InBuffer);
    void AddCachedOnDisk(int64 InSizeOnDisk);

    void RemoveUncompressed(const FSharedBuffer& InBuffer);
    void RemoveCompressed(const FCompressedBuffer& InBuffer);
    void RemoveCachedOnDisk(int64 InSizeOnDisk);

    int64 GetNumTilesUncompressed() const;
    int64 GetSizeUncompressed() const;
    int64 GetNumTilesCompressed() const;
    int64 GetSizeCompressed() const;
    int64 GetNumTilesOnDisk() const;
    int64 GetSizeOnDisk() const;

private:
    //Stats
    TAtomic<int64> NumTilesUncompressed;
    TAtomic<int64> SizeUncompressed;
    TAtomic<int64> NumTilesCompressed;
    TAtomic<int64> SizeCompressed;
    TAtomic<int64> NumTilesOnDisk;
    TAtomic<int64> SizeOnDisk;
};
class FOdysseyTileManager
    : public FTickableEditorObject
{
public:
    static FOdysseyTileManager& Get();
    static FString GetCacheOnDiskPath();

public:
    void Initialize();
    void Finalize();

    /**
     * Forces Tile Eviction
     * Will respect TileManager Settings
     */
    void EvictTiles();

    /** Used for debug purposes
     * Evicts ALL Tiles from memory
     * Does not respect TileManager Settings
     */
    void EvictAllTiles();

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

    FOdysseyTileManagerStats& GetStats();

private:
    /**
     * FTileData contains the real Tile Data after GPU ReadBack
     * A FTileData is created asyncronously once GPU ReadBack is done
     * Allowing us to create a FTileData only if it contains non empty data
     */
    class FTileData
        : public TSharedFromThis<FTileData>
    {
        public:
            static void EvictTiles(uint64 InMaxUncompressedSize, uint64 InMaxCompressedSize);

        public:
            //Used when creating a tile while drawing
            static TSharedRef<FTileData> FromUncompressedBuffer(const FSharedBuffer& UncompressedBuffer);
            static TSharedRef<FTileData> FromCompressedBuffer(const FCompressedBuffer& UncompressedBuffer);

        public:
            bool GetUncompressedBuffer(FSharedBuffer& OutBuffer) const;
            bool GetCompressedBuffer(FCompressedBuffer& OutBuffer) const;

        private:
            static void Touch(TSharedRef<FTileData>);

            void Compress();
            void CacheOnDisk();

            bool LoadUncompressedBuffer() const;
            bool LoadCompressedBuffer() const;

            void EvictUncompressed();
            void EvictCompressed();

        private:
            static FCriticalSection Mutex;
            static FCriticalSection CacheOnDiskMutex;

            static TDoubleLinkedList<TSharedPtr<FTileData>> UncompressedLRU;
            static TDoubleLinkedList<TSharedPtr<FTileData>> CompressedLRU;

            mutable FSharedBuffer UncompressedBuffer;
            mutable FCompressedBuffer CompressedBuffer;
            int64 DiskCacheOffset = INDEX_NONE;

            mutable TDoubleLinkedList<TSharedPtr<FTileData>>::TDoubleLinkedListNode* UncompressedLRUNode;
            mutable TDoubleLinkedList<TSharedPtr<FTileData>>::TDoubleLinkedListNode* CompressedLRUNode;
    };

    /**
     * FTile is a reliable structure identifying a Tile
     * It contains the GPU ReadBack Buffers.
     * And the FTileData created after GPU ReadBack
     * FTile is created Synchronously and is directly linked to a FTileId
     *
     *
     * Several FTile can, in fine, point to the same FTileData.
     * An FTile can also be empty, in which case it will be marked as "Free" after ReadBack
     */
    struct FTile
    {
        /** An FTile can be reused if it was previously empty
         * Generation allows us to track how many times the FTile has been reused
         * Which also allows us to differentiate Tiles indentified by the same Index but different Generation
        */
        uint64 Generation;
        TSharedPtr<FRHIGPUTextureReadback> GPUReadBack;
        TSharedPtr<FRHIGPUBufferReadback> GPUIsEmptyReadBack; //TODO: Replace by a Hash ReadBack
        TFuture<TSharedPtr<FTileData>> TileData;
    };

    void TryLoadTileFromReadBack(FOdysseyTileId InTileId);
    void LoadTileFromReadBack(FOdysseyTileId InTileId);

    void CompressTile(TSharedPtr<FTileData> InTileData);
    void CacheTileOnDisk(TSharedPtr<FTileData> InTileData);

    static void AddWriteTilePass(FRDGBuilder& GraphBuilder, FRDGTextureRef OutTexture, FSharedBuffer InBuffer);

private:
    // FTickableGameObject implementation
    virtual void Tick(float DeltaTime) override;
    virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(FOdysseyTileManager, STATGROUP_Tickables); }

private:
    /**
     * Tiles and FreeTiles are always accessed / modified in GameThread
     */
    FCriticalSection FreeTilesMutex;
    FCriticalSection TilesDataMutex;

    TArray64<FTile> Tiles;
    TArray64<uint64> FreeTiles;

    TMultiMap<FIoHash, TSharedPtr<FTileData>> HashToTileData;

    //Caching pipeline
    TArray<FOdysseyTileId> PendingTilesToReadBack;

    FOdysseyTileManagerStats Stats;
};
