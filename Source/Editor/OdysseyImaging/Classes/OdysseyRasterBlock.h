// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include "Misc/OdysseyHandle.h"
#include "ULISInvalidTileMap.h"
#include "OdysseyRasterBlockUndo.h"
#include "OdysseyDiskCache.h"
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
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnBlockChanged, const TArray<::ULIS::FRectI>&)

    /**
     * @brief Delegate called when the block pixels content changed
     */
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnBlockCommited, const TArray<::ULIS::FRectI>&)

    /**
     * @brief Delegate called to apply a post process phase whenever a change is made to pixels
     * 
     * 1st Argument : OriginalBlocks, contains all the original pixels (pixels values before the edit being made) for each tilemap position.
     * 2nd Argument : InvalidTileMap, contains the tiles on which to apply the post process
     * 3rd Argument : WaitList, contains ULIS Events on which to wait before applying any post process
     */
    typedef TMap<FIntPoint, TSharedPtr<::ULIS::FBlock>> tPostProcessOriginalBlocks;
    DECLARE_DELEGATE_RetVal_ThreeParams(TArray<::ULIS::FEvent>, FPostProcess, const tPostProcessOriginalBlocks&, const FULISInvalidTileMap&, const TArray<::ULIS::FEvent>&)

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
     * @brief Get the Block object
     * 
     * @return TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> 
     */
    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> GetBlock();

    //Called when the block tiles content changed
    FOnBlockChanged& OnBlockChanged();

    //Called when the block tiles content changed
    FOnBlockChanged& OnBlockCommited();

    /**
     * @brief The post process to call after each block mutation
     */
    FPostProcess& PostProcess();

    //If the result of GetBlock() is kept in memory by someone
    //OnBlockPtrChanged will be called to inform that the block is no longer valid
    //(because width/height/format changed) and must be retrieved again by that "someone"
    FSimpleMulticastDelegate& OnBlockPtrChanged();

    void InvalidateCache();

private:
    //--- Block Caching / Loading
    
    //Loads and returns a block from cache
    //bool LoadBlockFromCache(TSharedRef<::ULIS::FBlock, ESPMode::ThreadSafe> oBlock, const FString& iId);

    //Loads and return a block from bulkdata
    bool LoadBlockFromBulkData(FUniqueBuffer& oBuffer);

    static void CleanupBlock(uint8* iData, void* iInfo);
    //Saves the block corresponding to the tile at iTileIndex into the cache and removes the block from memory
    //static void SaveBlockToCache(const ::ULIS::FBlock& iBlock, const FString& iId);

public:
    /**
     * @brief Serialize this object
     * 
     * @param Ar 
     */
    void Serialize(FArchive& Ar);

private:
    friend class FOdysseyRasterBlockMutator;

    //FOdysseyDiskCache mCache;
    //FSharedBuffer mSharedBuffer;

    UObject* mOwner;

    FGuid Id; //unique ID identifying the block
    int Width = -1;
    int Height = -1;
    int Format;

    //The stable block for which edition is finished
    TWeakPtr<::ULIS::FBlock, ESPMode::ThreadSafe> mBlock; //Loaded on demand from cache, can be destroyed at any time if noone keeps a sharedptr on it

    UE::Serialization::FEditorBulkData mBulkData; //Allows serializing the block on disk when saving

    // 
    // DELEGATES
    //

    //Called when the block tiles content changed
    FOnBlockChanged mOnBlockChanged;

    //Called when the block tiles content is Commited
    FOnBlockCommited mOnBlockCommited;

    FPostProcess mPostProcess;

    //If the result of GetBlock() is kept in memory by someone
    //mOnBlockPtrChanged will be called to inform that the block is no longer valid
    //(because width/height/format changed) and must be retrieved again by that "someone"
    FSimpleMulticastDelegate mOnBlockPtrChanged;

    // 
    // OPTIMIZATIONS
    //

    FCriticalSection mMutex;
    //bool mIsCacheInvalid;

    struct FBlockData
    {
        bool mIsCacheInvalid;
        FUniqueBuffer mBuffer;
        FGuid mId;
    };

    FBlockData* mBlockData;
};


ODYSSEYIMAGING_API FArchive& operator<<(FArchive& Ar, FOdysseyRasterBlock& iRasterBlock);
