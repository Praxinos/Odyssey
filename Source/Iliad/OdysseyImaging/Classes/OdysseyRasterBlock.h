// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "CoreMinimal.h"

#include "Misc/OdysseyHandle.h"
#include "ULISInvalidTileMap.h"
#include "OdysseyRasterBlockUndo.h"
#include "OdysseyDiskCache.h"
#include "Serialization/EditorBulkData.h"
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
    FOdysseyRasterBlock(UObject* iOwner, int iWidth, int iHeight, ::ULIS::eFormat  iFormat);

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
     * @brief Returns the block unique ID
     *
     * @return const FGuid&
     */
    const FGuid& GetId() const;

    /**
     * @brief Get the rect of this block
     *
     * @return ::ULIS::FRectI
     */
    ::ULIS::FRectI GetRect() const;

    /**
    * Needs to be called in owner PostDuplicate()
    */
    void PostDuplicate();

    /**
     * @brief Converts the block to the given size(crop) and format
     *
     * @param iWidth
     * @param iHeight
     * @param iFormat
     */
    void ConvertTo(int iWidth, int iHeight, ::ULIS::eFormat iFormat);

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

private:
    //--- Block Caching / Loading
    void InvalidateCache();

    //Loads and return a block from bulkdata
    bool LoadBlockFromBulkData(FUniqueBuffer& oBuffer);
    static void CleanupBlock(uint8* iData, void* iInfo);

public:
    /**
     * @brief Serialize this object
     *
     * @param Ar
     */
    void Serialize(FArchive& Ar);

private:
    //Import/Export
    friend class FOdysseyRasterBlockExport;
    friend class FOdysseyRasterBlockImport;
    friend class FOdysseyRasterBlockUndo;
    friend class FOdysseyRasterBlockMutator;

private:
    UObject* mOwner;
    int mWidth = -1;
    int mHeight = -1;
    ::ULIS::eFormat mFormat = ::ULIS::Format_RGBA8;

    FGuid mId; //unique ID identifying the block

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

    //
    // OPTIMIZATIONS
    //

    FCriticalSection mMutex;
    TSharedPtr<FCriticalSection> mConstructionDestructionMutex;

    struct FBlockData
    {
        bool mIsCacheInvalid;
        FUniqueBuffer mBuffer;
        FGuid mId;
        TSharedPtr<FCriticalSection> mConstructionDestructionMutex;
    };

    FBlockData* mBlockData;
};


ODYSSEYIMAGING_API FArchive& operator<<(FArchive& Ar, FOdysseyRasterBlock& iRasterBlock);
