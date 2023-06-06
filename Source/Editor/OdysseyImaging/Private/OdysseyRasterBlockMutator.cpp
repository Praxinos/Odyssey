// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyRasterBlockMutator.h"

#include "OdysseyRasterBlock.h"

FOdysseyRasterBlockMutator::~FOdysseyRasterBlockMutator()
{
    Commit();
}

FOdysseyRasterBlockMutator::FOdysseyRasterBlockMutator()
    : mRasterBlock(nullptr)
    , mInvalidTileMap()
    , mOriginalTileBlocks()
    , mRasterBlockUndoBuilder()
{

}

FOdysseyRasterBlockMutator::FOdysseyRasterBlockMutator(TSharedPtr<FOdysseyRasterBlock> iRasterBlock)
    : mRasterBlock(iRasterBlock)
    , mInvalidTileMap(64, iRasterBlock->GetWidth(), iRasterBlock->GetHeight())
    , mOriginalTileBlocks()
    , mRasterBlockUndoBuilder()
    , mHandle(iRasterBlock->Preload())
{

}

void
FOdysseyRasterBlockMutator::SetRasterBlock(TSharedPtr<FOdysseyRasterBlock> iRasterBlock)
{
    Commit();
    mRasterBlock = iRasterBlock;
    if ( mRasterBlock )
    {
        mInvalidTileMap = FULISInvalidTileMap(64, mRasterBlock->GetWidth(), mRasterBlock->GetHeight());
        mHandle = iRasterBlock->Preload();
    }
    else
    {
        mInvalidTileMap = FULISInvalidTileMap();
        mHandle = nullptr;
    }
}

const FULISInvalidTileMap&
FOdysseyRasterBlockMutator::GetInvalidTileMap() const
{
    return mInvalidTileMap;
}

TSharedPtr<FOdysseyRasterBlock>
FOdysseyRasterBlockMutator::GetRasterBlock() const
{
    return mRasterBlock;
}

const TMap<FIntPoint, TSharedPtr<::ULIS::FBlock>>&
FOdysseyRasterBlockMutator::GetOriginalTileBlocks() const
{
    return mOriginalTileBlocks;
}

void
FOdysseyRasterBlockMutator::ResetTilesFromRects(const TArray<::ULIS::FRectI>& iRects)
{
    if (!mRasterBlock)
        return;

    //Retrieve intersections between rects
    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> block = mRasterBlock->GetBlock();
    if ( !block )
        return;

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(mRasterBlock->GetFormat());
    FULISInvalidTileMap invalidTileMap(64, mRasterBlock->GetWidth(), mRasterBlock->GetHeight());
    invalidTileMap.Invalidate(iRects);
    TArray<FIntPoint> invalidTiles = invalidTileMap.InvalidTiles();
    for (const FIntPoint& tile : invalidTiles)
    {
        if (!mOriginalTileBlocks.Contains(tile))
            continue;

        TSharedPtr<::ULIS::FBlock> originalBlock = mOriginalTileBlocks[tile];
        ULIS::FRectI rect = invalidTileMap.GetTileRect(tile);
        ctx.Copy( *originalBlock, *block, originalBlock->Rect(), rect.Position(), ::ULIS::FSchedulePolicy::AsyncCacheEfficient );
    }
    ctx.Finish();
}

void
FOdysseyRasterBlockMutator::EditTilesFromRects(const TArray<::ULIS::FRectI>& iRects, const FEditDelegate& iDelegate)
{
    if (!mRasterBlock)
        return;

    //Retrieve intersections between rects
    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> block = mRasterBlock->GetBlock();
    if ( !block )
        return;

    FULISInvalidTileMap invalidTileMap(64, mRasterBlock->GetWidth(), mRasterBlock->GetHeight());
    invalidTileMap.Invalidate(iRects);
    mInvalidTileMap.Invalidate(iRects);

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(mRasterBlock->GetFormat());
    TArray<FIntPoint> tileIndexes = invalidTileMap.InvalidTiles();
    int tileSize = invalidTileMap.TileSize();
    
    for (const FIntPoint& tileIndex : tileIndexes )
    {
        if (mOriginalTileBlocks.Contains(tileIndex))
            continue;

        TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> tileBlock = MakeShared<::ULIS::FBlock>(tileSize, tileSize, mRasterBlock->GetFormat());
        ctx.Copy(*block, *tileBlock, invalidTileMap.GetTileRect(tileIndex), ::ULIS::FVec2I(0), ::ULIS::FSchedulePolicy::AsyncCacheEfficient);
        mOriginalTileBlocks.Add(tileIndex, tileBlock);
    }
    ctx.Finish();

    iDelegate.ExecuteIfBound(invalidTileMap);
    ctx.Finish();

    mRasterBlock->OnBlockChanged().Broadcast(iRects); //always send at least one interactive event
}

void
FOdysseyRasterBlockMutator::Commit()
{
    if (!mRasterBlock)
        return;

    if (mOriginalTileBlocks.IsEmpty())
        return;

    
    mRasterBlock->mCleanupInfos->mIsCacheInvalid = true;
    mRasterBlock->OnBlockCommited().Broadcast(mInvalidTileMap.InvalidRects());
    mRasterBlockUndoBuilder.StoreUndo(*this);
    mInvalidTileMap.Clear();
    mOriginalTileBlocks.Empty();
}

void
FOdysseyRasterBlockMutator::Abort()
{
    if (!mRasterBlock)
        return;

    if (mOriginalTileBlocks.IsEmpty())
        return;

    TSharedPtr<::ULIS::FBlock> block = mRasterBlock->GetBlock();
    if ( !block )
        return;

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(block->Format());
    TArray<FIntPoint> tileIndexes;
    TArray<::ULIS::FRectI> rects;
    mOriginalTileBlocks.GetKeys(tileIndexes);
    for (const FIntPoint& tileIndex : tileIndexes)
    {
        ::ULIS::FRectI rect = mInvalidTileMap.GetTileRect(tileIndex);
        rects.Add(rect);
        ctx.Copy(*mOriginalTileBlocks[tileIndex], *block, mInvalidTileMap.GetTileRect(tileIndex), rect.Position(), ::ULIS::FSchedulePolicy::AsyncCacheEfficient);
    }
    ctx.Finish();

    mInvalidTileMap.Clear();
    mOriginalTileBlocks.Empty();
}