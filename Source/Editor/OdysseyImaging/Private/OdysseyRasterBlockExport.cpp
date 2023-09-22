#include "OdysseyRasterBlockExport.h"
#include "OdysseyRasterBlock.h"
#include "OdysseyFile.h"

void
FOdysseyRasterBlockExport::WriteBulkData( FOdysseyRasterBlock* iRasterBlock
                                                       , FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::RasterBlock::CHUNK_RASTERBLOCK_BULKDATA
                            , Ar
                            , [iRasterBlock](FArchive &Ar) -> void
    {
        TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> block = iRasterBlock->GetBlock();
        checkf(!!block, TEXT("No block to save"));

        FSharedBuffer sharedBuffer = FSharedBuffer::MakeView(block->Bits(), block->BytesTotal());

        //Updates the payload
        //The payload stored in mBulkData will then be removed from memory once mBulkData.Serialize() is called
        iRasterBlock->mBulkData.UpdatePayload(sharedBuffer, iRasterBlock->mOwner);
    
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
        iRasterBlock->mBulkData.SetCompressionOptions(ECompressedBufferCompressor::Selkie, FOodleDataCompression::ECompressionLevel::Normal);
        
        //Eric: I don't understand what bAllowRegister is
        //but it seems UTexture sets it to false on saving and on true on loading
        //so I'll do the same, but correct me if I'm wrong
        iRasterBlock->mBulkData.Serialize(Ar, iRasterBlock->mOwner, false /* bAllowRegister */);
    });
}

void
FOdysseyRasterBlockExport::WriteId( FOdysseyRasterBlock* iRasterBlock
                                                       , FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::RasterBlock::CHUNK_RASTERBLOCK_ID
                            , Ar
                            , [iRasterBlock](FArchive &Ar) -> void
    {
        Ar << iRasterBlock->Id; //unique ID identifying the block
    });
}

void
FOdysseyRasterBlockExport::WriteResolution( FOdysseyRasterBlock* iRasterBlock
                                                       , FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::RasterBlock::CHUNK_RASTERBLOCK_RESOLUTION
                            , Ar
                            , [iRasterBlock](FArchive &Ar) -> void
    {
        Ar << iRasterBlock->Width;
        Ar << iRasterBlock->Height;
    });
}

void
FOdysseyRasterBlockExport::WriteFormat( FOdysseyRasterBlock* iRasterBlock
                                                       , FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::RasterBlock::CHUNK_RASTERBLOCK_FORMAT
                            , Ar
                            , [iRasterBlock](FArchive &Ar) -> void
    {
        Ar << iRasterBlock->Format;
    });
}

void
FOdysseyRasterBlockExport::Write( FOdysseyRasterBlock* iRasterBlock
                                             , FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::RasterBlock::CHUNK_RASTERBLOCK
                            , Ar
                            , [iRasterBlock](FArchive &Ar) -> void
    {
        WriteId( iRasterBlock, Ar );
        WriteResolution( iRasterBlock, Ar );
        WriteFormat( iRasterBlock, Ar );
        WriteBulkData( iRasterBlock, Ar );
    } );
}
