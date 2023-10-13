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
#include "OdysseyRasterBlockExport.h"
#include "OdysseyRasterBlockImport.h"

#define FOdysseyRasterBlock_CACHE_NAME TEXT("OdysseyRasterBlock")
#define FOdysseyRasterBlock_CACHE_VERSION TEXT("A6ED84107BAD11EDA1EB0242AC120002")

FOdysseyRasterBlock::~FOdysseyRasterBlock()
{
}

FOdysseyRasterBlock::FOdysseyRasterBlock()
    //: mCache(FOdysseyRasterBlock_CACHE_NAME, FOdysseyRasterBlock_CACHE_VERSION)
    : mOwner(nullptr)
    , Id(FGuid::NewGuid())
    , mBlockData(nullptr)
{
}

FOdysseyRasterBlock::FOdysseyRasterBlock(UObject* iOwner)
    //: mCache(FOdysseyRasterBlock_CACHE_NAME, FOdysseyRasterBlock_CACHE_VERSION)
    : mOwner(iOwner)
    , Id(FGuid::NewGuid())
    , mBlockData(nullptr)
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
    FBlockData* blockData = static_cast<FBlockData*>(iInfo);

    if ( blockData->mIsCacheInvalid )
    {
        FOdysseyDiskCache cache(FOdysseyRasterBlock_CACHE_NAME, FOdysseyRasterBlock_CACHE_VERSION);
        FSharedBuffer sharedBuffer = FSharedBuffer::MakeView(blockData->mBuffer.GetView());
        cache.Save(blockData->mId.ToString(), sharedBuffer);
    }
    
    //we have the responsability to delete the block data
    if (blockData->mBuffer.IsOwned())
    {
        //Data is owned by the sharedBuffer
    }
    else
    {
        //Data is owned by the block
        ::ULIS::OnCleanup_FreeMemory(iData, iInfo); 
    }

    delete blockData;
}

void
FOdysseyRasterBlock::SetBlock(TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> iBlock)
{
    FScopeLock Lock(&mMutex);

    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> currentBlock;
    currentBlock = mBlock.Pin();
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

        mBlockData = new FBlockData();
        mBlockData->mBuffer = FUniqueBuffer::MakeView(iBlock->Bits(), iBlock->BytesTotal());
        mBlockData->mIsCacheInvalid = true;
        mBlockData->mId = Id;
        
        iBlock->OnCleanup(::ULIS::FOnCleanupData(&FOdysseyRasterBlock::CleanupBlock, mBlockData));
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
    block = mBlock.Pin();
    if ( block )
        return block;

    mBlockData = new FBlockData();
    mBlockData->mId = Id;
    mBlockData->mIsCacheInvalid = false;

    //FUniqueBuffer buffer;
    FOdysseyDiskCache cache(FOdysseyRasterBlock_CACHE_NAME, FOdysseyRasterBlock_CACHE_VERSION);
    if ( !cache.Load(Id.ToString(), mBlockData->mBuffer) )
    {
        mBlockData->mIsCacheInvalid = true;
        if (!LoadBlockFromBulkData(mBlockData->mBuffer))
        {
            delete mBlockData;
            mBlockData = nullptr;
            return nullptr;
        }
    }

    block = MakeShared<::ULIS::FBlock>((uint8*)mBlockData->mBuffer.GetData(), Width, Height, (::ULIS::eFormat)Format);
    block->OnCleanup(::ULIS::FOnCleanupData(&FOdysseyRasterBlock::CleanupBlock, mBlockData));
    mBlock = block;

    return block; //return the currently loaded block, the receiver can release it whenever he wants
}

void
FOdysseyRasterBlock::InvalidateCache()
{
    //ensure we have a block while invalidating
    TSharedPtr<::ULIS::FBlock> block = mBlock.Pin();
    if ( !block )
        return;
    
    mBlockData->mIsCacheInvalid = true;
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
    if ( Ar.IsTransacting() || !Ar.IsPersistent() )
        return;

    if( Ar.IsSaving() )
    {
        FOdysseyRasterBlockExport::Write( this, Ar );
    }

    if( Ar.IsLoading() )
    {
        if (!FOdysseyRasterBlockImport::Read( this, Ar ))
        {
            //Old Style No Chunk Loading

            Ar << Id; //unique ID identifying the block
            Ar << Width;
            Ar << Height;
            Ar << Format;

            mBulkData.Serialize(Ar, mOwner);
        }

        FOdysseyDiskCache cache(FOdysseyRasterBlock_CACHE_NAME, FOdysseyRasterBlock_CACHE_VERSION);
        cache.Remove(Id.ToString());
    }
}
