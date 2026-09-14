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

    /** Several FTiles can share a similaire Index in the Tiles Array
     * This can happen when a tile is detected as empty and leaves its place to be reused by another tile
     * Generation allows us to track how many times the FTile has been reused
     * Which also allows us to differentiate Tiles indentified by the same Index but different Generation
    */
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

    FOdysseyTileId CreateTile(const FIoHash& InHash, const FCompressedBuffer& InCompressedBuffer);

    /**
     * Returns a FTextureRHIRef containing the Tile's Texture
     * The returned FTextureRHIRef can be invalid, indicating an empty texture
     * Please check FTextureRHIRef::IsValid() before using it
     */
    TFuture<FTextureRHIRef> GetTileTexture(FOdysseyTileId InTileId, uint32 InTileSize, EPixelFormat InTileFormat) const;

    bool GetTileCompressedBuffer(FOdysseyTileId InTileId, FCompressedBuffer& OutBuffer);
    bool GetTileCompressedBuffer(FOdysseyTileId InTileId, FCompressedBuffer& OutBuffer) const;

    bool GetTileHash(FOdysseyTileId InTileId, FIoHash& OutHash);

    FOdysseyTileManagerStats& GetStats();

    /**
     * Waits until all tiles are properly cached on disk and evicted if needed
     */
    void WaitUntilAllTilesAreCached(bool InEvictTiles = true);

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
                Cached
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
        //When reading a Tile, its data must stay consistent
        FCriticalSection Mutex;

        FOdysseyTileId Id;
        TPromise<FTextureRHIRef> TempTexturePromise;
        TFuture<FTextureRHIRef> TempTexture;
        TSharedPtr<FRHIGPUTextureReadback> GPUReadBack;
        TSharedPtr<FRHIGPUBufferReadback> GPUIsEmptyReadBack;
        TPromise<TSharedPtr<FTileData>> TileDataPromise;
        TFuture<TSharedPtr<FTileData>> TileData;
    };

    template<typename... ArgsType>
    TSharedPtr<FTile> CreateNewTile(ArgsType&&... Args);

    void TryLoadTileFromReadBack(TSharedPtr<FTile> InTile);
    void LoadTileFromReadBack(TSharedPtr<FTile> InTile);

    void CompressTile(TSharedPtr<FTileData> InTileData);
    void CacheTileOnDisk(TSharedPtr<FTileData> InTileData);

    static void AddWriteTilePass(FRDGBuilder& GraphBuilder, FRDGTextureRef OutTexture, FSharedBuffer InBuffer);

private:
    // FTickableGameObject implementation
    virtual void Tick(float DeltaTime) override;
    virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(FOdysseyTileManager, STATGROUP_Tickables); }

private:
    FCriticalSection FreeTilesMutex;
    FCriticalSection HashToTileDataMutex;

    TArray64<TSharedPtr<FTile>> Tiles;
    TArray64<FOdysseyTileId> FreeTiles;

    TMultiMap<FIoHash, TSharedPtr<FTileData>> HashToTileData;
    TArray<TSharedPtr<FTile>> PendingTilesToReadBack;

    FOdysseyTileManagerStats Stats;
};
