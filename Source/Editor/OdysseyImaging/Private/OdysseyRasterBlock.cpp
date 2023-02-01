// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyRasterBlock.h"

#include "Compression/OodleDataCompression.h"
#include "DerivedDataCache.h"
#include "DerivedDataCacheInterface.h"
#include "DerivedDataRequestOwner.h"
#include "DerivedDataRequestTypes.h"
#include "Misc/Change.h"
#include "Misc/ITransaction.h"
#include "Misc/TransactionObjectEvent.h"
#include "Misc/OdysseyHandle.h"
#include "OdysseyRectUtils.h"
#include "OdysseyPerformanceMode.h"
#include "ULISEventBuilder.h"
#include "ULISLoaderModule.h"

#define FOdysseyRasterBlock_CACHE_NAME TEXT("OdysseyRasterBlock")
#define FOdysseyRasterBlock_CACHE_VERSION TEXT("A6ED84107BAD11EDA1EB0242AC120002")

class FOdysseyRasterBlockPreloadHandle : public IOdysseyHandle
{
public:
    FOdysseyRasterBlockPreloadHandle(FOdysseyRasterBlock* iBlock)
        : mBlock(iBlock->GetBlock())
    {}

private:
    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> mBlock;
};

void
RemoveValueFromCache(const FString& iId)
{
    FString CacheKey = FDerivedDataCacheInterface::BuildCacheKey(
        FOdysseyRasterBlock_CACHE_NAME,
        FOdysseyRasterBlock_CACHE_VERSION, //a GUID identifying the version of the key
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
                UE::DerivedData::FSharedString(), //Not needed
                UE::DerivedData::ConvertLegacyCacheKey(CacheKey),
                MoveTemp(derivedDataValue),
                UE::DerivedData::ECachePolicy::StoreLocal //Use "StoreLocal" instead of "Local" to store and override existing value
            }
        },
        putOwner
    );
    putOwner.KeepAlive();
}

FOdysseyRasterBlock::~FOdysseyRasterBlock()
{
}

FOdysseyRasterBlock::FOdysseyRasterBlock()
    : mOwner(nullptr)
    , Id(FGuid::NewGuid())
{
}

FOdysseyRasterBlock::FOdysseyRasterBlock(UObject* iOwner)
    : mOwner(iOwner)
    , Id(FGuid::NewGuid())
{
}

UObject*
FOdysseyRasterBlock::GetOwner() const
{
    return mOwner;
}

int
FOdysseyRasterBlock::GetWidth() const
{
    return Width;
}

int
FOdysseyRasterBlock::GetHeight() const
{
    return Height;
}

::ULIS::eFormat
FOdysseyRasterBlock::GetFormat() const
{
    return (::ULIS::eFormat)Format;
}

void
FOdysseyRasterBlock::CleanupBlock(uint8* iData, void* iInfo)
{
    //WARNING: The original FOdysseyRasterBlock could not exist anymore
    FOdysseyRasterBlock* rasterBlock = static_cast<FOdysseyRasterBlock*>(iInfo);
    if ( !rasterBlock )
    {
        ::ULIS::OnCleanup_FreeMemory(iData, iInfo); //we have the responsability to delete the block data
        return;
    }

    ::ULIS::FBlock block(iData, rasterBlock->Width, rasterBlock->Height, rasterBlock->GetFormat());
    rasterBlock->SaveBlockToCache(block, rasterBlock->Id.ToString()); //TODO: maybe save only if version changed ?

    ::ULIS::OnCleanup_FreeMemory(iData, iInfo); //we have the responsability to delete the block data
}

void
FOdysseyRasterBlock::SetBlock(TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> iBlock)
{
    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> currentBlock = mBlock.Pin();
    if (iBlock == currentBlock)
        return;
    
    if (currentBlock)
    {
        //remove OnCleanup Callback
        currentBlock->OnCleanup(::ULIS::FOnCleanupData(&::ULIS::OnCleanup_FreeMemory));

        //Cleanup everything else
        mBlock = nullptr;
        Width = -1;
        Height = -1;
        mInvalidTileMap.Clear();
    }

    if (iBlock)
    {
        mBlock = iBlock;
        Width = iBlock->Width();
        Height = iBlock->Height();
        Format = iBlock->Format();
        iBlock->OnCleanup(::ULIS::FOnCleanupData(&FOdysseyRasterBlock::CleanupBlock, this));

        mInvalidTileMap = FULISInvalidTileMap(64, Width, Height);
    }

    //If an editableBlock was set, don't consider it as the editableBlock anymore
    //Let the user reload the block
    mEditableBlock = nullptr;
    
     mOnBlockPtrChanged.Broadcast();
}

bool
FOdysseyRasterBlock::IsBeingEdited()
{
    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> editableBlock = mEditableBlock.Pin();
    return !!editableBlock; //returns true if editableblock is valid (don't use IsValid() as it can be wrong sometimes)
}

TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe>
FOdysseyRasterBlock::GetEditableBlock()
{
    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> editableBlock = mEditableBlock.Pin();
    if ( editableBlock )
        return editableBlock;

    //Create a copy of the internal block
    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> block = GetBlock();
    if (!block)
    {
        UE_LOG(LogTemp, Warning, TEXT("No block available. Did you forget to call SetBlock() ?") );
        return nullptr;
    }

    editableBlock = MakeShared<::ULIS::FBlock>(Width, Height, (::ULIS::eFormat)Format);

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext((::ULIS::eFormat)Format);
    ctx.Copy(*block, *editableBlock, ::ULIS::FRectI::Auto, ::ULIS::FVec2I(0), ::ULIS::FSchedulePolicy::AsyncCacheEfficient);
    ctx.Finish();

    mEditableBlock = editableBlock; //Keep Weak Reference

    return editableBlock;
}

const TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe>
FOdysseyRasterBlock::GetBlock()
{
    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> block = mBlock.Pin();
    if ( block )
        return block;

    if ( Width <= 0 || Height <= 0 )
        return nullptr;

    //Load Block from DDC
    block = MakeShared<::ULIS::FBlock>(Width, Height, (::ULIS::eFormat)Format);
    block->OnCleanup(::ULIS::FOnCleanupData(&FOdysseyRasterBlock::CleanupBlock, this));
    mBlock = block; //watch the loaded block

    if ( LoadBlockFromCache(block.ToSharedRef(), Id.ToString()) )
        return block;

    if ( LoadBlockFromBulkData(block.ToSharedRef()) )
        return block;

    return block; //return the currently loaded block, the reveiver can release it whenever he wants
}

const TMap<FIntPoint, TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe>>&
FOdysseyRasterBlock::GetOriginalTileBlocks() const
{
    return mOriginalTileBlocks;
}

const FULISInvalidTileMap&
FOdysseyRasterBlock::GetInvalidTileMap() const
{
    return mInvalidTileMap;
}

void
FOdysseyRasterBlock::Invalidate(const TArray<::ULIS::FRectI>& iRects, bool iIsInteractive)
{
    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> block = GetBlock();
    if ( !block )
        return;

    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> editableBlock = mEditableBlock.Pin();
    if ( !editableBlock )
        return;
    
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext((::ULIS::eFormat)Format);

    //Non-Interactive update
    mInvalidTileMap.Invalidate(iRects);

    //Save original tiles
    TArray<FIntPoint> tileIndexes = mInvalidTileMap.InvalidTiles();
    int tileSize = mInvalidTileMap.TileSize();
    //const TArray<::ULIS::FRectI>& rects = mInvalidTileMap.InvalidRects();
    for (const FIntPoint& tileIndex : tileIndexes )
    {
        if (mOriginalTileBlocks.Contains(tileIndex))
            continue;

        TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> tileBlock = MakeShared<::ULIS::FBlock>(tileSize, tileSize, (::ULIS::eFormat)Format);
        ctx.Copy(*block, *tileBlock, mInvalidTileMap.GetTileRect(tileIndex), ::ULIS::FVec2I(0), ::ULIS::FSchedulePolicy::AsyncCacheEfficient);

        mOriginalTileBlocks.Add(tileIndex, tileBlock);
    }
    ctx.Finish();

    //Copy editableBlock to block
    for (const ::ULIS::FRectI& rect : iRects)
    {
        ctx.Copy(*editableBlock, *block, rect, rect.Position(), ::ULIS::FSchedulePolicy::AsyncCacheEfficient);
    }
    ctx.Finish();

    //Send Interactive Update event
    if (iRects.Num() > 0)
        OnBlockChanged().Broadcast(iRects, true); //always send at least one interactive event

    //Non-Interactive Event
    if (!iIsInteractive)
    {
        OnBlockChanged().Broadcast(mInvalidTileMap.InvalidRects(), false);
        mRasterBlockUndoBuilder.StoreUndo(AsShared());
        mInvalidTileMap.Clear();
        mOriginalTileBlocks.Empty();
    }
}

TSharedPtr<IOdysseyHandle>
FOdysseyRasterBlock::Preload()
{
    TSharedPtr<IOdysseyHandle> handle = mPreloadHandle.Pin();
    if (handle)
        return handle;

    handle = MakeShared<FOdysseyRasterBlockPreloadHandle>(this);
    mPreloadHandle = handle;
    return handle;
}

FOdysseyRasterBlock::FOnBlockChanged&
FOdysseyRasterBlock::OnBlockChanged()
{
    return mOnBlockChanged;
}

FOdysseyRasterBlock::FOnEditableBlockChanged&
FOdysseyRasterBlock::OnEditableBlockChanged()
{
    return mOnEditableBlockChanged;
}

FSimpleMulticastDelegate&
FOdysseyRasterBlock::OnBlockPtrChanged()
{
    return mOnBlockPtrChanged;
}

void
FOdysseyRasterBlock::SaveBlockToCache(const ::ULIS::FBlock& iBlock, const FString& iId)
{
    FString CacheKey = FDerivedDataCacheInterface::BuildCacheKey(
        FOdysseyRasterBlock_CACHE_NAME,
        FOdysseyRasterBlock_CACHE_VERSION, //a GUID identifying the version of the key
        iId
    );

    FSharedBuffer sharedBuffer = FSharedBuffer::MakeView(iBlock.Bits(), iBlock.BytesTotal());
    UE::DerivedData::FValue derivedDataValue = UE::DerivedData::FValue::Compress(sharedBuffer);

    //Store the tile in cache
    UE::DerivedData::FRequestOwner putOwner(UE::DerivedData::EPriority::Lowest);
    UE::DerivedData::GetCache().PutValue(
        {
            {
                TEXT("FOdysseyRasterBlock"),
                UE::DerivedData::ConvertLegacyCacheKey(CacheKey),
                MoveTemp(derivedDataValue),
                UE::DerivedData::ECachePolicy::StoreLocal //Use "StoreLocal" instead of "Local" to store and override existing value
            }
        },
        putOwner
    );
    putOwner.KeepAlive();
}

bool
FOdysseyRasterBlock::LoadBlockFromCache(TSharedRef<::ULIS::FBlock, ESPMode::ThreadSafe> oBlock, const FString& iId)
{
    bool success = false;

    //Load Block from DDC
    FString CacheKey = FDerivedDataCacheInterface::BuildCacheKey(
		FOdysseyRasterBlock_CACHE_NAME,
        FOdysseyRasterBlock_CACHE_VERSION, //a GUID identifying the version of the key
		iId
	);

    UE::DerivedData::FRequestOwner getOwner(UE::DerivedData::EPriority::Blocking);
    UE::DerivedData::GetCache().GetValue(
		{
            {
                TEXT("FOdysseyRasterBlock"),
                UE::DerivedData::ConvertLegacyCacheKey(CacheKey),
                UE::DerivedData::ECachePolicy::Local
            }
        },
		getOwner,
		[&, this](UE::DerivedData::FCacheGetValueResponse&& iResponse)
        {
            if (iResponse.Status != UE::DerivedData::EStatus::Ok)
                return;
    
            if ( !iResponse.Value.HasData() || iResponse.Value.GetRawSize() == 1) //assume the block is empty, see RemoveValueFromCache()
                return;

            FSharedBuffer rawData = iResponse.Value.GetData().Decompress();
            FUniqueBuffer uniqueBuffer = FUniqueBuffer::MakeView(oBlock->Bits(), oBlock->BytesTotal());
            uniqueBuffer.GetView().CopyFrom(rawData);
            success = true;
        }
    );
    getOwner.Wait();
    return success;
}

bool
FOdysseyRasterBlock::LoadBlockFromBulkData(TSharedRef<::ULIS::FBlock, ESPMode::ThreadSafe> oBlock)
{
    FSharedBuffer buffer = mBulkData.GetPayload().Get();
    FUniqueBuffer uniqueBuffer = FUniqueBuffer::MakeView(oBlock->Bits(), oBlock->BytesTotal());
    uniqueBuffer.GetView().CopyFrom(buffer);

    return true;
}

void
FOdysseyRasterBlock::Serialize(FArchive& Ar)
{
    //Load/Save all UPROPERTIES
	//Super::Serialize(Ar);

    Ar << Id; //unique ID identifying the block
    Ar << Width;
    Ar << Height;
    Ar << Format;

    if ( Ar.IsTransacting() || !Ar.IsPersistent() )
        return;

    if ( Ar.IsSaving() )
    {
        TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> block = GetBlock();
        if ( !block )
            return;

        FSharedBuffer sharedBuffer = FSharedBuffer::MakeView(block->Bits(), block->BytesTotal());

        //Updates the payload
        //The payload stored in mBulkData will then be removed from memory once mBulkData.Serialize() is called
        mBulkData.UpdatePayload(sharedBuffer, mOwner);
    
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
        mBulkData.SetCompressionOptions(ECompressedBufferCompressor::Selkie, FOodleDataCompression::ECompressionLevel::Normal);
        
        //Eric: I don't understand what bAllowRegister is
        //but it seems UTexture sets it to false on saving and on true on loading
        //so I'll do the same, but correct me if I'm wrong
        mBulkData.Serialize(Ar, mOwner, false /* bAllowRegister */);
    }
    else if ( Ar.IsLoading() )
    {       
        mBulkData.Serialize(Ar, mOwner);
        mInvalidTileMap = FULISInvalidTileMap(64, Width, Height);
        RemoveValueFromCache(Id.ToString());
    }
}

void
FOdysseyRasterBlock::ResetEditableBlock()
{
    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> editableBlock = mEditableBlock.Pin();
    if ( !editableBlock )
        return;

    if (mOriginalTileBlocks.IsEmpty())
        return;

    mInvalidTileMap.Clear();

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext((::ULIS::eFormat)Format);
    TArray<FIntPoint> tileIndexes;
    TArray<::ULIS::FRectI> rects;
    mOriginalTileBlocks.GetKeys(tileIndexes);
    for (const FIntPoint& tileIndex : tileIndexes)
    {
        ::ULIS::FRectI rect = mInvalidTileMap.GetTileRect(tileIndex);
        rects.Add(rect);
        ctx.Copy(*mOriginalTileBlocks[tileIndex], *editableBlock, mInvalidTileMap.GetTileRect(tileIndex), rect.Position(), ::ULIS::FSchedulePolicy::AsyncCacheEfficient);
    }
    ctx.Finish();

    mOnEditableBlockChanged.Broadcast(rects);
}