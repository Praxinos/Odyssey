// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"

#include "OdysseyInvalidTileMap.h"
#include "OdysseyBlendParameters.h"
#include "OdysseyRasterBlockMutator.h"
#include "Misc/ITransaction.h"

#include <ULIS>

class FOdysseyRasterBlock;

class ODYSSEYPAINTEREDITOR_API FOdysseyPaintEngine
{

public:
    // Destructor
    virtual ~FOdysseyPaintEngine();

    //Constructor
    FOdysseyPaintEngine();

public:
    // Setters
    void RasterBlock(TSharedPtr<FOdysseyRasterBlock> iRasterBlock);
    void SetMaskBlock(TSharedPtr<::ULIS::FBlock> iMaskBlock);

public:
    // Getters
    TSharedPtr<::ULIS::FBlock> PaintBlock();
    TSharedPtr<FOdysseyRasterBlock> GetRasterBlock() const;

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

    void OnRasterBlockChanged(const TArray<::ULIS::FRectI>& iRects);

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

    TSharedPtr<::ULIS::FBlock> mPaintBlock; // The Block containing only the modified tiles
    TSharedPtr<::ULIS::FBlock> mMaskBlock;

    //Options
    FOdysseyBlendParameters             mPreviousBlendParameters;

    //Internal
    TArray<::ULIS::FRectI>              mInvalidRects;
    bool mIsBeforeUndoBound;
    bool mIsChangingBlock = false;
};
