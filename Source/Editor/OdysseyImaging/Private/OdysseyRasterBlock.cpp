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

#include <chrono>

#define FOdysseyRasterBlock_CACHE_NAME TEXT("OdysseyRasterBlock")
#define FOdysseyRasterBlock_CACHE_VERSION TEXT("A6ED84107BAD11EDA1EB0242AC120002")

class FOdysseyRasterBlockPreloadHandle : public IOdysseyHandle
{
public:
    FOdysseyRasterBlockPreloadHandle(UOdysseyRasterBlock* iBlock)
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

class FOdysseyRasterBlockChange : public FCommandChange
{
public:
    ~FOdysseyRasterBlockChange()
    {
        RemoveValueFromCache(mUndoId.ToString());
        RemoveValueFromCache(mRedoId.ToString());
    }

    FOdysseyRasterBlockChange(const FGuid& iUndoId, const FGuid& iRedoId) :
        mUndoId(iUndoId),
        mRedoId(iRedoId)
    {
    }

	/** Makes the change to the object */
    //REDO
	virtual void Apply( UObject* Object ) override
    {
        UOdysseyRasterBlock* rasterBlock = Cast<UOdysseyRasterBlock>(Object);
        rasterBlock->ResetEditableBlock();
        rasterBlock->LoadUndoFromCache(mRedoId.ToString());
    }

	/** Reverts change to the object */
    //UNDO
	virtual void Revert( UObject* Object ) override
    {
        UOdysseyRasterBlock* rasterBlock = Cast<UOdysseyRasterBlock>(Object);
        rasterBlock->ResetEditableBlock();
        rasterBlock->LoadUndoFromCache(mUndoId.ToString());
    }

	/** Describes this change (for debugging) */
	virtual FString ToString() const override
    {
        return TEXT("Odyssey Raster Block Undo/Redo");
    }

public:
    FGuid mUndoId;
    FGuid mRedoId;
};


UOdysseyRasterBlock::~UOdysseyRasterBlock()
{
}

UOdysseyRasterBlock::UOdysseyRasterBlock()
{
    Id = FGuid::NewGuid();
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

void
UOdysseyRasterBlock::CleanupBlock(uint8* iData, void* iInfo)
{
    UOdysseyRasterBlock* rasterBlock = static_cast<UOdysseyRasterBlock*>(iInfo);
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
UOdysseyRasterBlock::SetBlock(TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> iBlock)
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
        iBlock->OnCleanup(::ULIS::FOnCleanupData(&UOdysseyRasterBlock::CleanupBlock, this));

        mInvalidTileMap = FULISInvalidTileMap(64, Width, Height);
    }

    //If an editableBlock was set, don't consider it as the editableBlock anymore
    //Let the user reload the block
    mEditableBlock = nullptr;
    
     mOnBlockPtrChanged.Broadcast();
}

bool
UOdysseyRasterBlock::IsBeingEdited()
{
    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> editableBlock = mEditableBlock.Pin();
    return !!editableBlock; //returns true if editableblock is valid (don't use IsValid() as it can be wrong sometimes)
}

TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe>
UOdysseyRasterBlock::GetEditableBlock()
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
UOdysseyRasterBlock::GetBlock()
{
    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> block = mBlock.Pin();
    if ( block )
        return block;

    if ( Width <= 0 || Height <= 0 )
        return nullptr;

    //Load Block from DDC
    block = MakeShared<::ULIS::FBlock>(Width, Height, (::ULIS::eFormat)Format);
    block->OnCleanup(::ULIS::FOnCleanupData(&UOdysseyRasterBlock::CleanupBlock, this));
    mBlock = block; //watch the loaded block

    if ( LoadBlockFromCache(block.ToSharedRef(), Id.ToString()) )
        return block;

    if ( LoadBlockFromBulkData(block.ToSharedRef()) )
        return block;

    return block; //return the currently loaded block, the reveiver can release it whenever he wants
}

void
UOdysseyRasterBlock::Invalidate(const TArray<::ULIS::FRectI>& iRects, bool iIsInteractive)
{
    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> editableBlock = mEditableBlock.Pin();
    if ( !editableBlock )
        return;

    //Interactive update
    {
        FULISInvalidTileMap tempInvalidTileMap(64, Width, Height);
        tempInvalidTileMap.Invalidate(iRects);
        const TArray<::ULIS::FRectI>& invalidRects = tempInvalidTileMap.InvalidRects();
        if (invalidRects.Num() > 0)
            OnBlockChanged().Broadcast(invalidRects, true); //always send at least one interactive event
    }

    //Non-Interactive update
    mInvalidTileMap.Invalidate(iRects);
    if (!iIsInteractive && mInvalidTileMap.InvalidTiles().Num() > 0 )
    {
        //Create a copy of the internal block
        TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> block = GetBlock();
        if ( !block )
            return;

        //Store Undo if needed
        FGuid undoId = FGuid::NewGuid();
        FGuid redoId = FGuid::NewGuid();
        bool storeUndo = GUndo && GUndo->ContainsObject(this);
        if (storeUndo)
            SaveUndoToCache(undoId.ToString());

        //Copy editableBlock to block
        ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext((::ULIS::eFormat)Format);
        const TArray<::ULIS::FRectI>& invalidRects = mInvalidTileMap.InvalidRects();
        for (const ::ULIS::FRectI& rect : invalidRects)
        {
            ctx.Copy(*editableBlock, *block, rect, rect.Position(), ::ULIS::FSchedulePolicy::AsyncCacheEfficient);
        }
        ctx.Finish();
        
        //Store Undo if needed
        if (storeUndo)
        {
            SaveUndoToCache(redoId.ToString());
            GUndo->StoreUndo(this, MakeUnique<FOdysseyRasterBlockChange>(undoId, redoId));
        }

        OnBlockChanged().Broadcast(mInvalidTileMap.InvalidRects(), false);
        mInvalidTileMap.Clear();
    }
}

TSharedPtr<IOdysseyHandle>
UOdysseyRasterBlock::Preload()
{
    TSharedPtr<IOdysseyHandle> handle = mPreloadHandle.Pin();
    if (handle)
        return handle;

    handle = MakeShared<FOdysseyRasterBlockPreloadHandle>(this);
    mPreloadHandle = handle;
    return handle;
}

UOdysseyRasterBlock::FOnBlockChanged&
UOdysseyRasterBlock::OnBlockChanged()
{
    return mOnBlockChanged;
}

UOdysseyRasterBlock::FOnEditableBlockChanged&
UOdysseyRasterBlock::OnEditableBlockChanged()
{
    return mOnEditableBlockChanged;
}

FSimpleMulticastDelegate&
UOdysseyRasterBlock::OnBlockPtrChanged()
{
    return mOnBlockPtrChanged;
}

void
UOdysseyRasterBlock::SaveBlockToCache(const ::ULIS::FBlock& iBlock, const FString& iId)
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
                GetPathName(),
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
UOdysseyRasterBlock::LoadBlockFromCache(TSharedRef<::ULIS::FBlock, ESPMode::ThreadSafe> oBlock, const FString& iId)
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
                GetPathName(),
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
UOdysseyRasterBlock::LoadBlockFromBulkData(TSharedRef<::ULIS::FBlock, ESPMode::ThreadSafe> oBlock)
{
    FSharedBuffer buffer = mBulkData.GetPayload().Get();
    FUniqueBuffer uniqueBuffer = FUniqueBuffer::MakeView(oBlock->Bits(), oBlock->BytesTotal());
    uniqueBuffer.GetView().CopyFrom(buffer);

    return true;
}

void
UOdysseyRasterBlock::SaveUndoToCache(const FString& iId)
{
    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> block = GetBlock();
    if (!block)
        return;

    const TArray<::ULIS::FRectI>& rectsToSave = mInvalidTileMap.InvalidRects();

    TArray<uint8> TempData;
    FMemoryWriter writer(TempData); //allows us to save rectangles alongside the block

    int numTiles = rectsToSave.Num();
    writer << numTiles;
    for (int i = 0; i < numTiles; i++)
    {
        ::ULIS::FRectI rect = rectsToSave[i];
        writer << rect.x;
        writer << rect.y;
        writer << rect.w;
        writer << rect.h;
    }
    
    //copy the rectangle in the block
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext((::ULIS::eFormat)Format);
    int tileSize = mInvalidTileMap.TileSize();
    int dataStart = TempData.Num();//Do this before AddUninitialized() to keep track of where to write the block in memory
    TempData.AddUninitialized(tileSize * tileSize * numTiles * block->BytesPerPixel());
    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> blockToSave = MakeShared<::ULIS::FBlock>(TempData.GetData() + dataStart, tileSize, tileSize * numTiles, (::ULIS::eFormat)Format);
    for (int i = 0; i < rectsToSave.Num(); i++)
    {
        const ::ULIS::FRectI& rect = rectsToSave[i];
        ctx.Copy(*block, *blockToSave, rect, ::ULIS::FVec2I(0, tileSize * i), ::ULIS::FSchedulePolicy::AsyncCacheEfficient);
    }
    ctx.Finish();

    FString CacheKey = FDerivedDataCacheInterface::BuildCacheKey(
        FOdysseyRasterBlock_CACHE_NAME,
        FOdysseyRasterBlock_CACHE_VERSION, //a GUID identifying the version of the key
        iId
    );

    FSharedBuffer sharedBuffer = FSharedBuffer::MakeView(TempData.GetData(), TempData.Num());
    UE::DerivedData::FValue derivedDataValue = UE::DerivedData::FValue::Compress(sharedBuffer);

    //Store the tile in cache
    UE::DerivedData::FRequestOwner putOwner(UE::DerivedData::EPriority::Lowest);
    UE::DerivedData::GetCache().PutValue(
        {
            {
                GetPathName(),
                UE::DerivedData::ConvertLegacyCacheKey(CacheKey),
                MoveTemp(derivedDataValue),
                UE::DerivedData::ECachePolicy::StoreLocal
            }
        },
        putOwner
    );
    putOwner.KeepAlive();
}

void
UOdysseyRasterBlock::ResetEditableBlock()
{
    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> editableBlock = mEditableBlock.Pin();
    if ( !editableBlock )
        return;

    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> block = GetBlock();
    if ( !block )
        return;

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext((::ULIS::eFormat)Format);
    const TArray<::ULIS::FRectI>& invalidRects = mInvalidTileMap.InvalidRects();
    for (const ::ULIS::FRectI& rect : invalidRects)
    {
        ctx.Copy(*block, *editableBlock, rect, rect.Position(), ::ULIS::FSchedulePolicy::AsyncCacheEfficient);
    }
    ctx.Finish();

    mOnEditableBlockChanged.Broadcast(invalidRects);
    mInvalidTileMap.Clear();
}

bool
UOdysseyRasterBlock::LoadUndoFromCache(const FString& iId)
{
    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> block = GetBlock();
    if ( !block )
        return false;

    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> editableBlock = mEditableBlock.Pin();

    bool success = false;
    TArray<::ULIS::FRectI> rects;

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
                GetPathName(),
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

            int tileSize = mInvalidTileMap.TileSize();
            TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> blockToLoad = MakeShared<::ULIS::FBlock>(static_cast<uint8*>(dataPtr) + reader.Tell(), tileSize, tileSize * numTiles, (::ULIS::eFormat)Format);
            ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext((::ULIS::eFormat)Format);
            for (int i = 0; i < numTiles; i++)
            {   
                const ::ULIS::FRectI& rect = rects[i];
                ctx.Copy(*blockToLoad, *block, ::ULIS::FRectI::FromXYWH(0, i * tileSize, tileSize, tileSize), ::ULIS::FVec2I(rect.x, rect.y), ::ULIS::FSchedulePolicy::AsyncCacheEfficient);
            }

            if ( editableBlock )
            {
                for ( int i = 0; i < numTiles; i++ )
                {
                    const ::ULIS::FRectI& rect = rects[i];
                    ctx.Copy(*blockToLoad, *editableBlock, ::ULIS::FRectI::FromXYWH(0, i * tileSize, tileSize, tileSize), ::ULIS::FVec2I(rect.x, rect.y), ::ULIS::FSchedulePolicy::AsyncCacheEfficient);
                }
            }

            ctx.Finish();
            success = true;
        }
    );
    getOwner.Wait();

    mOnBlockChanged.Broadcast(rects, false);
    if (editableBlock)
        mOnEditableBlockChanged.Broadcast(rects);

    return success;
}

void
UOdysseyRasterBlock::Serialize(FArchive& Ar)
{
    //Load/Save all UPROPERTIES
	Super::Serialize(Ar);

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
        mBulkData.UpdatePayload(sharedBuffer, this);
    
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
        mBulkData.Serialize(Ar, this, false /* bAllowRegister */);
    }
    else if ( Ar.IsLoading() )
    {       
        mBulkData.Serialize(Ar, this);
        mInvalidTileMap = FULISInvalidTileMap(64, Width, Height);
        RemoveValueFromCache(Id.ToString());
    }
}