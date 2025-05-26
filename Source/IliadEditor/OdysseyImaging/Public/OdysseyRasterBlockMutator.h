// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "OdysseyRasterBlockUndo.h"
#include "OdysseyInvalidTileMap.h"
#include <ULIS>

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
    const FOdysseyInvalidTileMap& GetInvalidTileMap() const;
    const TMap<FIntPoint, TSharedPtr<::ULIS::FBlock>>& GetOriginalTileBlocks() const;

    void ResetTilesFromRects(const TArray<::ULIS::FRectI>& iRects);
    void EditTilesFromRects(const TArray<::ULIS::FRectI>& iRects, TFunction<TArray<::ULIS::FEvent>(TSharedPtr<::ULIS::FBlock>, const FOdysseyInvalidTileMap&)> iDelegate);

    void Copy(TSharedPtr<::ULIS::FBlock> iBlock, const TArray<::ULIS::FRectI>& iRects);

    void Commit();
    void Abort();

private:
    TSharedPtr<FOdysseyRasterBlock> mRasterBlock;
    FOdysseyInvalidTileMap mInvalidTileMap;
    TMap<FIntPoint, TSharedPtr<::ULIS::FBlock>> mOriginalTileBlocks;
    FOdysseyRasterBlockUndoBuilder mRasterBlockUndoBuilder;
    TSharedPtr<::ULIS::FBlock> mBlock;
    bool mStoreUndo;
};
