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
 * Use GetEditableBlock to get an editable version (copy) of the block.
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
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnEditableBlockChanged, const TArray<::ULIS::FRectI>&)

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
     * It is considered being edited if a EditableBlock is held by someone
     * 
     * @return bool
     */
    bool IsBeingEdited();

    /**
     * @brief Retrieves the internal block, only for reading
     * Call CopyBlock() to get an editable block
     * 
     * @return TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> 
     */
    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> GetEditableBlock();

    /**
     * @brief Retrieves the internal block, only for reading
     * Call CopyBlock() to get an editable block
     * 
     * @return TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> 
     */
    const TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> GetBlock();

    /**
     * @brief Retrieves the original tile blocks containing the pixels before any call to Invalidate()
     * 
     */
    const TMap<FIntPoint, TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe>>& GetOriginalTileBlocks() const;

    /**
     * @brief Get the map of currently invalid tiles
     * 
     * @return const FULISInvalidTileMap& 
     */
    const FULISInvalidTileMap& GetInvalidTileMap() const;

    /**
     * @brief Called to copy rects directly into the block tiles
     * Also needs to be called when working on the result of GetBlock() to validate any modification
     *  
     * @param iBlock
     * @param iRects
     * @param iIsInteractive
     */
    void Invalidate(const TArray<::ULIS::FRectI>& iRects, bool iIsInteractive);

    /**
     * @brief Resets the editable block to its state before Invalidate()
     * 
     */
    void ResetEditableBlock();

    /** 
     * @brief Preloads the block in memory, and keeps it in memory until the returned handle is destroyed
     */
    TSharedPtr<IOdysseyHandle> Preload();

    //Called when the block tiles content changed
    FOnBlockChanged& OnBlockChanged();

    //Called when the block tiles content changed
    FOnEditableBlockChanged& OnEditableBlockChanged();

    //If the result of GetBlock() is kept in memory by someone
    //OnBlockPtrChanged will be called to inform that the block is no longer valid
    //(because width/height/format changed) and must be retrieved again by that "someone"
    FSimpleMulticastDelegate& OnBlockPtrChanged();

private:
    //--- Block Caching / Loading
    
    //Saves the block corresponding to the tile at iTileIndex into the cache and removes the block from memory
    void SaveBlockToCache(const ::ULIS::FBlock& iBlock, const FString& iId);
    
    //Loads and returns a block from cache
    bool LoadBlockFromCache(TSharedRef<::ULIS::FBlock, ESPMode::ThreadSafe> oBlock, const FString& iId);

    //Loads and return a block from bulkdata
    bool LoadBlockFromBulkData(TSharedRef<::ULIS::FBlock, ESPMode::ThreadSafe> oBlock);

    static void CleanupBlock(uint8* iData, void* iInfo);

private:
    //Used by FOdysseyRasterBlockUndo to save load the block tiles to an undo cache
    friend class FOdysseyRasterBlockUndo;
    void SaveUndoToCache(const FString& iId);
    bool LoadUndoFromCache(const FString& iId);
    void RemoveUndoFromCache(const FString& iId);

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
    int Width;
    int Height;
    int Format;

    //The stable block for which edition is finished
    TWeakPtr<::ULIS::FBlock, ESPMode::ThreadSafe> mBlock; //Loaded on demand from cache, can be destroyed at any time if noone keeps a sharedptr on it

    //The unstable block which is currently being edited
    TWeakPtr<::ULIS::FBlock, ESPMode::ThreadSafe> mEditableBlock; //Loaded on demand from cache, can be destroyed at any time if noone keeps a sharedptr on it

    TMap<FIntPoint, TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe>> mOriginalTileBlocks;

    UE::Serialization::FEditorBulkData mBulkData; //Allows serializing the block on disk when saving
    FULISInvalidTileMap mInvalidTileMap;

    // 
    // DELEGATES
    //

    //Called when the block tiles content changed
    FOnBlockChanged mOnBlockChanged;

    //Called when the editable block pixels changed by an internal action (like undo)
    FOnEditableBlockChanged mOnEditableBlockChanged;

    //If the result of GetBlock() is kept in memory by someone
    //mOnBlockPtrChanged will be called to inform that the block is no longer valid
    //(because width/height/format changed) and must be retrieved again by that "someone"
    FSimpleMulticastDelegate mOnBlockPtrChanged;

    // 
    // OPTIMIZATIONS
    //
    TWeakPtr<IOdysseyHandle> mPreloadHandle;

    FOdysseyRasterBlockUndoBuilder mRasterBlockUndoBuilder;
};


ODYSSEYIMAGING_API FArchive& operator<<(FArchive& Ar, TSharedPtr<FOdysseyRasterBlock>& iRasterBlock);
