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

FOdysseyRasterBlock::~FOdysseyRasterBlock()
{
}

FOdysseyRasterBlock::FOdysseyRasterBlock()
    : mCache(FOdysseyRasterBlock_CACHE_NAME, FOdysseyRasterBlock_CACHE_VERSION)
    , mOwner(nullptr)
    , Id(FGuid::NewGuid())
    , mAvailableCounter(0)
    , mIsCacheInvalid(false)
{
}

FOdysseyRasterBlock::FOdysseyRasterBlock(UObject* iOwner)
    : mCache(FOdysseyRasterBlock_CACHE_NAME, FOdysseyRasterBlock_CACHE_VERSION)
    , mOwner(iOwner)
    , Id(FGuid::NewGuid())
    , mAvailableCounter(0)
    , mIsCacheInvalid(false)
{
}

UObject*
FOdysseyRasterBlock::GetOwner() const
{
    return mOwner;
}

void
FOdysseyRasterBlock::PostDuplicate()
{
    Id = FGuid::NewGuid();
}

const FGuid&
FOdysseyRasterBlock::GetId() const
{
    return Id;
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
    FOdysseyRasterBlock* rasterBlock = static_cast<FOdysseyRasterBlock*>(iInfo);

    if ( rasterBlock->mIsCacheInvalid )
    {
        rasterBlock->mCache.Save(rasterBlock->GetId().ToString(), rasterBlock->mSharedBuffer);
        rasterBlock->mIsCacheInvalid = false;
    }
    
    //we have the responsability to delete the block data
    if (rasterBlock->mSharedBuffer.IsOwned())
    {
        //Data is owned by the sharedBuffer
    }
    else
    {
        //Data is owned by the block
        ::ULIS::OnCleanup_FreeMemory(iData, iInfo); 
    }

    rasterBlock->mAvailableCounter.Set(0);
}

void
FOdysseyRasterBlock::SetBlock(TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> iBlock)
{
    FScopeLock Lock(&mMutex);

    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> currentBlock;
    do
    {
        currentBlock = mBlock.Pin();
        if ( currentBlock )
            break;
    }
    while ( mAvailableCounter.GetValue() == 1 );
    
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
    }

    if (iBlock)
    {
        mBlock = iBlock;
        Width = iBlock->Width();
        Height = iBlock->Height();
        Format = iBlock->Format();

        mSharedBuffer = FSharedBuffer::MakeView(iBlock->Bits(), iBlock->BytesTotal());
        iBlock->OnCleanup(::ULIS::FOnCleanupData(&FOdysseyRasterBlock::CleanupBlock, this));

        mIsCacheInvalid = true;
        mAvailableCounter.Set(1);
    }
    
    mOnBlockPtrChanged.Broadcast();
}

TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe>
FOdysseyRasterBlock::GetBlock()
{
	FScopeLock Lock(&mMutex);

    if ( Width <= 0 || Height <= 0 )
        return nullptr;
    
    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> block;
    do
    {
        block = mBlock.Pin();
        if ( block )
            return block;
    }
    while ( mAvailableCounter.GetValue() == 1 );

    mAvailableCounter.Set(1);

    FUniqueBuffer buffer;
    if ( !mCache.Load(Id.ToString(), buffer) )
    {
        mIsCacheInvalid = true;
        if (!LoadBlockFromBulkData(buffer))
            return nullptr;
    }

    block = MakeShared<::ULIS::FBlock>((uint8*)buffer.GetData(), Width, Height, (::ULIS::eFormat)Format);
    mSharedBuffer = buffer.MoveToShared();
    block->OnCleanup(::ULIS::FOnCleanupData(&FOdysseyRasterBlock::CleanupBlock, this));
    mBlock = block;

    return block; //return the currently loaded block, the receiver can release it whenever he wants
}

FOdysseyRasterBlock::FOnBlockChanged&
FOdysseyRasterBlock::OnBlockChanged()
{
    return mOnBlockChanged;
}

FOdysseyRasterBlock::FOnBlockCommited&
FOdysseyRasterBlock::OnBlockCommited()
{
    return mOnBlockCommited;
}

FOdysseyRasterBlock::FPostProcess&
FOdysseyRasterBlock::PostProcess()
{
    return mPostProcess;
}

FSimpleMulticastDelegate&
FOdysseyRasterBlock::OnBlockPtrChanged()
{
    return mOnBlockPtrChanged;
}

bool
FOdysseyRasterBlock::LoadBlockFromBulkData(FUniqueBuffer& oBuffer)
{
    FSharedBuffer buffer = mBulkData.GetPayload().Get();
    oBuffer = FUniqueBuffer::Alloc(buffer.GetSize());
    oBuffer.GetView().CopyFrom(buffer);

    return true;
}

FArchive&
operator<<(FArchive& Ar, FOdysseyRasterBlock& iRasterBlock)
{
    iRasterBlock.Serialize(Ar);
    return Ar;
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

        //FSharedBuffer sharedBuffer = FSharedBuffer::MakeView(block->Bits(), block->BytesTotal());

        //Updates the payload
        //The payload stored in mBulkData will then be removed from memory once mBulkData.Serialize() is called
        mBulkData.UpdatePayload(mSharedBuffer, mOwner);
    
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
        //mInvalidTileMap = FULISInvalidTileMap(64, Width, Height);
        mCache.Remove(Id.ToString());
    }
}
