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
    : mOwner(nullptr)
    , mId(FGuid::NewGuid())
    , mConstructionDestructionMutex(MakeShared<FCriticalSection>())
    , mBlockData(nullptr)
{
}

FOdysseyRasterBlock::FOdysseyRasterBlock(UObject* iOwner)
    : mOwner(iOwner)
    , mConstructionDestructionMutex(MakeShared<FCriticalSection>())
    , mBlockData(nullptr)
{
}

FOdysseyRasterBlock::FOdysseyRasterBlock(UObject* iOwner, int iWidth, int iHeight, ::ULIS::eFormat iFormat)
    : mOwner(iOwner)
    , mWidth(iWidth)
    , mHeight(iHeight)
    , mFormat(iFormat)
    , mId(FGuid::NewGuid())
    , mConstructionDestructionMutex(MakeShared<FCriticalSection>())
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
    //Load the block in memory
    //Then change the Id
    //Once the SharedPtr is released the block will be automatically saved in cache with the new Id
    TSharedPtr<::ULIS::FBlock> originalBlock = GetBlock();
    mId = FGuid::NewGuid();
}

void
FOdysseyRasterBlock::ConvertTo(int iWidth, int iHeight, ::ULIS::eFormat iFormat)
{
    if ( iWidth == mWidth && iHeight == mHeight && iFormat == mFormat )
        return; //will save the block using the new Id

    //Duplicate block
    TSharedPtr<::ULIS::FBlock> originalBlock = GetBlock();
    TSharedPtr<::ULIS::FBlock> block = MakeShared<::ULIS::FBlock>(iWidth, iHeight, iFormat);
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(iFormat);
    ctx.ConvertFormat(
        *originalBlock.Get(),
        *block.Get(),
        ::ULIS::FRectI::Auto,
        ::ULIS::FVec2I(0),
        ::ULIS::FSchedulePolicy::AsyncCacheEfficient
    );
    ctx.Finish();

    FScopeLock Lock(&mMutex);
    
    //we have the responsability to delete the block data
    if ( mBlockData->mBuffer.IsOwned() )
    {
        //Data is owned by the sharedBuffer
        originalBlock->OnCleanup(::ULIS::FOnCleanupData());
    }
    else
    {
        //Data is not owned by the sharedBuffer, destroy it ourself
        originalBlock->OnCleanup(::ULIS::FOnCleanupData(&::ULIS::OnCleanup_FreeMemory));
    }

    mBlock = block;
    mWidth = iWidth;
    mHeight = iHeight;
    mFormat = iFormat;

    delete mBlockData;
    mBlockData = new FBlockData();
    mBlockData->mConstructionDestructionMutex = mConstructionDestructionMutex;
    mBlockData->mBuffer = FUniqueBuffer::MakeView(block->Bits(), block->BytesTotal());
    mBlockData->mIsCacheInvalid = true;
    mBlockData->mId = mId;
        
    block->OnCleanup(::ULIS::FOnCleanupData(&FOdysseyRasterBlock::CleanupBlock, mBlockData));
}

const FGuid&
FOdysseyRasterBlock::GetId() const
{
    return mId;
}

int
FOdysseyRasterBlock::GetWidth() const
{
    return mWidth;
}

int
FOdysseyRasterBlock::GetHeight() const
{
    return mHeight;
}

::ULIS::FRectI
FOdysseyRasterBlock::GetRect() const
{
    return ::ULIS::FRectI::FromXYWH(0, 0, mWidth, mHeight);
}

::ULIS::eFormat
FOdysseyRasterBlock::GetFormat() const
{
    return mFormat;
}

void
FOdysseyRasterBlock::CleanupBlock(uint8* iData, void* iInfo)
{
    TRACE_CPUPROFILER_EVENT_SCOPE(FOdysseyRasterBlock::CleanupBlock);
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

    blockData->mConstructionDestructionMutex->Unlock();

    delete blockData;
}

TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe>
FOdysseyRasterBlock::GetBlock()
{
    FScopeLock Lock(&mMutex);

    if ( mWidth <= 0 || mHeight <= 0 )
        return nullptr;
    
    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> block;
    block = mBlock.Pin();
    if ( block )
        return block;

    mConstructionDestructionMutex->Lock();

    mBlockData = new FBlockData();
    mBlockData->mConstructionDestructionMutex = mConstructionDestructionMutex;
    mBlockData->mId = mId;
    mBlockData->mIsCacheInvalid = false;

    //FUniqueBuffer buffer;
    FOdysseyDiskCache cache(FOdysseyRasterBlock_CACHE_NAME, FOdysseyRasterBlock_CACHE_VERSION);
    if ( !cache.Load(mId.ToString(), mBlockData->mBuffer) )
    {
        mBlockData->mIsCacheInvalid = true;
        if (!LoadBlockFromBulkData(mBlockData->mBuffer))
        {
            block = MakeShared<::ULIS::FBlock>(mWidth, mHeight, mFormat);
            ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(mFormat);
            ctx.Clear(*block);
            ctx.Finish();

            mBlockData->mBuffer = FUniqueBuffer::MakeView(block->Bits(), block->BytesTotal());

            block->OnCleanup(::ULIS::FOnCleanupData(&FOdysseyRasterBlock::CleanupBlock, mBlockData));
            mBlock = block;

            return block;
        }
    }

    block = MakeShared<::ULIS::FBlock>((uint8*)mBlockData->mBuffer.GetData(), mWidth, mHeight, mFormat);
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

bool
FOdysseyRasterBlock::LoadBlockFromBulkData(FUniqueBuffer& oBuffer)
{
    if (!mBulkData.HasPayloadData())
        return false;
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

            Ar << mId; //unique ID identifying the block
            Ar << mWidth;
            Ar << mHeight;

            int format;
            Ar << format;
            mFormat = (::ULIS::eFormat)format;

            mBulkData.Serialize(Ar, mOwner);
        }

        FOdysseyDiskCache cache(FOdysseyRasterBlock_CACHE_NAME, FOdysseyRasterBlock_CACHE_VERSION);
        cache.Remove(mId.ToString());
    }
}
