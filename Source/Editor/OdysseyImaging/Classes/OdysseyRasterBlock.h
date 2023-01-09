// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include "OdysseyPerformanceMode.h"
#include <ULIS>

#include "OdysseyRasterBlock.generated.h"

USTRUCT()
struct FOdysseyRasterBlockTile
{
    GENERATED_BODY()

    UPROPERTY()
    int X;

    UPROPERTY()
    int Y;

    UPROPERTY()
    int Width;

    UPROPERTY()
    int Height;

    UPROPERTY()
    FString Hash;

    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> mBlock; //Loaded on demand from DDC or BulkData
    UE::Serialization::FEditorBulkData mBulkData; //Allows serializing the tile on disk when saving
    bool mCacheFromBulkData = true;
};

UCLASS()
class ODYSSEYIMAGING_API UOdysseyRasterBlock : public UObject
{
    GENERATED_BODY()
    
public:
    /**
     * @brief Delegate called when the block pixels content changed
     */
    DECLARE_MULTICAST_DELEGATE_TwoParams(FOnPixelsChanged, const TArray<::ULIS::FRectI>&, bool)

public:
    // Construction / Destruction
    ~UOdysseyRasterBlock();
    UOdysseyRasterBlock();

public:
    /**
     * @brief Called to copy rects directly into the block tiles
     * Also needs to be called when working on the result of GetBlock() to validate any modification
     *  
     * @param iBlock
     * @param iRects
     * @param iIsInteractive
     */
    void Update(TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> iBlock, const TArray<::ULIS::FRectI>& iRects, bool iIsInteractive);

    /**
    * Validates all interactive updates, and send a non interactive OnPixelsChanged event
    * Does not update any tile
    */
    void Commit();
    
    /**
     * @brief Initialize the Block with given ULIS block
     */
    void SetBlock(TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> iBlock);

    /**
     * @brief Retrieves a Block which allows pixel edition through ULIS
     * call block->Dirty(rects) to validate rectangles into the rasterblock
     * 
     * @return TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> 
     */
    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> GetBlock();

    /**
     * @brief Get the Performance Mode of the RasterBlock
     * Usually performance mode is set to "Speed" when we start editing the block
     * and it is set to Memory when we stop editing the block
     * 
     * Speed = better reactivity when drawing, but more memory consuming
     * Memory = less memory consumption, but slower
     * 
     * @return ePerformanceMode 
     */
    eOdysseyPerformanceMode GetPerformanceMode();

    /**
     * @brief Set the Global Performance Mode of the block
     * Usually performance mode is set to "Speed" when we start editing the block
     * and it is set to Memory when we stop editing the block
     * 
     * Speed = better reactivity when drawing, but more memory consuming
     * Memory = less memory consumption, but slower
     * 
     * @param iPerformanceMode 
     */
    void SetPerformanceMode(eOdysseyPerformanceMode iPerformanceMode);

    /**
     * @brief Get the block Width
     * 
     * @return int 
     */
    int GetWidth() const;
    
    /**
     * @brief Get the block Height
     * 
     * @return int 
     */
    int GetHeight() const;
    
    /**
     * @brief Get the block Format
     * 
     * @return ::ULIS::eFormat 
     */
    ::ULIS::eFormat GetFormat() const;

    //Called when the block tiles content changed
    FOnPixelsChanged& OnPixelsChanged();

    //If the result of GetBlock() is kept in memory by someone
    //OnBlockChanged will be called to inform that the block is no longer valid
    //(because width/height/format changed) and must be retrieved again by that "someone"
    FSimpleMulticastDelegate& OnBlockChanged();

private:
    //Renders all tiles into the given block
    TArray<::ULIS::FEvent> RenderTiles(TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> iBlock);

    //Renders the tile at iTileIndex into the given block
    TArray<::ULIS::FEvent> RenderTile(int iTileIndex, TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> iBlock);

    //Retrieve the block corresponding to tile at iTileIndex
    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> GetTileBlock(int iTileIndex);
    
    //Loads and returns a block from cache corresponding to the tile at iTileIndex
    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> LoadTileBlockFromCache(int iTileIndex);

    //Loads and return a block from bulkdata corresponding to the tile at iTileIndex
    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> LoadTileBlockFromBulkData(int iTileIndex);

    //Saves the block corresponding to the tile at iTileIndex into the cache and removes the block from memory
    void SaveTileBlocksToCache();

    //Called when tiles have changed
    void TilesChanged(const TSet<int>& iTileIndexes, bool iIsInteractive);

public:
    //UObject overrides

    /**
     * @brief Serialize this object
     * 
     * @param Ar 
     */
    virtual void Serialize(FArchive& Ar) override;

    /**
     * @brief Called after an undo affected this object
     * 
     * @param Ar 
     */
    virtual void PostTransacted(const FTransactionObjectEvent& iTransactionEvent) override;

private:
    UPROPERTY()
    int Width;

    UPROPERTY()
    int Height;

    UPROPERTY()
    int Format;

    UPROPERTY()
    TArray<FOdysseyRasterBlockTile> Tiles;

    TWeakPtr<::ULIS::FBlock, ESPMode::ThreadSafe> mBlock; //Loaded on demand from cache, can be destroyed at any time if noone keeps a sharedptr on it
    TSet<int> mUndoneTiles;
    TSet<int> mInteractivelyChangedTiles;

    //Called when the block tiles content changed
    FOnPixelsChanged mOnPixelsChanged;

    //If the result of GetBlock() is kept in memory by someone
    //OnBlockChanged will be called to inform that the block is no longer valid
    //(because width/height/format changed) and must be retrieved again by that "someone"
    FSimpleMulticastDelegate mOnBlockChanged;

    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> mBlockRetainerForSpeed;
};

