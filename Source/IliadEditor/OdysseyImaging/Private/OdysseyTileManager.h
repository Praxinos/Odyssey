// IDDN.FR.001.060015.015.S.X.2019.000.00000
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
#include "OdysseyTileDiskCacheDirectory.h"
#include "OdysseyTileId.h"

class UTexture;

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

    int64 GetNumTilesUncompressed() const;
    int64 GetSizeUncompressed() const;
    int64 GetNumTilesCompressed() const;
    int64 GetSizeCompressed() const;
    int64 GetNumTilesOnDisk() const;
    int64 GetSizeOnDisk() const;
    int64 GetNumTilesOnGPU() const { return NumTilesOnGPU; }
    int64 GetSizeOnGPU() const { return SizeOnGPU; }
    void SetGPUCacheStats(int64 NumTiles, int64 Size) { NumTilesOnGPU = NumTiles; SizeOnGPU = Size; }

private:
    //Stats
    TAtomic<int64> NumTilesUncompressed;
    TAtomic<int64> SizeUncompressed;
    TAtomic<int64> NumTilesCompressed;
    TAtomic<int64> SizeCompressed;
    TAtomic<int64> NumTilesOnDisk;
    TAtomic<int64> SizeOnDisk;
    TAtomic<int64> NumTilesOnGPU{0};
    TAtomic<int64> SizeOnGPU{0};
};

/**
 * Holds every tile of the editor session: the pixels of all the UOdysseyTiledImage, deduplicated, compressed and cached
 * on the GPU, in RAM and on disk within the budgets of UOdysseyTileManagerSettings.
 * Created and destroyed by the OdysseyImaging module. See Documentation/Doxygen/manual/Architecture/TiledImage.md.
 *
 * Pipeline of a write (UOdysseyTiledImage::CopyFromTexture -> CreateOrUpdateTiles):
 *   1. render thread: the old tile and the source are drawn into a GPU atlas (the creation texture of each new FTile),
 *      the empty tiles are tested (AddAreTextureTilesEmptyPass) and a readback of the atlas is enqueued;
 *   2. Tick (game thread) polls the readbacks; once one is ready, a render command processes it: an empty tile frees its slot
 *      (Generation + 1); otherwise its pixels are hashed and deduplicated against HashToTileData, or become a new FTileData,
 *      and the creation texture moves to the GPU cache;
 *   3. thread pool: the data is compressed, then appended to the disk file of the session (FOdysseyTileDiskCacheDirectory);
 *   4. Tick: EvictTiles() releases the uncompressed buffers beyond MaxUncompressedSize, then the compressed buffers already
 *      on disk beyond MaxCompressedSize (least recently used first); they are reloaded when needed.
 * Reads (GetTileTextures): pending creation texture, GPU cache hit, or an upload batched into an atlas. MaintainGPUCache()
 * keeps the most recently used tiles within MaxGPUSize, repacked into atlases of GPUCachePageBytes.
 *
 * Threads and locks:
 * | Data                                     | Threads              | Lock                 |
 * |------------------------------------------|----------------------|----------------------|
 * | Tiles, FreeTiles (slot table)            | game, render         | FreeTilesMutex       |
 * | one FTile (futures, readbacks)           | game, render         | FTile::Mutex         |
 * | HashToTileData                           | render, game         | HashToTileDataMutex  |
 * | FTileData LRU lists (static)             | game, render         | FTileData::LRUMutex  |
 * | GPU cache (GPUTextures)                  | game, render         | GPUCacheMutex        |
 *
 * Known limitation: tiles are never released during a session; only their buffers are evicted.
 */
class FOdysseyTileManager
    : public FTickableEditorObject
{
public:
    static FOdysseyTileManager& Get();
    static FString GetCacheOnDiskPath();

private:
    class FTileAtlas;

public:
    //Public structs/classes
    struct FCreatedTile
    {
        FOdysseyTileId Id;
        /** The tile coordinate: the pixel position of the tile divided by the tile size. */
        FIntPoint TileCoord;
    };

    class FTileTextureHandle
    {
        friend class FOdysseyTileManager;
    public:
        /** Holds an immutable location, keeping already submitted renders valid during compaction. */
        ~FTileTextureHandle();
        FTileTextureHandle();
        FTileTextureHandle(TSharedPtr<FTileAtlas> InAtlas, uint32 InTileIndexInAtlas);

    public:
        FTextureRHIRef GetTextureRHI() const;
        FIntRect GetRectInTexture() const;
        FIntPoint GetPositionInTexture() const;

    private:
        /** Defines the Atlas containing the Tile */
        TSharedPtr<FTileAtlas> Atlas;

        /** The slice of the Atlas containing the Tile */
        uint32 TileIndexInAtlas;
    };

private:
    //Private structs/classes

    class FTileAtlas
    {
    public:
        FTileAtlas() = default;
        FTileAtlas(FRHICommandList& InRHICmdList, uint32 InTileSize, EPixelFormat InTileFormat, uint32 InNumTiles, bool bInCacheAtlas = false);
        static FRHITextureCreateDesc GetTextureDesc(uint32 InTileSize, EPixelFormat InTileFormat, uint32 InNumTiles, bool bInCacheAtlas);

    public:
        FTextureRHIRef GetTexture() const { return Texture; };

        FIntRect GetTileRect(uint32 InTileIndexInAtlas) const;
        FIntPoint GetTilePosition(uint32 InTileIndexInAtlas) const;
        uint64 GetSizeInBytes() const { return SizeInBytes; }
        uint32 GetCapacity() const { return WidthInTiles * HeightInTiles; }
        bool IsCacheAtlas() const { return bCacheAtlas; }

    private:
        FTextureRHIRef Texture;

        uint32 WidthInTiles;
        uint32 HeightInTiles;
        uint32 TileSize;
        uint64 SizeInBytes = 0;
        bool bCacheAtlas = false;
    };

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
            static TSharedRef<FTileData> FromUncompressedBuffer(const FIoHash& InHash, const FSharedBuffer& InUncompressedBuffer);
            static TSharedRef<FTileData> FromCompressedBuffer(const FIoHash& InHash, const FCompressedBuffer& InCompressedBuffer);
            static TSharedRef<FTileData> FromBuffers(const FIoHash& InHash, const FSharedBuffer& InUncompressedBuffer, const FCompressedBuffer& InCompressedBuffer);

        public:
            bool GetUncompressedBuffer(FSharedBuffer& OutBuffer) const;
            bool GetCompressedBuffer(FCompressedBuffer& OutBuffer) const;
            const FIoHash& GetHash() const;

            void WaitUntilCachedOnDisk();

        private:
            static void Touch(TSharedRef<FTileData>);

            FCompressedBuffer Compress();
            void CacheOnDisk();

            bool LoadUncompressedBuffer() const;
            bool LoadCompressedBuffer() const;

            void EvictUncompressed();
            void EvictCompressed();

        private:
            enum class ECacheState
            {
                InProgress,
                Cached,
                Failed
            };

            //Static members
            /** Guards the static LRU lists of tile data. */
            static FCriticalSection LRUMutex;
            static FCriticalSection CacheOnDiskMutex;

            static TDoubleLinkedList<TSharedPtr<FTileData>> UncompressedLRU;
            static TDoubleLinkedList<TSharedPtr<FTileData>> CompressedLRU;

            //Non-Static members
            FCriticalSection CacheStateMutex;
            FIoHash Hash;
            ECacheState CacheState = ECacheState::InProgress;
            mutable FSharedBuffer UncompressedBuffer;
            mutable TFuture<FCompressedBuffer> CompressedBuffer;
            int64 DiskCacheOffset = INDEX_NONE;
            TFuture<void> CacheCompressedCompletionEvent;
            TFuture<void> CacheOnDiskCompletionEvent;

            mutable TDoubleLinkedList<TSharedPtr<FTileData>>::TDoubleLinkedListNode* UncompressedLRUNode = nullptr;
            mutable TDoubleLinkedList<TSharedPtr<FTileData>>::TDoubleLinkedListNode* CompressedLRUNode = nullptr;
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
    class FTile
    {
    public:
        FTile(const FOdysseyTileId& InId);

    public:
        void Initialize();
        void Initialize(TSharedPtr<FTileData> InTileData);

    public:
        enum class EGPUReadBackState
        {
            Idle, //GPU Readback has not started yet
            InProgress, //GPU Readback is in progress
            Done //GPU Readback is finished
        };


        //When reading a Tile, its data must stay consistent
        FCriticalSection Mutex;

        FOdysseyTileId Id;
        TUniquePtr<TPromise<TSharedPtr<FTileTextureHandle>>> CreationTexturePromise;
        TSharedFuture<TSharedPtr<FTileTextureHandle>> CreationTexture;

        EGPUReadBackState GPUReadBackState;

        struct FPartialBufferReadBack
        {
            TSharedPtr<FRHIGPUBufferReadback> ReadBack;
            uint32 ResultSize;
            uint32 ResultIndex;
        };

        struct FPartialTextureReadBack
        {
            TSharedPtr<FRHIGPUTextureReadback> ReadBack;
            FIntPoint PositionInPixels;
            uint32 BytesPerPixel;
            uint32 TileSize;
        };

        TUniquePtr<TPromise<FPartialBufferReadBack>> GPUIsEmptyReadBackPromise;
        TFuture<FPartialBufferReadBack> GPUIsEmptyReadBack;

        TUniquePtr<TPromise<FPartialTextureReadBack>> GPUReadBackPromise;
        TFuture<FPartialTextureReadBack> GPUReadBack;
        TPromise<TSharedPtr<FTileData>> TileDataPromise;
        TFuture<TSharedPtr<FTileData>> TileData;
    };

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

    DECLARE_DELEGATE_RetVal_OneParam(FOdysseyTileId, FGetExistingTileId, const FIntPoint&)

    /**
     * Creates tiles from the given rect in the given texture
     * @param InTexture Texture to copy from
     * @param InRect The Texture rect to copy from
     * @param InPosition Position where to copy the texture in the tile space in pixels
     * @param InTileSize Width and height of the tile to create in pixels, tiles are always squares
     * @param InTileFormat The tile pixel format
     * @param InGetExistingTileId Returns the id of the tile already at a tile coordinate, if any: the pixels of a tile that
     *                            the rect covers partially are kept outside the rect.
     * @return The new tile of each tile coordinate the rect touches. Game thread.
     */
    TArray<FCreatedTile> CreateOrUpdateTiles(
        UTexture* InTexture,
        FIntRect InRect,
        FIntPoint InPosition,
        uint32 InTileSize,
        EPixelFormat InTileFormat,
        const FGetExistingTileId& InGetExistingTileId = FGetExistingTileId()
    );

    /**
     * Creates a tile from saved data (UOdysseyTiledImage::Serialize), deduplicated with the existing data of the same hash.
     * The data is decompressed on first use. Game thread.
     */
    FOdysseyTileId CreateTile(const FIoHash& InHash, const FCompressedBuffer& InCompressedBuffer);

    /**
     * Creates the tiles of an area given as CPU pixels, deduplicated with the existing data, without going through the GPU.
     * Game thread only. Pixels of the touched tiles outside the area are transparent. A tile whose alpha is 0 everywhere is empty: no tile is
     * created for it, -0 counting as 0 and NaN as content, as for the tiles created from a texture.
     * @param InPosition The position of the top-left pixel of the area, in pixels; a multiple of InTileSize.
     * @param InSize The size of the area, in pixels.
     * @param InPixels The pixels of the area in the tile pixel format, in rows of InSize.X pixels.
     * @param InTileSize Width and height of the tiles in pixels.
     * @param InTileFormat The tile pixel format: PF_R8G8B8A8, PF_FloatRGBA or PF_A32B32G32R32F.
     * @return For each tile coordinate the area touches, its new tile; an invalid id for an empty tile.
     *         Empty when InTileSize is invalid, InSize is not positive, InPixels is too short or InTileFormat is unsupported.
     */
    TArray<FCreatedTile> CreateTilesFromPixels(FIntPoint InPosition, FIntPoint InSize, FMemoryView InPixels, uint32 InTileSize, EPixelFormat InTileFormat);

    /**
     * Returns, for each TileId, a FTextureRHIRef containing the Tile's Texture
     * The returned FTextureRHIRef can be invalid, indicating an empty texture
     * Please check FTextureRHIRef::IsValid() before using it
     * Does not wait for queued render commands on the game thread. Consume the returned futures
     * in a render command queued after this call, or after IsReady(). Resident CPU data may be
     * loaded synchronously from the disk/compression cache.
     */
    TArray<TSharedFuture<TSharedPtr<FTileTextureHandle>>> GetTileTextures(const TArray<FOdysseyTileId>& InTileIds, uint32 InTileSize, EPixelFormat InTileFormat) const;

    /** Blocking game-thread access for serialization; may wait for GPU readback, compression and disk I/O.
     * Returns true with a null buffer for an empty tile, and false on a data retrieval error. */
    bool GetTileCompressedBuffer(FOdysseyTileId InTileId, FCompressedBuffer& OutBuffer);
    bool GetTileCompressedBuffer(FOdysseyTileId InTileId, FCompressedBuffer& OutBuffer) const;

    /** Blocking game-thread access; may wait for GPU readback. */
    bool GetTileHash(FOdysseyTileId InTileId, FIoHash& OutHash);

    FOdysseyTileManagerStats& GetStats();

    /**
     * Waits until all tiles are cached on disk and GPU cache compaction is published, then evicted if needed.
     * Blocks the game thread. Intended for explicit synchronization and shutdown, not polling.
     */
    void WaitUntilAllTilesAreCached(bool InEvictTiles = true);

    /** Writes the memory use, the tile counts and the budgets of the tile manager to the log. Game thread only. */
    void LogStats();

private:
    /** Snapshot of the tile manager's slot, tile data and readback counts. */
    struct FTileCounts
    {
        /** Tile slots allocated, live and free. */
        int64 NumSlots = 0;
        /** Tile slots holding a tile, that is, not on the free list. */
        int64 NumLiveSlots = 0;
        /** Distinct tile data entries after deduplication by content hash. */
        int64 NumUniqueTileData = 0;
        /** Tiles whose GPU readback has not completed yet. */
        int64 NumPendingReadBacks = 0;
    };

    /** Returns a snapshot of the tile counts. Game thread only. */
    [[nodiscard]] FTileCounts CreateTileCounts();

    /** Publishes the stats as trace counters, shown by Unreal Insights when the counters channel is traced. */
    void TraceStats();

    template<typename... ArgsType>
    TSharedPtr<FTile> CreateNewTile(ArgsType&&... Args);

    void TryLoadTileFromReadBack(TSharedPtr<FTile> InTile);
    void LoadTileFromReadBack(TSharedPtr<FTile> InTile);
    /** Removes from PendingTilesToReadBack the tiles whose readback has started. Game thread only. */
    void RemoveStartedReadBacks();

    /** Returns the data holding the same bytes as InBuffer under InHash, or null. HashToTileDataMutex must be held. */
    TSharedPtr<FTileData> FindTileData(const FIoHash& InHash, FMemoryView InBuffer) const;

    /** Returns true when the tile is empty, as CreateTilesFromPixels() defines it. */
    static bool IsTileEmpty(FMemoryView InTile, EPixelFormat InTileFormat);

    /**
     * Ends the readback of a tile: releases its creation texture and readbacks and gives its data, null for an empty tile.
     * The caller holds the mutex of the tile.
     */
    static void CompleteReadBack(FTile& InTile, const TSharedPtr<FTileData>& InTileData);

    void CompressTile(TSharedPtr<FTileData> InTileData);
    void CacheTileOnDisk(TSharedPtr<FTileData> InTileData);

    static void AddWriteTilePass(FRDGBuilder& GraphBuilder, FRDGTextureRef OutAtlasTexture, FIntPoint TilePositionInAtlas, uint32 InTileSize, FSharedBuffer InBuffer);

    struct FGPUCacheKey
    {
        FGPUCacheKey(const FTileData* InTileData, uint32 InTileSize, EPixelFormat InFormat)
            : TileData(InTileData), TileSize(InTileSize), Format(InFormat)
        {
        }

        // HashToTileData retains the immutable content identity throughout the session.
        const FTileData* TileData;
        uint32 TileSize;
        EPixelFormat Format;
        bool operator==(const FGPUCacheKey&) const = default;
        friend uint32 GetTypeHash(const FGPUCacheKey& Key)
        {
            return HashCombine(PointerHash(Key.TileData),
                HashCombine(::GetTypeHash(Key.TileSize), ::GetTypeHash(uint32(Key.Format))));
        }
    };

    struct FGPUCachedTile
    {
        TSharedPtr<FTileTextureHandle> Handle;
        uint64 LastUsed = 0;
    };

    TSharedPtr<FTileTextureHandle> FindGPUTexture(const FGPUCacheKey& Key);
    void CacheGPUTexture(const FGPUCacheKey& Key, TSharedPtr<FTileTextureHandle> Handle);
    void RequestGPUCacheMaintenance();
    void MaintainGPUCache(FRHICommandListImmediate& RHICmdList);
    void UpdateGPUCacheStats(); // GPUCacheMutex must be held.
    void ClearGPUCache();
    void SetGPUCacheBudget(uint64 Bytes);
    uint32 GetMaxTilesPerGPUAtlas(uint32 TileSize, EPixelFormat Format);

private:
    // FTickableGameObject implementation
    virtual void Tick(float DeltaTime) override;
    virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(FOdysseyTileManager, STATGROUP_Tickables); }

private:
    friend class FTileTextureHandle;
#if WITH_DEV_AUTOMATION_TESTS
    friend struct FOdysseyTileManagerTestAccess;
#endif

    FCriticalSection FreeTilesMutex;
    FCriticalSection HashToTileDataMutex;
    FCriticalSection GPUCacheMutex;
    TMap<FGPUCacheKey, FGPUCachedTile> GPUTextures;
    TMap<FGPUCacheKey, TSharedFuture<TSharedPtr<FTileTextureHandle>>> PendingGPUTextures;
    TMap<FTileAtlas*, TSharedPtr<FTileAtlas>> GPUAtlases;
    uint64 GPUCacheBytes = 0;
    uint64 GPUCacheBudget = 0;
    uint64 GPUUseCounter = 0;
    uint64 GPUCacheRevision = 0;
    bool bGPUCacheDirty = false;
    TAtomic<bool> bGPUCacheMaintenanceQueued{false};
    /** Number of tiles uploaded from RAM to the GPU since the session began; a fast growth means the GPU budget is too small. */
    TAtomic<uint64> GPUUploadCount{0};

    TArray64<TSharedPtr<FTile>> Tiles;
    TArray64<FOdysseyTileId> FreeTiles;

    TMultiMap<FIoHash, TSharedPtr<FTileData>> HashToTileData;
    TArray<TSharedPtr<FTile>> PendingTilesToReadBack;

    FOdysseyTileManagerStats Stats;
    TUniquePtr<FOdysseyTileDiskCacheDirectory> CacheSession;
};
