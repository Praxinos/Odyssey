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
#include "OdysseyTileCacheSession.h"
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
    void RemoveCachedOnDisk(int64 InSizeOnDisk);

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
        FIntPoint Pos;
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
        bool IsValid() const;

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
        uint32 GetWidthInTiles() const { return WidthInTiles; };
        uint32 GetHeightInTiles() const { return HeightInTiles; };

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
        //EPixelFormat TileFormat;
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

            void WaitUntilCachedCompressed();
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
            static FCriticalSection Mutex;
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
        TUniquePtr<TPromise<TSharedPtr<FTileTextureHandle>>> TempTexturePromise;
        TSharedFuture<TSharedPtr<FTileTextureHandle>> TempTexture;

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

    FOdysseyTileId CreateTile(const FIoHash& InHash, const FCompressedBuffer& InCompressedBuffer);

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

private:
    template<typename... ArgsType>
    TSharedPtr<FTile> CreateNewTile(ArgsType&&... Args);

    void TryLoadTileFromReadBack(TSharedPtr<FTile> InTile);
    void LoadTileFromReadBack(TSharedPtr<FTile> InTile);

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
#if WITH_DEV_AUTOMATION_TESTS
    TAtomic<uint64> GPUUploadCount{0};
#endif

    TArray64<TSharedPtr<FTile>> Tiles;
    TArray64<FOdysseyTileId> FreeTiles;

    TMultiMap<FIoHash, TSharedPtr<FTileData>> HashToTileData;
    TArray<TSharedPtr<FTile>> PendingTilesToReadBack;

    FOdysseyTileManagerStats Stats;
    TUniquePtr<FOdysseyTileCacheSession> CacheSession;
};
