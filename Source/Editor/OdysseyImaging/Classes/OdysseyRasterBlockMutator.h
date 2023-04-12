// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyRasterBlockUndo.h"

class FOdysseyRasterBlock;

class ODYSSEYIMAGING_API FOdysseyRasterBlockMutator
{
public:
    DECLARE_DELEGATE_OneParam(FEditDelegate, const FULISInvalidTileMap&)

public:
    ~FOdysseyRasterBlockMutator();
    FOdysseyRasterBlockMutator();
    FOdysseyRasterBlockMutator(TSharedPtr<FOdysseyRasterBlock> iRasterBlock);

public:
    void SetRasterBlock(TSharedPtr<FOdysseyRasterBlock> iRasterBlock);

    TSharedPtr<FOdysseyRasterBlock> GetRasterBlock() const;
    const FULISInvalidTileMap& GetInvalidTileMap() const;
    const TMap<FIntPoint, TSharedPtr<::ULIS::FBlock>>& GetOriginalTileBlocks() const;

    void ResetTilesFromRects(const TArray<::ULIS::FRectI>& iRects);
    void EditTilesFromRects(const TArray<::ULIS::FRectI>& iRects, const FEditDelegate& iDelegate);

    void Commit();
    void Abort();

private:
    TSharedPtr<FOdysseyRasterBlock> mRasterBlock;
    FULISInvalidTileMap mInvalidTileMap;
    TMap<FIntPoint, TSharedPtr<::ULIS::FBlock>> mOriginalTileBlocks;
    FOdysseyRasterBlockUndoBuilder mRasterBlockUndoBuilder;
};