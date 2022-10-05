// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include "OdysseyInvalidTileMap.h"
#include "OdysseyBlendParameters.h"

#include <ULIS>

class ODYSSEYPAINTENGINE_API FOdysseyPaintEngine
{
public:
    // Delegates

    // Any type of painting delegates (stroke, clear, fill, etc...)
    DECLARE_MULTICAST_DELEGATE(FOnBlockChanged);
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnCommit, const TArray<::ULIS::FRectI>& iChangedTiles);
    DECLARE_DELEGATE_RetVal_OneParam(FOdysseyBlendParameters, FOnPreUpdate, const FOdysseyBlendParameters& iBlendParameters);
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnUpdate, const TArray<::ULIS::FRectI>& iChangedTiles);
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnReset, const TArray<::ULIS::FRectI>& iChangedTiles);

public:
    // Destructor
    virtual ~FOdysseyPaintEngine();

    //Constructor
    FOdysseyPaintEngine();

public:
    // Setters

    // Sets the Block on which the Paint Engine will draw
    void Block(::ULIS::FBlock* iBlock);

public:
    // Getters

    //Returns the PaintBlock (Stroke Block), use with caution as it is mostly used by the brush in an asynchronous way
    ::ULIS::FBlock* PaintBlock();

    //Returns the OriginalBlock
    ::ULIS::FBlock* OriginalBlock();

    // Delegates
    FOnBlockChanged& OnBlockChangedDelegate() { return mOnBlockChangedDelegate; }
    FOnCommit& OnCommitDelegate() { return mOnCommitDelegate; }
    FOnPreUpdate& OnPreUpdateDelegate() { return mOnPreUpdateDelegate; }
    FOnUpdate& OnUpdateDelegate() { return mOnUpdateDelegate; }
    FOnReset& OnResetDelegate() { return mOnResetDelegate; }
    

public:
    // Update Edited block according to PaintBlokc content without commiting
    // And resets the PaintBlock
    void Update(const FOdysseyBlendParameters& iBlendParameters);

    // Commit the changes by blending the paintblock over the editedblock
    // And resets the PaintBlock
    void Commit(const FOdysseyBlendParameters& iBlendParameters);
    
    // Resets the PaintBlock
    void Reset();

private:
    // Internal Blocks Management
    static void PaintBlockChanged( const ::ULIS::FBlock* iBlock, const ::ULIS::FRectI* iRects, const uint32 iNumRects, void* iInfo );

    // Internal Methods
    void ClearPaintBlock();

    // Copies EditedBlock Rects to Original Block
    void CopyEditedBlockToOriginalBlock();

    // Blends PaintBlock on OriginalBlock and stores the result in EditedBlock
    bool UpdateEditedBlock(const FOdysseyBlendParameters& iBlendParameters);

private:
    //Blocks
    ::ULIS::FBlock*                     mEditedBlock; // The Block to edit (mPaintBlock over mOriginalBlock)
    ::ULIS::FBlock*                     mPaintBlock; // The Block containing only the modified tiles
    ::ULIS::FBlock*                     mOriginalBlock; // The Block containing the edited block before being edited
    
    //Options
    FOdysseyBlendParameters             mPreviousBlendParameters;

    // Delegates
    FOnBlockChanged                     mOnBlockChangedDelegate;
    FOnPreUpdate                        mOnPreUpdateDelegate;
    FOnUpdate                           mOnUpdateDelegate;
    FOnCommit                           mOnCommitDelegate;
    FOnReset                            mOnResetDelegate;

    //Internal
    FOdysseyInvalidTileMap              mPaintBlockInvalidTileMap;
    FOdysseyInvalidTileMap              mEditedBlockInvalidTileMap;
    FOdysseyInvalidTileMap              mResetInvalidTileMap;
};
