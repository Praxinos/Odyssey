// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include "Misc/OdysseyHandle.h"
#include "ULISInvalidTileMap.h"
#include "OdysseyRasterBlockUndo.h"
#include <ULIS>

//#include "OdysseyRasterBlock.generated.h"

/**
 * @brief Represents a raster block
 * 
 * The internal ULIS::Block cannot be directly accessed and modified.
 * The reason is linked to undo/redo system, for which we need to be able to know what was the content before Update().
 * Use GetUndoableBlock to get an undoable version (copy) of the block.
 * 
 * Once you did all the modifications call Update().
 * 
 * Update can be Interactive or Non-Interactive
 * Interactive means that the content of the block changed, but it is probably not the final content of the block
 * Non-Interactive will set the final content of the block
 * 
 * If you call Update in Interactive mode and you want to validate the content to be the final content afterwards.
 * Just call Commit().
 * 
 * Please avoid using keeping the block in an Interactive state for too long,
 * because it will only cache its data when a Non-Interactive Update() or Commit() is called.
 */

class ODYSSEYIMAGING_API FOdysseyRasterBlock : public TSharedFromThis<FOdysseyRasterBlock>
{
public:
    /**
     * @brief Delegate called when the block pixels content changed
     */
    DECLARE_MULTICAST_DELEGATE_TwoParams(FOnBlockChanged, const TArray<::ULIS::FRectI>&, bool)
    
    /**
     * @brief Delegate called when the edited block pixels content changed
     */
    //DECLARE_MULTICAST_DELEGATE_OneParam(FOnUndoableBlockChanged, const TArray<::ULIS::FRectI>&)

public:
    // Construction / Destruction
    ~FOdysseyRasterBlock();
    FOdysseyRasterBlock();
    FOdysseyRasterBlock(UObject* iOwner);

public:
    /**
     * @brief Get the owner object
     * 
     * @return int 
     */
    UObject* GetOwner() const;

    /**
    * Needs to be called in owner PostDuplicate()
    */
    void PostDuplicate();

    /**
     * @brief Returns the block unique ID
     * 
     * @return const FGuid& 
     */
    const FGuid& GetId() const;
    
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
    
    /**
     * @brief Initialize the Block with given ULIS block
     */
    void SetBlock(TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> iBlock);

    /**
     * @brief Returns wether the raster block is being edited
     * It is considered being edited if a UndoableBlock is held by someone
     * 
     * @return bool
     */
    //bool IsBeingEdited();

    /**
     * @brief Retrieves an undoable version of the internal block
     * 
     * @return TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> 
     */
    //TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> GetUndoableBlock();

    /**
     * @brief Retrieves the internal block, only for reading
     * Call GetUndoableBlock() to get an undoable block
     * 
     * @return TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> 
     */
    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> GetBlock();

    /**
     * @brief Retrieves the original tile blocks containing the pixels before any call to Invalidate()
     * 
     */
    //const TMap<FIntPoint, TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe>>& GetOriginalTileBlocks() const;

    /**
     * @brief Get the map of currently invalid tiles
     * 
     * @return const FULISInvalidTileMap& 
     */
    //const FULISInvalidTileMap& GetInvalidTileMap() const;

    /**
     * @brief Called to copy rects directly into the block tiles
     * Also needs to be called when working on the result of GetBlock() to validate any modification
     *  
     * @param iBlock
     * @param iRects
     * @param iIsInteractive
     */
    //void UpdateFromUndoableBlock(const TArray<::ULIS::FRectI>& iRects);

    /**
     * @brief Commits any changes done through UpdateFromUndoableBlock()
     * Creating an undo and resetting the invalidmap
     * 
     * @param iRects 
     */
    //void CommitUndoableBlock();
    
    /**
     * @brief Calls UpdateFromUndoableBlock() followed by CommitUndoableBlock()
     * 
     * @param iRects 
     */
    //void CommitUndoableBlock(const TArray<::ULIS::FRectI>& iRects);

    /**
     * @brief Resets the Undoable block to its state before Invalidate()
     * 
     */
    //void ResetUndoableBlock();

    /** 
     * @brief Preloads the block in memory, and keeps it in memory until the returned handle is destroyed
     */
    TSharedPtr<IOdysseyHandle> Preload();

    //Called when the block tiles content changed
    FOnBlockChanged& OnBlockChanged();

    //Called when the block tiles content changed
    //FOnUndoableBlockChanged& OnUndoableBlockChanged();

    //If the result of GetBlock() is kept in memory by someone
    //OnBlockPtrChanged will be called to inform that the block is no longer valid
    //(because width/height/format changed) and must be retrieved again by that "someone"
    FSimpleMulticastDelegate& OnBlockPtrChanged();

private:
    //--- Block Caching / Loading
    
    //Loads and returns a block from cache
    bool LoadBlockFromCache(TSharedRef<::ULIS::FBlock, ESPMode::ThreadSafe> oBlock, const FString& iId);

    //Loads and return a block from bulkdata
    bool LoadBlockFromBulkData(TSharedRef<::ULIS::FBlock, ESPMode::ThreadSafe> oBlock);

    static void CleanupBlock(uint8* iData, void* iInfo);
    //Saves the block corresponding to the tile at iTileIndex into the cache and removes the block from memory
    static void SaveBlockToCache(const ::ULIS::FBlock& iBlock, const FString& iId);

private:
    //Used by FOdysseyRasterBlockUndo to save load the block tiles to an undo cache
    //friend class FOdysseyRasterBlockUndo;
    //void SaveUndoToCache(const FString& iId);
    //bool LoadUndoFromCache(const FString& iId);
    //void RemoveUndoFromCache(const FString& iId);

public:
    /**
     * @brief Serialize this object
     * 
     * @param Ar 
     */
    void Serialize(FArchive& Ar);

private:
    UObject* mOwner;

    FGuid Id; //unique ID identifying the block
    int Width = -1;
    int Height = -1;
    int Format;

    //The stable block for which edition is finished
    TWeakPtr<::ULIS::FBlock, ESPMode::ThreadSafe> mBlock; //Loaded on demand from cache, can be destroyed at any time if noone keeps a sharedptr on it

    //The unstable block which is currently being edited
    //TWeakPtr<::ULIS::FBlock, ESPMode::ThreadSafe> mUndoableBlock; //Loaded on demand from cache, can be destroyed at any time if noone keeps a sharedptr on it

    //TMap<FIntPoint, TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe>> mOriginalTileBlocks;

    UE::Serialization::FEditorBulkData mBulkData; //Allows serializing the block on disk when saving
    //FULISInvalidTileMap mInvalidTileMap;

    // 
    // DELEGATES
    //

    //Called when the block tiles content changed
    FOnBlockChanged mOnBlockChanged;

    //Called when the undoable block pixels changed by an internal action (like undo)
    //FOnUndoableBlockChanged mOnUndoableBlockChanged;

    //If the result of GetBlock() is kept in memory by someone
    //mOnBlockPtrChanged will be called to inform that the block is no longer valid
    //(because width/height/format changed) and must be retrieved again by that "someone"
    FSimpleMulticastDelegate mOnBlockPtrChanged;

    // 
    // OPTIMIZATIONS
    //
    TWeakPtr<IOdysseyHandle> mPreloadHandle;

    //FOdysseyRasterBlockUndoBuilder mRasterBlockUndoBuilder;
};


ODYSSEYIMAGING_API FArchive& operator<<(FArchive& Ar, FOdysseyRasterBlock& iRasterBlock);
