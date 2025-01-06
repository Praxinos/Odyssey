// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "OdysseyRasterBlockUndo.h"

#include "OdysseyRasterBlock.h"
#include "OdysseyRasterBlockMutator.h"
#include "ULISLoaderModule.h"
#include "DerivedDataCacheInterface.h"
#include "DerivedDataValue.h"
#include "DerivedDataRequestTypes.h"
#include "DerivedDataRequestOwner.h"
#include "DerivedDataCache.h"
#include "CoreTypes.h"

#define FOdysseyRasterBlockUndo_CACHE_NAME TEXT("OdysseyRasterBlockUndo")
#define FOdysseyRasterBlockUndo_CACHE_VERSION TEXT("C0E3215B0E9A4983B2C0C01131A37935")

void
FOdysseyRasterBlockUndoBuilder::StoreUndo(const FOdysseyRasterBlockMutator& iRasterBlockMutator)
{
    const TMap<FIntPoint, TSharedPtr<::ULIS::FBlock>>& originalTileBlocks = iRasterBlockMutator.GetOriginalTileBlocks();
    if ( originalTileBlocks.IsEmpty() )
        return;

    FGuid undoId = FGuid::NewGuid();
    FGuid redoId = FGuid::NewGuid();
    SaveUndoToCache(iRasterBlockMutator, undoId.ToString(), redoId.ToString());
    GUndo->StoreUndo(iRasterBlockMutator.GetRasterBlock()->GetOwner(), MakeUnique<FOdysseyRasterBlockUndo>(undoId, redoId, iRasterBlockMutator.GetRasterBlock()));
}

void
FOdysseyRasterBlockUndoBuilder::BuildRedoData(const FOdysseyRasterBlockMutator& iRasterBlockMutator, TArray<uint8>& oData, TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> iBlock)
{
    const TMap<FIntPoint, TSharedPtr<::ULIS::FBlock>>& originalTileBlocks = iRasterBlockMutator.GetOriginalTileBlocks();
    if ( originalTileBlocks.IsEmpty() )
        return;

    const FULISInvalidTileMap& invalidTileMap = iRasterBlockMutator.GetInvalidTileMap();
    FMemoryWriter writer(oData); //allows us to save rectangles alongside the block

    TArray<FIntPoint> tileIndexes;
    originalTileBlocks.GetKeys(tileIndexes);

    int numTiles = tileIndexes.Num();
    writer << numTiles;
    for (const FIntPoint& tileIndex : tileIndexes)
    {
        ::ULIS::FRectI rect = invalidTileMap.GetTileRect(tileIndex);
        writer << rect.x;
        writer << rect.y;
        writer << rect.w;
        writer << rect.h;
    }

    //copy the rectangle in the block
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(iRasterBlockMutator.GetRasterBlock()->GetFormat());
    int tileSize = invalidTileMap.TileSize();
    int dataStart = oData.Num();//Do this before AddUninitialized() to keep track of where to write the block in memory
    oData.AddUninitialized(tileSize * tileSize * numTiles * iBlock->BytesPerPixel());
    for (int i = 0; i < tileIndexes.Num(); i++)
    {
        TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> blockToSave = MakeShared<::ULIS::FBlock>(oData.GetData() + dataStart, tileSize, tileSize, iRasterBlockMutator.GetRasterBlock()->GetFormat());
        ::ULIS::FRectI rect = invalidTileMap.GetTileRect(tileIndexes[i]);
        ctx.Copy(*iBlock, *blockToSave, rect, ::ULIS::FVec2I(0, 0), ::ULIS::FSchedulePolicy::AsyncCacheEfficient);
        dataStart += tileSize * tileSize * iBlock->BytesPerPixel();
        ctx.Finish();
    }
}

void
FOdysseyRasterBlockUndoBuilder::BuildUndoData(const FOdysseyRasterBlockMutator& iRasterBlockMutator, TArray<uint8>& oData, TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> iBlock)
{
    const TMap<FIntPoint, TSharedPtr<::ULIS::FBlock>>& originalTileBlocks = iRasterBlockMutator.GetOriginalTileBlocks();
    if ( originalTileBlocks.IsEmpty() )
        return;

    const FULISInvalidTileMap& invalidTileMap = iRasterBlockMutator.GetInvalidTileMap();
    FMemoryWriter writer(oData); //allows us to save rectangles alongside the block

    TArray<FIntPoint> tileIndexes;
    originalTileBlocks.GetKeys(tileIndexes);

    int numTiles = tileIndexes.Num();
    writer << numTiles;
    for (const FIntPoint& tileIndex : tileIndexes)
    {
        ::ULIS::FRectI rect = invalidTileMap.GetTileRect(tileIndex);
        writer << rect.x;
        writer << rect.y;
        writer << rect.w;
        writer << rect.h;
    }

    //copy the rectangle in the block
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(iRasterBlockMutator.GetRasterBlock()->GetFormat());
    int tileSize = invalidTileMap.TileSize();
    int dataStart = oData.Num();//Do this before AddUninitialized() to keep track of where to write the block in memory
    oData.AddUninitialized(tileSize * tileSize * numTiles * iBlock->BytesPerPixel());
    for (int i = 0; i < tileIndexes.Num(); i++)
    {
        TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> blockToSave = MakeShared<::ULIS::FBlock>(oData.GetData() + dataStart, tileSize, tileSize, iRasterBlockMutator.GetRasterBlock()->GetFormat());
        TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> tileBlock = originalTileBlocks[tileIndexes[i]];
        ctx.Copy(*tileBlock, *blockToSave, tileBlock->Rect(), ::ULIS::FVec2I(0, 0), ::ULIS::FSchedulePolicy::AsyncCacheEfficient);
        dataStart+= tileSize * tileSize * iBlock->BytesPerPixel();
        ctx.Finish();
    }
}

void
FOdysseyRasterBlockUndoBuilder::SaveUndoToCache(const FOdysseyRasterBlockMutator& iRasterBlockMutator, const FString& iUndoId, const FString& iRedoId)
{
    const TMap<FIntPoint, TSharedPtr<::ULIS::FBlock>>& originalTileBlocks = iRasterBlockMutator.GetOriginalTileBlocks();
    if ( originalTileBlocks.IsEmpty() )
        return;
    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> block = iRasterBlockMutator.GetRasterBlock()->GetBlock();
    if (!block)
        return;

    TArray<uint8> UndoData;
    TArray<uint8> RedoData;
    BuildUndoData(iRasterBlockMutator, UndoData, block);
    BuildRedoData(iRasterBlockMutator, RedoData, block);

    SaveToCache(iUndoId, UndoData);
    SaveToCache(iRedoId, RedoData);
}

void
FOdysseyRasterBlockUndoBuilder::SaveToCache(const FString& iId, const TArray<uint8>& iData)
{
    FString CacheKey = FDerivedDataCacheInterface::BuildCacheKey(
        FOdysseyRasterBlockUndo_CACHE_NAME,
        FOdysseyRasterBlockUndo_CACHE_VERSION, //a GUID identifying the version of the key
        iId
    );

    FSharedBuffer sharedBuffer = FSharedBuffer::MakeView(iData.GetData(), iData.Num());
    UE::DerivedData::FValue derivedDataValue = UE::DerivedData::FValue::Compress(sharedBuffer);

    //Store the tile in cache
    UE::DerivedData::FRequestOwner putOwner(UE::DerivedData::EPriority::Lowest);
    UE::DerivedData::GetCache().PutValue(
        {
            UE::DerivedData::FCachePutValueRequest
            {
                UE::FSharedString(TEXT("FOdysseyRasterBlock")),
                UE::DerivedData::ConvertLegacyCacheKey(CacheKey),
                MoveTemp(derivedDataValue),
                UE::DerivedData::ECachePolicy::StoreLocal
            }
        },
        putOwner
    );
    putOwner.KeepAlive();
}

FOdysseyRasterBlockUndo::~FOdysseyRasterBlockUndo()
{
    RemoveUndoFromCache(mUndoId.ToString());
    RemoveUndoFromCache(mRedoId.ToString());
}

FOdysseyRasterBlockUndo::FOdysseyRasterBlockUndo(const FGuid& iUndoId, const FGuid& iRedoId, TSharedPtr<FOdysseyRasterBlock> iRasterBlock)
    : mUndoId(iUndoId)
    , mRedoId(iRedoId)
    , mRasterBlock(iRasterBlock)
{
}

/** Makes the change to the object */
//REDO
void
FOdysseyRasterBlockUndo::Apply( UObject* Object )
{
    TSharedPtr<FOdysseyRasterBlock> rasterBlock = mRasterBlock.Pin();
    if (!rasterBlock)
        return;

    LoadUndoFromCache(mRedoId.ToString());
}

/** Reverts change to the object */
//UNDO
void
FOdysseyRasterBlockUndo::Revert( UObject* Object )
{
    TSharedPtr<FOdysseyRasterBlock> rasterBlock = mRasterBlock.Pin();
    if (!rasterBlock)
        return;

    LoadUndoFromCache(mUndoId.ToString());
}

/** Describes this change (for debugging) */
FString
FOdysseyRasterBlockUndo::ToString() const
{
    return TEXT("Odyssey Raster Block Undo/Redo");
}

void
FOdysseyRasterBlockUndo::LoadUndoFromCache(const FString& iId)
{
    TSharedPtr<FOdysseyRasterBlock> rasterBlock = mRasterBlock.Pin();
    if (!rasterBlock)
        return;

    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> block = rasterBlock->GetBlock();
    if ( !block )
        return;

    TArray<::ULIS::FRectI> rects;

    //Load Block from DDC
    FString CacheKey = FDerivedDataCacheInterface::BuildCacheKey(
        FOdysseyRasterBlockUndo_CACHE_NAME,
        FOdysseyRasterBlockUndo_CACHE_VERSION, //a GUID identifying the version of the key
        iId
    );

    UE::DerivedData::FRequestOwner getOwner(UE::DerivedData::EPriority::Blocking);
    UE::DerivedData::GetCache().GetValue(
        {
            UE::DerivedData::FCacheGetValueRequest
            {
                UE::FSharedString(TEXT("FOdysseyRasterBlock")),
                UE::DerivedData::ConvertLegacyCacheKey(CacheKey),
                UE::DerivedData::ECachePolicy::Local
            }
        },
        getOwner,
        [&, this](UE::DerivedData::FCacheGetValueResponse&& iResponse)
        {
            if (iResponse.Status != UE::DerivedData::EStatus::Ok)
                return;

            if ( !iResponse.Value.HasData() || iResponse.Value.GetRawSize() == 1 ) //assume the block is empty, see RemoveValueFromCache()
                return;

            FSharedBuffer rawData = iResponse.Value.GetData().Decompress();

            void* dataPtr = const_cast<void*>(rawData.GetData());

            FBufferReader reader(dataPtr, rawData.GetSize(), false, false);
            int numTiles = 0;
            reader << numTiles;

            rects.SetNumUninitialized(numTiles);
            for (int i = 0; i < numTiles; i++)
            {
                ::ULIS::FRectI& rect = rects[i];
                reader << rect.x;
                reader << rect.y;
                reader << rect.w;
                reader << rect.h;

            }

            int tileSize = 64;

            uint8* dataStart = static_cast<uint8*>(dataPtr) + reader.Tell();
            ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(rasterBlock->GetFormat());
            for (int i = 0; i < numTiles; i++)
            {
                TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> blockToLoad = MakeShared<::ULIS::FBlock>( dataStart, tileSize, tileSize, rasterBlock->GetFormat());
                const ::ULIS::FRectI& rect = rects[i];
                ctx.Copy(*blockToLoad, *block, blockToLoad->Rect(), ::ULIS::FVec2I(rect.x, rect.y), ::ULIS::FSchedulePolicy::AsyncCacheEfficient);
                dataStart += tileSize * tileSize * blockToLoad->BytesPerPixel();
                ctx.Finish();
            }
        }
    );
    getOwner.Wait();

    rasterBlock->InvalidateCache();
    rasterBlock->OnBlockChanged().Broadcast(rects);
    rasterBlock->OnBlockCommited().Broadcast(rects);
}

void
FOdysseyRasterBlockUndo::RemoveUndoFromCache(const FString& iId)
{
    FString CacheKey = FDerivedDataCacheInterface::BuildCacheKey(
        FOdysseyRasterBlockUndo_CACHE_NAME,
        FOdysseyRasterBlockUndo_CACHE_VERSION, //a GUID identifying the version of the key
        iId
    );

    uint8 dummy = 0;
    FSharedBuffer sharedBuffer = FSharedBuffer::MakeView(&dummy, 1);
    UE::DerivedData::FValue derivedDataValue = UE::DerivedData::FValue::Compress(sharedBuffer);

    //Store the tile in cache
    UE::DerivedData::FRequestOwner putOwner(UE::DerivedData::EPriority::Lowest);
    UE::DerivedData::GetCache().PutValue(
        {
            {
                UE::FSharedString(), //Not needed
                UE::DerivedData::ConvertLegacyCacheKey(CacheKey),
                MoveTemp(derivedDataValue),
                UE::DerivedData::ECachePolicy::StoreLocal //Use "StoreLocal" instead of "Local" to store and override existing value
            }
        },
        putOwner
    );
    putOwner.KeepAlive();
}
