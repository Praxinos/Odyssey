// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyRasterBlock.h"

#include "Serialization/EditorBulkData.h"

#include "OdysseyDiskCache.h"
#include "OdysseyFile.h"

#define FOdysseyRasterBlock_CACHE_NAME TEXT("OdysseyRasterBlock")
#define FOdysseyRasterBlock_CACHE_VERSION TEXT("A6ED84107BAD11EDA1EB0242AC120002")

namespace
{
/** The fields of a saved raster block. */
struct FLegacyRasterBlock
{
    FGuid Id;
    int32 Width = -1;
    int32 Height = -1;
    ::ULIS::eFormat Format = ::ULIS::Format_RGBA8;
    UE::Serialization::FEditorBulkData BulkData;
};

/**
 * Reads a raster block saved in chunks.
 * @return False when the archive holds no chunk, e.g. a raster block saved before chunks: the archive is then back at its position.
 */
bool ReadChunks(FArchive& Ar, UObject& Owner, FLegacyRasterBlock& Block)
{
    if (Ar.AtEnd() || Ar.IsCriticalError())
    {
        return true;
    }

    const uint64 Start = Ar.Tell();
    uint32 ChunkId;
    uint64 ChunkLength;
    Ar << ChunkId;
    Ar << ChunkLength;

    // Reading beyond the bounds of the linker, which differ from those of the archive, sets a critical error:
    // seek back and clear it, as if nothing was read.
    if (Ar.IsCriticalError())
    {
        Ar.Seek(Start);
        Ar.ClearCriticalError();
        return true;
    }

    if (ChunkId != FOdysseyFile::RasterBlock::CHUNK_RASTERBLOCK)
    {
        Ar.Seek(Start);
        return false;
    }

    FOdysseyFile::ReadChunks(Ar.Tell() + ChunkLength, Ar, [&Owner, &Block](uint32 InChunkId, uint64 InChunkLength, FArchive& InAr) -> void
    {
        switch (InChunkId)
        {
            case FOdysseyFile::RasterBlock::CHUNK_RASTERBLOCK_BULKDATA:
                Block.BulkData.Serialize(InAr, &Owner);
                break;

            case FOdysseyFile::RasterBlock::CHUNK_RASTERBLOCK_ID:
                InAr << Block.Id;
                break;

            case FOdysseyFile::RasterBlock::CHUNK_RASTERBLOCK_RESOLUTION:
                InAr << Block.Width;
                InAr << Block.Height;
                break;

            case FOdysseyFile::RasterBlock::CHUNK_RASTERBLOCK_FORMAT:
            {
                int32 Format = Block.Format;
                InAr << Format;
                Block.Format = static_cast<::ULIS::eFormat>(Format);
                break;
            }

            default:
                // Unknown chunks are skipped.
                InAr.Seek(InAr.Tell() + InChunkLength);
                break;
        }
    });
    return true;
}
}

TSharedPtr<::ULIS::FBlock>
Odyssey::ReadLegacyRasterBlock(FArchive& Ar, UObject& Owner)
{
    if (!Ar.IsLoading() || Ar.IsTransacting() || !Ar.IsPersistent())
    {
        return nullptr;
    }

    FLegacyRasterBlock Block;
    if (!ReadChunks(Ar, Owner, Block))
    {
        // Raster blocks saved before chunks.
        Ar << Block.Id;
        Ar << Block.Width;
        Ar << Block.Height;
        int32 Format;
        Ar << Format;
        Block.Format = static_cast<::ULIS::eFormat>(Format);
        Block.BulkData.Serialize(Ar, &Owner);
    }

    // The former raster blocks kept a copy of their pixels in a disk cache, which is no longer read.
    FOdysseyDiskCache Cache(FOdysseyRasterBlock_CACHE_NAME, FOdysseyRasterBlock_CACHE_VERSION);
    Cache.Remove(Block.Id.ToString());

    if (Block.Width <= 0 || Block.Height <= 0 || !Block.BulkData.HasPayloadData())
    {
        return nullptr;
    }

    // The payload is loaded from the package file, also while the owner is being loaded.
    const FSharedBuffer Payload = Block.BulkData.GetPayload().Get();
    TSharedPtr<::ULIS::FBlock> Pixels = MakeShared<::ULIS::FBlock>(Block.Width, Block.Height, Block.Format);
    if (Payload.GetSize() != Pixels->BytesTotal())
    {
        return nullptr;
    }
    FMemory::Memcpy(Pixels->Bits(), Payload.GetData(), Payload.GetSize());
    return Pixels;
}
