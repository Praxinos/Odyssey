// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyRasterBlockUndo.h"
#include "Misc/OdysseyHandle.h"
#include "ULISInvalidTileMap.h"

namespace ULIS { class FBlock; }
namespace ULIS { class FEvent; }
class FOdysseyRasterBlock;

class ODYSSEYIMAGING_API FOdysseyRasterBlockMutator
{
public:
    ~FOdysseyRasterBlockMutator();
    FOdysseyRasterBlockMutator(bool iStoreUndo = true);
    FOdysseyRasterBlockMutator(TSharedPtr<FOdysseyRasterBlock> iRasterBlock, bool iStoreUndo = true);

public:
    void SetRasterBlock(TSharedPtr<FOdysseyRasterBlock> iRasterBlock);

    TSharedPtr<FOdysseyRasterBlock> GetRasterBlock() const;
    const FULISInvalidTileMap& GetInvalidTileMap() const;
    const TMap<FIntPoint, TSharedPtr<::ULIS::FBlock>>& GetOriginalTileBlocks() const;

    void ResetTilesFromRects(const TArray<::ULIS::FRectI>& iRects);
    void EditTilesFromRects(const TArray<::ULIS::FRectI>& iRects, TFunction<TArray<::ULIS::FEvent>(TSharedPtr<::ULIS::FBlock>, const FULISInvalidTileMap&)> iDelegate);

    void Copy(TSharedPtr<::ULIS::FBlock> iBlock, const TArray<::ULIS::FRectI>& iRects);

    void Commit();
    void Abort();

private:
    TSharedPtr<FOdysseyRasterBlock> mRasterBlock;
    FULISInvalidTileMap mInvalidTileMap;
    TMap<FIntPoint, TSharedPtr<::ULIS::FBlock>> mOriginalTileBlocks;
    FOdysseyRasterBlockUndoBuilder mRasterBlockUndoBuilder;
    TSharedPtr<::ULIS::FBlock> mBlock;
    bool mStoreUndo;
};