// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyRasterBlock.h"

#include "Compression/OodleDataCompression.h"
#include "DerivedDataCache.h"
#include "DerivedDataCacheInterface.h"
#include "DerivedDataRequestOwner.h"
#include "DerivedDataRequestTypes.h"
#include "Misc/TransactionObjectEvent.h"
#include "OdysseyRectUtils.h"
#include "OdysseyPerformanceMode.h"
#include "ULISEventBuilder.h"
#include "ULISLoaderModule.h"

#include <chrono>

#define FOdysseyRasterBlockTile_CACHE_NAME TEXT("OdysseyRasterBlockTile")
#define FOdysseyRasterBlockTile_CACHE_VERSION TEXT("A6ED84107BAD11EDA1EB0242AC120002")
static int baseTileWidth = 64;
static int baseTileHeight = 64;

UOdysseyRasterBlock::~UOdysseyRasterBlock()
{
}

UOdysseyRasterBlock::UOdysseyRasterBlock()
{
}

void
UOdysseyRasterBlock::SetBlock(TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> iBlock)
{
    if (iBlock == mBlock)
        return;

    mBlock = iBlock;

    Width = iBlock->Width();
    Height = iBlock->Height();
    Format = iBlock->Format();

    //clear tiles
    Tiles.Empty();
    mInteractivelyChangedTiles.Empty();

    int lastTileX = (Width - 1) / baseTileWidth;
    int lastTileY = (Height - 1) / baseTileHeight;

    for (int y = 0; y <= lastTileY; y++)
    {
        int tileHeight = FMath::Min(Height - (y * baseTileHeight), baseTileHeight);
        if (tileHeight <= 0)
            continue;

        for ( int x = 0; x <= lastTileX; x++ )
        {
            int tileWidth = FMath::Min(Width - (y * baseTileWidth), baseTileWidth);
            if (tileWidth <= 0)
                continue;

            Tiles.Add({x * baseTileWidth, y * baseTileHeight, tileWidth, tileHeight, "", nullptr });
        }
    }
    UpdateTiles(iBlock, {iBlock->Rect()});
    SaveTileBlocksToCache();

    OnBlockChanged().Broadcast();
}

TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe>
UOdysseyRasterBlock::GetBlock()
{
    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> block = mBlock.Pin();
    if (!block)
    {
        //Load Block from DDC
        block = MakeShared<::ULIS::FBlock>(Width, Height, (::ULIS::eFormat)Format);

        //Render to block
        ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext((::ULIS::eFormat)Format);
        RenderTiles(block);
        ctx.Finish();
        mBlock = block; //watch the loaded block
    }

    return block; //return the currently loaded block, the reveiver can release it whenever he wants
}

int
UOdysseyRasterBlock::GetWidth() const
{
    return Width;
}

int
UOdysseyRasterBlock::GetHeight() const
{
    return Height;
}

::ULIS::eFormat
UOdysseyRasterBlock::GetFormat() const
{
    return (::ULIS::eFormat)Format;
}

UOdysseyRasterBlock::FOnPixelsChanged&
UOdysseyRasterBlock::OnPixelsChanged()
{
    return mOnPixelsChanged;
}

FSimpleMulticastDelegate&
UOdysseyRasterBlock::OnBlockChanged()
{
    return mOnBlockChanged;
}

TArray<::ULIS::FEvent> 
UOdysseyRasterBlock::RenderTiles(TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> iBlock)
{
    TArray<::ULIS::FEvent> renderTilesEvents;
    
    //Get all tile blocks
    for (int i = 0; i < Tiles.Num(); i++)
    {
        TArray<::ULIS::FEvent> renderTileEvents = RenderTile(i, iBlock);
        renderTilesEvents.Append(renderTileEvents);
    }

    return renderTilesEvents;
}

TArray<::ULIS::FEvent> 
UOdysseyRasterBlock::RenderTile(int iTileIndex, TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> iBlock)
{
    TArray<::ULIS::FEvent> renderTileEvents;

    if (iTileIndex < 0 || iTileIndex >= Tiles.Num())
        return renderTileEvents;

    const FOdysseyRasterBlockTile& tile = Tiles[iTileIndex];
    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> block = GetTileBlock(iTileIndex);
    
    //If tile is considered empty, don't bother looking for a block and clear the tile section directly
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext((::ULIS::eFormat)Format);
    if (!block)
    {
        ::ULIS::FEvent clearEvent = FULISEventBuilder().RetainBlock(block).Build();
        ctx.Clear(*iBlock, ::ULIS::FRectI::FromXYWH(tile.X, tile.Y, tile.Width, tile.Height), ::ULIS::FSchedulePolicy::AsyncCacheEfficient, 0, nullptr, &clearEvent);
        renderTileEvents.Add(clearEvent);
        return renderTileEvents;
    }

    ::ULIS::FEvent copyEvent = FULISEventBuilder().RetainBlock(block).Build();
    ctx.Copy(*block, *iBlock, ::ULIS::FRectI::Auto, ::ULIS::FVec2I(tile.X, tile.Y), ::ULIS::FSchedulePolicy::AsyncCacheEfficient, 0, nullptr, &copyEvent);
    renderTileEvents.Add(copyEvent);
    return renderTileEvents;
}

TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe>
UOdysseyRasterBlock::GetTileBlock(int iTileIndex)
{
    if (iTileIndex < 0 || iTileIndex >= Tiles.Num())
        return nullptr;

    const FOdysseyRasterBlockTile& tile = Tiles[iTileIndex];
    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> block = tile.mBlock;
    if (!block && tile.Hash == "")
        return nullptr;

    if (!block)
        block = LoadTileBlockFromCache(iTileIndex);

    if (!block)
        block = LoadTileBlockFromBulkData(iTileIndex);

    return block; //return the currently loaded block, the reveiver can release it whenever he wants
}

TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe>
UOdysseyRasterBlock::LoadTileBlockFromCache(int iTileIndex)
{
    if (iTileIndex < 0 || iTileIndex >= Tiles.Num())
        return nullptr;

    const FOdysseyRasterBlockTile& tile = Tiles[iTileIndex];

    if (tile.Hash == "")
        return nullptr;

    //Load Block from DDC
    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> block = nullptr;

    FString CacheKey = FDerivedDataCacheInterface::BuildCacheKey(
		FOdysseyRasterBlockTile_CACHE_NAME,
        FOdysseyRasterBlockTile_CACHE_VERSION, //a GUID identifying the version of the key
		tile.Hash
	);

    UE::DerivedData::FRequestOwner AsyncOwner(UE::DerivedData::EPriority::Blocking);
    UE::DerivedData::GetCache().GetValue(
		{
            {
                GetPathName(),
                UE::DerivedData::ConvertLegacyCacheKey(CacheKey),
                UE::DerivedData::ECachePolicy::Local
            }
        },
		AsyncOwner,
		[&, this](UE::DerivedData::FCacheGetValueResponse&& iResponse)
        {
            FSharedBuffer rawData = iResponse.Value.GetData().Decompress();
            if (iResponse.Status != UE::DerivedData::EStatus::Ok)
                return;

            block = MakeShared<::ULIS::FBlock>(tile.Width, tile.Height, (::ULIS::eFormat)Format);
            FUniqueBuffer uniqueBuffer = FUniqueBuffer::MakeView(block->Bits(), block->BytesTotal());
            uniqueBuffer.GetView().CopyFrom(rawData);
        }
    );
    AsyncOwner.Wait();
    return block;
}

TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe>
UOdysseyRasterBlock::LoadTileBlockFromBulkData(int iTileIndex)
{
    if (iTileIndex < 0 || iTileIndex >= Tiles.Num())
        return nullptr;

    const FOdysseyRasterBlockTile& tile = Tiles[iTileIndex];

    if (tile.Hash == "")
        return nullptr;

    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> block = MakeShared<::ULIS::FBlock>(tile.Width, tile.Height, (::ULIS::eFormat)Format);

    FSharedBuffer buffer = tile.mBulkData.GetPayload().Get();
    FUniqueBuffer uniqueBuffer = FUniqueBuffer::MakeView(block->Bits(), block->BytesTotal());
    uniqueBuffer.GetView().CopyFrom(buffer);

    return block;
}

void
UOdysseyRasterBlock::SaveTileBlocksToCache()
{
    //Build the list of keys we need to save, so that we check and save them only once
    TSet<FString> hashes;
    for ( int i = 0; i < Tiles.Num(); i++ )
    {
        FOdysseyRasterBlockTile& tile = Tiles[i];
        if ( tile.Hash == "" )
        {
            tile.mBlock = nullptr; //ensure tile mblock is released
            continue;
        }

        if ( hashes.Contains(tile.Hash) )
        {
            tile.mBlock = nullptr; //ensure tile mblock is released
            continue;
        }

        hashes.Add(tile.Hash);

        FString CacheKey = FDerivedDataCacheInterface::BuildCacheKey(
            FOdysseyRasterBlockTile_CACHE_NAME,
            FOdysseyRasterBlockTile_CACHE_VERSION, //a GUID identifying the version of the key
            tile.Hash
        );

        TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> blockToSave = tile.mBlock;
        if ( !blockToSave && tile.mCacheFromBulkData )
            blockToSave = LoadTileBlockFromBulkData(i);

        tile.mCacheFromBulkData = false;

        //look if the tile is already in cache
        UE::DerivedData::FRequestOwner getOwner(UE::DerivedData::EPriority::Lowest);
        UE::DerivedData::GetCache().GetValue(
            {
                {
                    GetPathName(),
                    UE::DerivedData::ConvertLegacyCacheKey(CacheKey),
                    UE::DerivedData::ECachePolicy::Query | UE::DerivedData::ECachePolicy::SkipData| UE::DerivedData::ECachePolicy::SkipMeta, //that's the policy to use when asking if a data is in cache or not
                }
            },
            getOwner,
            [&, this, blockToSave](UE::DerivedData::FCacheGetValueResponse&& iResponse)
            {
                //Save block only if it is not found in cache
                bool isAlreadyInCache = (iResponse.Status == UE::DerivedData::EStatus::Ok);
                if (isAlreadyInCache)
                    return;

                if ( !blockToSave )
                    return;

                FSharedBuffer sharedBuffer = FSharedBuffer::MakeView(blockToSave->Bits(), blockToSave->BytesTotal());
                UE::DerivedData::FValue derivedDataValue = UE::DerivedData::FValue::Compress(sharedBuffer);

                //Store the tile in cache
                UE::DerivedData::FRequestOwner putOwner(UE::DerivedData::EPriority::Lowest);
                UE::DerivedData::GetCache().PutValue(
                    {
                        {
                            iResponse.Name,
                            iResponse.Key,
                            MoveTemp(derivedDataValue),
                            UE::DerivedData::ECachePolicy::Local
                        }
                    },
                    putOwner
                );
                putOwner.KeepAlive();
            }
        );
        getOwner.KeepAlive();

        tile.mBlock = nullptr;
    }
}

TSet<int>
UOdysseyRasterBlock::UpdateTiles(TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> iBlock, const TArray<::ULIS::FRectI>& iRects)
{
    TSet<int> tileIndexes;

    if (!iBlock)
        return tileIndexes;

    int numTilesX = ((Width - 1) / baseTileWidth) + 1;
    int numTilesY = ((Height - 1) / baseTileHeight) + 1;
    ::ULIS::FRectI blockRect = ::ULIS::FRectI::FromXYWH(0, 0, Width, Height);

    //Render rects to tiles
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext((::ULIS::eFormat)Format);
    TArray<::ULIS::FRectI> rects = ::OdysseyRectUtils::ToNonOverlappingRects(iRects);

    //Find affected tiles
    for ( const ::ULIS::FRectI& rect : rects )
    {
        ::ULIS::FRectI safeRect = blockRect & rect;
        safeRect.Sanitize();
        if ( safeRect.Area() <= 0 )
            continue;

        int startX = safeRect.x / baseTileWidth;
        int startY = safeRect.y / baseTileHeight;
        int endX = (safeRect.x + safeRect.w - 1) / baseTileWidth;
        int endY = (safeRect.y + safeRect.h - 1) / baseTileHeight;

        for ( int y = startY; y <= endY; y++ )
        {
            for ( int x = startX; x <= endX; x++ )
            {
                int tileIndex = y * numTilesX + x;
                tileIndexes.Add(tileIndex);
            }
        }
    }

    //Prepare affected tile blocks and copy rects
    for ( const int& i : tileIndexes )
    {
        FOdysseyRasterBlockTile& tile = Tiles[i];
        tile.mBlock = GetTileBlock(i); //load block in memory

        ::ULIS::FEvent eventClear;
        if ( !tile.mBlock )
        {
            tile.mBlock = MakeShared<::ULIS::FBlock>(tile.Width, tile.Height, (::ULIS::eFormat)Format);
            ctx.Clear(*tile.mBlock, ::ULIS::FRectI::Auto, ::ULIS::FSchedulePolicy::AsyncCacheEfficient, 0, nullptr, &eventClear);
        }
        else
        {
            eventClear = ::ULIS::FEvent::NoOP();
        }

        ::ULIS::FRectI tileRect = ::ULIS::FRectI::FromXYWH(tile.X, tile.Y, tile.Width, tile.Height);
        for ( const ::ULIS::FRectI& rect : rects )
        {
            ::ULIS::FRectI safeRect = tileRect & rect;
            safeRect.Sanitize();
            if ( safeRect.Area() <= 0 )
                continue;

            ctx.Copy(*iBlock, *tile.mBlock, safeRect, safeRect.Position() - tileRect.Position(), ::ULIS::FSchedulePolicy::AsyncCacheEfficient, 1, &eventClear);
        }
    }

    ctx.Finish();

    //refresh tiles hashes
    for (const int& i : tileIndexes)
    {
        FOdysseyRasterBlockTile& tile = Tiles[i];
        tile.Hash = LexToString(FIoHash::HashBuffer(tile.mBlock->Bits(), tile.mBlock->BytesTotal()));
    }

    //refresh cached block if needed
    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> cachedBlock = mBlock.Pin();
    if (cachedBlock && iBlock != cachedBlock) //if the given block is already the cached block, we don't have to refresh it
    {
        for (const int& i : tileIndexes)
        {
            RenderTile(i, cachedBlock);
        }
        ctx.Finish();
    }

    return tileIndexes;
}

void
UOdysseyRasterBlock::Update(TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> iBlock, const TArray<::ULIS::FRectI>& iRects, bool iIsInteractive)
{
    TSet<int> tileIndexes = UpdateTiles(iBlock, iRects);
    if (tileIndexes.Num() <= 0)
        return;

    TilesChanged(tileIndexes, iIsInteractive);
}

void
UOdysseyRasterBlock::Commit()
{
    if ( mInteractivelyChangedTiles.Num() < 0 )
        return;

    TArray<::ULIS::FRectI> rects;
    for ( const int& i : mInteractivelyChangedTiles )
    {
        const FOdysseyRasterBlockTile& tile = Tiles[i];
        rects.Add(::ULIS::FRectI::FromXYWH(tile.X, tile.Y, tile.Width, tile.Height));
    }
    OnPixelsChanged().Broadcast(rects, false);
    mInteractivelyChangedTiles.Empty();
}

void
UOdysseyRasterBlock::TilesChanged(const TSet<int>& iTileIndexes, bool iIsInteractive)
{
    TArray<::ULIS::FRectI> rects;
    for (const int& i : iTileIndexes)
    {
        const FOdysseyRasterBlockTile& tile = Tiles[i];
        rects.Add(::ULIS::FRectI::FromXYWH(tile.X, tile.Y, tile.Width, tile.Height));   
    }
    OnPixelsChanged().Broadcast(rects, true); //always send at least one interactive event
    mInteractivelyChangedTiles.Append(iTileIndexes);
    
    if ( !iIsInteractive )
        Commit();
}

void
UOdysseyRasterBlock::Serialize(FArchive& Ar)
{
    //When undoing, before loading anything keep current block hashes in memory to be able to detect which tiles changes
    TArray<FString> prevHashes;
    if ( Ar.IsTransacting() && Ar.IsLoading() )
    {
        for (int i = 0; i < Tiles.Num(); i++)
        {
            const FOdysseyRasterBlockTile& tile = Tiles[i];
            prevHashes.Add(tile.Hash);
        }
    }

    //Load/Save all UPROPERTIES
	Super::Serialize(Ar);

    if ( Ar.IsTransacting() || !Ar.IsPersistent() )
    {
        if ( Ar.IsSaving() )
        {
            SaveTileBlocksToCache();
        }
        else if ( Ar.IsLoading() )
        {
            if (prevHashes.Num() == Tiles.Num())
            {
                for (int i = 0; i < Tiles.Num(); i++)
                {
                    const FString& prevHash = prevHashes[i];
                    FOdysseyRasterBlockTile& tile = Tiles[i];

                    if (tile.Hash != prevHash )
                    {
                        mUndoneTiles.Add(i);
                        tile.mBlock = nullptr;
                    }
                }
            }
        }
    }
    else
    {
        if ( Ar.IsSaving() )
        {
            // Force to load block from cache, as this can be called when mBlock is unsynchronized with cache
            // Everything we save/transact must be the cached version
            for (int i = 0; i < Tiles.Num(); i++)
            {
                FOdysseyRasterBlockTile& tile = Tiles[i];
                if (tile.Hash == "")
                    continue;

                TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> block = GetTileBlock(i);
                if (!block)
                    continue;

                FSharedBuffer sharedBuffer = FSharedBuffer::MakeView(block->Bits(), block->BytesTotal());

                //Updates the payload
                //The payload stored in mBulkData will then be removed from memory once mBulkData.Serialize() is called
                tile.mBulkData.UpdatePayload(sharedBuffer, this);
            
                /**
                 * @brief Here is a simple explanation of ECompressedBufferCompressor values
                 * see http://www.radgametools.com/oodlecompressors.htm
                 * 
                 * NotSet = 0, //No Compression
                 * Selkie = 1, //Ultra-fast decompression, compression ratio > zlib but < lzma
                 * Mermaid = 2, //Slower than selkie, faster than zlib/lzma, slightly better compression ratio than selkie but < lzma
                 * Kraken  = 3, //Slower than Mermaid, faster than zlib/lzma, slightly better compression ratio than Mermaid but < lzma
                 * Leviathan = 4 //Slower than Kraken, faster than zlib/lzma, slightly better compression ratio than Kraken and slightly > lzma
                */

                //Zlib ration compression is enough, but speed is ultra important, so Selkie compressor is what we need here
                //Unreal defaults for FCompressedBuffer is ECompressedBufferCompressor::Mermaid, ECompressedBufferCompressionLevel::VeryFast
                //But we will use custom values to balance performance at its best between compression tim, decompression time and size
                
                //Needs to be called everytime UpdatePayload is called to ensure the correct compression is selected
                tile.mBulkData.SetCompressionOptions(ECompressedBufferCompressor::Selkie, FOodleDataCompression::ECompressionLevel::Normal);
                
                //Eric: I don't understand what bAllowRegister is
                //but it seems UTexture sets it to false on saving and on true on loading
                //so I'll do the same, but correct me if I'm wrong
                tile.mBulkData.Serialize(Ar, this, false /* bAllowRegister */);

                //No need to keep a tile block after that as it is saved, I guess
                tile.mBlock = nullptr;
            }
        }
        else if ( Ar.IsLoading() )
        {
            for (int i = 0; i < Tiles.Num(); i++)
            {
                FOdysseyRasterBlockTile& tile = Tiles[i];
                if (tile.Hash == "")
                    continue;
                
                tile.mBulkData.Serialize(Ar, this);
            }
        }
    }
}

void
UOdysseyRasterBlock::PostTransacted(const FTransactionObjectEvent& iTransactionEvent)
{
    if ( iTransactionEvent.GetEventType() != ETransactionObjectEventType::UndoRedo )
        return;

    const TArray<FName>& changedPropertyNames = iTransactionEvent.GetChangedProperties();
    bool blockChanged = false;
    for ( const FName& propertyName : changedPropertyNames )
    {
        if ( propertyName == "Width")
            blockChanged = true;
        if ( propertyName == "Height")
            blockChanged = true;
        if ( propertyName == "Format")
            blockChanged = true;
    }

    if (blockChanged)
    {
        mUndoneTiles.Empty();
        mInteractivelyChangedTiles.Empty();
        mBlock = nullptr; //release the block, the user will be responsible to retrieve the block again via GetBlock()
        OnBlockChanged().Broadcast();
        return;
    }

    //If their is a block, now is the time to refresh the tiles that has been changed
    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> block = mBlock.Pin();
    if (block)
    {
        ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext((::ULIS::eFormat)Format);
        for (const int& i : mUndoneTiles)
        {
            RenderTile(i, block);
        }
        ctx.Finish();
    }

    TilesChanged(mUndoneTiles, false); //inform that the block changed
    mUndoneTiles.Empty();
}

void
UOdysseyRasterBlock::SetPerformanceMode(eOdysseyPerformanceMode iPerformanceMode)
{
    /**
     * When performance is set to Speed, we load the block in memory to avoid
     * unwanted loading times.
     * 
     * When performance is set to Memory, we stop retaining the block in memory
     */

    switch(iPerformanceMode)
    {
        case eOdysseyPerformanceMode::Speed:
            mBlockRetainerForSpeed = GetBlock();
        break;

        case eOdysseyPerformanceMode::Memory:
        case eOdysseyPerformanceMode::Shutdown:
            mBlockRetainerForSpeed = nullptr;
        break;
    }
}