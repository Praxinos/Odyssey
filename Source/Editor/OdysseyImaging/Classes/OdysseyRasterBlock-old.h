// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#if 0

#include "CoreMinimal.h"

#include "Misc/OdysseyHandle.h"
#include "ULISInvalidTileMap.h"
#include <ULIS>

#include "OdysseyRasterBlock.generated.h"

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

UCLASS()
class ODYSSEYIMAGING_API UOdysseyRasterBlock : public UObject
{
    GENERATED_BODY()

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
    ~UOdysseyRasterBlock();
    UOdysseyRasterBlock();

public:
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
     * @brief Called to copy rects directly into the block tiles
     * Also needs to be called when working on the result of GetBlock() to validate any modification
     *  
     * @param iBlock
     * @param iRects
     * @param iIsInteractive
     */
    void Invalidate(const TArray<::ULIS::FRectI>& iRects, bool iIsInteractive);

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

    void SaveUndoToCache(const FString& iId);

    bool LoadUndoFromCache(const FString& iId);

    static void CleanupBlock(uint8* iData, void* iInfo);

    void ResetEditableBlock();
public:
    //UObject overrides

    /**
     * @brief Serialize this object
     * 
     * @param Ar 
     */
    virtual void Serialize(FArchive& Ar) override;

private:
    UPROPERTY()
    FGuid Id; //unique ID identifying the block

    UPROPERTY()
    int Width;

    UPROPERTY()
    int Height;

    UPROPERTY()
    int Format;

    //The stable block for which edition is finished
    TWeakPtr<::ULIS::FBlock, ESPMode::ThreadSafe> mBlock; //Loaded on demand from cache, can be destroyed at any time if noone keeps a sharedptr on it

    //The unstable block which is currently being edited
    TWeakPtr<::ULIS::FBlock, ESPMode::ThreadSafe> mEditableBlock; //Loaded on demand from cache, can be destroyed at any time if noone keeps a sharedptr on it

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

private:
    friend class FOdysseyRasterBlockChange;
};

#endif