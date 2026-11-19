// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyRasterBlock.h"

#include "OdysseyDiskCache.h"
#include "OdysseyRasterBlockImport.h"

#define FOdysseyRasterBlock_CACHE_NAME TEXT("OdysseyRasterBlock")
#define FOdysseyRasterBlock_CACHE_VERSION TEXT("A6ED84107BAD11EDA1EB0242AC120002")

FOdysseyRasterBlock::~FOdysseyRasterBlock()
{
}

FOdysseyRasterBlock::FOdysseyRasterBlock(UObject* iOwner)
    : mOwner(iOwner)
{
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

::ULIS::eFormat
FOdysseyRasterBlock::GetFormat() const
{
    return mFormat;
}

TSharedPtr<::ULIS::FBlock>
FOdysseyRasterBlock::GetBlock()
{
    if (mBlock || mWidth <= 0 || mHeight <= 0 || !mBulkData.HasPayloadData())
        return mBlock;

    FSharedBuffer payload = mBulkData.GetPayload().Get();
    TSharedPtr<::ULIS::FBlock> block = MakeShared<::ULIS::FBlock>(mWidth, mHeight, mFormat);
    if (payload.GetSize() != block->BytesTotal())
        return nullptr;

    FMemory::Memcpy(block->Bits(), payload.GetData(), payload.GetSize());
    mBlock = block;
    return mBlock;
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
    if ( !Ar.IsLoading() || Ar.IsTransacting() || !Ar.IsPersistent() )
        return;

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

    // The former raster blocks kept a copy of their pixels in a disk cache, which is no longer read.
    FOdysseyDiskCache cache(FOdysseyRasterBlock_CACHE_NAME, FOdysseyRasterBlock_CACHE_VERSION);
    cache.Remove(mId.ToString());
}
