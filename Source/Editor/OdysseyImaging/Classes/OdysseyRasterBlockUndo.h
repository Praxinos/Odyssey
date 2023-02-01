// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Misc/Change.h"
//#include "OdysseyRasterBlock.h"

class FOdysseyRasterBlock;

class ODYSSEYIMAGING_API FOdysseyRasterBlockUndoBuilder
{
public:
    void StoreUndo(TSharedPtr<FOdysseyRasterBlock> iRasterBlock);

private:
    void BuildUndoData(TSharedPtr<FOdysseyRasterBlock> iRasterBlock, TArray<uint8>& oData, TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> iBlock, const TMap<FIntPoint, TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe>>& iOriginalTileBlocks);
    void BuildRedoData(TSharedPtr<FOdysseyRasterBlock> iRasterBlock, TArray<uint8>& oData, TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> iBlock, const TMap<FIntPoint, TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe>>& iOriginalTileBlocks);
    void SaveUndoToCache(TSharedPtr<FOdysseyRasterBlock> iRasterBlock, const FString& iUndoId, const FString& iRedoId);
    void SaveToCache(const FString& iId, const TArray<uint8>& iData);
};

class ODYSSEYIMAGING_API FOdysseyRasterBlockUndo : public FCommandChange
{
public:
    ~FOdysseyRasterBlockUndo();
    FOdysseyRasterBlockUndo();

    

	/** Makes the change to the object */
    //REDO
	virtual void Apply( UObject* Object ) override;

	/** Reverts change to the object */
    //UNDO
	virtual void Revert( UObject* Object ) override;

	/** Describes this change (for debugging) */
	virtual FString ToString() const override;

public:
    FOdysseyRasterBlockUndo(const FGuid& iUndoId, const FGuid& iRedoId, TSharedPtr<FOdysseyRasterBlock> iBlock);

private:
    void LoadUndoFromCache(const FString& iId);
    void RemoveUndoFromCache(const FString& iId);

private:
    FGuid mUndoId;
    FGuid mRedoId;
    TWeakPtr<FOdysseyRasterBlock> mRasterBlock;
};
