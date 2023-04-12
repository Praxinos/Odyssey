// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include "ULISInvalidTileMap.h"
#include "OdysseyBlendParameters.h"
#include "OdysseyRasterBlockMutator.h"
#include "Misc/ITransaction.h"

#include <ULIS>

class FOdysseyRasterBlock;

class ODYSSEYPAINTENGINE_API FOdysseyPaintEngine
{
public:
    // Delegates

    // Any type of painting delegates (stroke, clear, fill, etc...)
    DECLARE_DELEGATE_RetVal_OneParam(FOdysseyBlendParameters, FOnPreUpdate, const FOdysseyBlendParameters& iBlendParameters);

public:
    // Destructor
    virtual ~FOdysseyPaintEngine();

    //Constructor
    FOdysseyPaintEngine();

public:
    // Setters

    // Sets the Block on which the Paint Engine will draw
    void RasterBlock(TSharedPtr<FOdysseyRasterBlock> iRasterBlock);

public:
    // Getters

    //Returns the PaintBlock (Stroke Block), use with caution as it is mostly used by the brush in an asynchronous way
    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> PaintBlock();

    // Delegates
    FOnPreUpdate& OnPreUpdateDelegate() { return mOnPreUpdateDelegate; }

public:
    // Update Edited block according to PaintBlock content without commiting
    void Update(const FOdysseyBlendParameters& iBlendParameters);

    // Commit the changes by blending the paintblock over the original editedblock
    // And resets the PaintBlock
    void Commit(const FOdysseyBlendParameters& iBlendParameters);
    
    // Resets the PaintBlock
    void Abort();

private:
    // Internal Blocks Management
    static void PaintBlockChanged( const ::ULIS::FBlock* iBlock, const ::ULIS::FRectI* iRects, const uint32 iNumRects, void* iInfo );

    // Internal Methods
    void ClearPaintBlock();

    // Blends PaintBlock on OriginalBlock and stores the result in EditedBlock
    bool UpdateEditedBlock(const FOdysseyBlendParameters& iBlendParameters);

    void OnEditedBlockChanged(const TArray<::ULIS::FRectI>& iRects);

    void OnBlockPtrChanged();
    
    FOdysseyBlendParameters AdjustBlendParameters(const FOdysseyBlendParameters& iBlendParameters);

    void OnBeforeRedoUndo( const FTransactionContext& TransactionContext );

private:
    //Blocks
    TSharedPtr<FOdysseyRasterBlock> mRasterBlock;
    FOdysseyRasterBlockMutator mRasterBlockMutator;

    //TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> mEditedBlock; // The Block to edit (mPaintBlock over mOriginalBlock)
    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> mPaintBlock; // The Block containing only the modified tiles
    //TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> mOriginalBlock; // The Block containing the edited block before being edited

    //Options
    FOdysseyBlendParameters             mPreviousBlendParameters;

    // Delegates
    FOnPreUpdate                        mOnPreUpdateDelegate;
    
    //Internal
    TArray<::ULIS::FRectI>              mInvalidRects;
    //FULISInvalidTileMap                 mInvalidMap;
};
