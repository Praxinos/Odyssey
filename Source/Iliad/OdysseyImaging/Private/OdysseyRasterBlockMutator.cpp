// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "OdysseyRasterBlockMutator.h"

#include "OdysseyRasterBlock.h"
#include "Editor.h"
#include "ULISLoaderModule.h"

FOdysseyRasterBlockMutator::~FOdysseyRasterBlockMutator()
{
    Commit();
}

FOdysseyRasterBlockMutator::FOdysseyRasterBlockMutator(bool iStoreUndo)
    : mRasterBlock(nullptr)
    , mInvalidTileMap()
    , mOriginalTileBlocks()
    , mRasterBlockUndoBuilder()
    , mBlock(nullptr)
    , mStoreUndo(iStoreUndo)
{

}

FOdysseyRasterBlockMutator::FOdysseyRasterBlockMutator(TSharedPtr<FOdysseyRasterBlock> iRasterBlock, bool iStoreUndo)
    : mRasterBlock(iRasterBlock)
    , mInvalidTileMap(64, iRasterBlock->GetWidth(), iRasterBlock->GetHeight())
    , mOriginalTileBlocks()
    , mRasterBlockUndoBuilder()
    , mBlock(iRasterBlock->GetBlock())
    , mStoreUndo(iStoreUndo)
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
        mBlock = iRasterBlock->GetBlock();
    }
    else
    {
        mInvalidTileMap = FULISInvalidTileMap();
        mBlock = nullptr;
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
        ctx.Copy( *originalBlock, *mBlock, originalBlock->Rect(), rect.Position(), ::ULIS::FSchedulePolicy::AsyncCacheEfficient );
    }
    ctx.Finish();
}

void
FOdysseyRasterBlockMutator::EditTilesFromRects(const TArray<::ULIS::FRectI>& iRects, TFunction<TArray<::ULIS::FEvent>(TSharedPtr<::ULIS::FBlock>, const FULISInvalidTileMap&)> iDelegate)
{
    if (!mRasterBlock)
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

        ::ULIS::FRectI rect = mInvalidTileMap.GetTileRect(tileIndex);
        TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> tileBlock = MakeShared<::ULIS::FBlock>(tileSize, tileSize, mRasterBlock->GetFormat());
        ctx.Copy(*mBlock, *tileBlock, rect, ::ULIS::FVec2I(0), ::ULIS::FSchedulePolicy::AsyncCacheEfficient);
        mOriginalTileBlocks.Add(tileIndex, tileBlock);
    }
    ctx.Finish();

    if (iDelegate)
    {
        TArray<::ULIS::FEvent> delegateEvents = iDelegate(mBlock, invalidTileMap);
        if (mRasterBlock->PostProcess().IsBound())
            mRasterBlock->PostProcess().Execute(mOriginalTileBlocks, invalidTileMap, delegateEvents);
    }
    ctx.Finish();

    mRasterBlock->OnBlockChanged().Broadcast(invalidTileMap.InvalidRects()); //always send at least one interactive event
}

void
FOdysseyRasterBlockMutator::Commit()
{
    if (!mRasterBlock)
        return;

    if (mOriginalTileBlocks.IsEmpty())
        return;


    mRasterBlock->InvalidateCache();
    mRasterBlock->OnBlockCommited().Broadcast(mInvalidTileMap.InvalidRects());
    if ( mStoreUndo && GEditor->IsTransactionActive() )
    {
        mRasterBlockUndoBuilder.StoreUndo(*this);
    }
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

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(mBlock->Format());
    TArray<FIntPoint> tileIndexes;
    TArray<::ULIS::FRectI> rects;
    mOriginalTileBlocks.GetKeys(tileIndexes);
    ctx.Finish();
    for (const FIntPoint& tileIndex : tileIndexes)
    {
        if (!mOriginalTileBlocks.Contains(tileIndex))
            continue;

        TSharedPtr<::ULIS::FBlock> originalBlock = mOriginalTileBlocks[tileIndex];

        ::ULIS::FRectI rect = mInvalidTileMap.GetTileRect(tileIndex);
        rects.Add(rect);
        ctx.Copy(*originalBlock, *mBlock, originalBlock->Rect(), rect.Position(), ::ULIS::FSchedulePolicy::AsyncCacheEfficient);
    }
    ctx.Finish();

    mInvalidTileMap.Clear();
    mOriginalTileBlocks.Empty();
    mRasterBlock->OnBlockChanged().Broadcast(rects); //always send at least one interactive event
}

void
FOdysseyRasterBlockMutator::Copy(TSharedPtr<::ULIS::FBlock> iBlockToCopy, const TArray<::ULIS::FRectI>& iRects)
{
    EditTilesFromRects(iRects,
        [this, iBlockToCopy](TSharedPtr<::ULIS::FBlock> iBlock, const FULISInvalidTileMap& iTileMap) -> TArray<::ULIS::FEvent>
        {
            ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(iBlock->Format());
            ctx.ConvertFormat(*iBlockToCopy, *mBlock);
            ctx.Finish();
            return {};
        }
    );
}
