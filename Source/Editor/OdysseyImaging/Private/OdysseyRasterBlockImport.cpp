#include "OdysseyRasterBlockImport.h"
#include "OdysseyRasterBlock.h"
#include "OdysseyFile.h"

bool
FOdysseyRasterBlockImport::Read( FOdysseyRasterBlock* iRasterBlock
                                            , FArchive &Ar )
{
    uint64 start = Ar.Tell();

    uint32 chunkID;
    uint64 chunkLen;
    uint64 chunkEnd;

    // Reads the first chunk (FOdysseyFile::RasterBlock::CHUNK_RASTERBLOCK)
    Ar << chunkID;
    Ar << chunkLen;

    chunkEnd = Ar.Tell() + chunkLen;

    switch( chunkID )
    {
        case FOdysseyFile::RasterBlock::CHUNK_RASTERBLOCK :
            //UE_LOG(LogTemp, Warning, TEXT("CHUNK_RASTERBLOCK") );

            FOdysseyRasterBlockImport::Read( iRasterBlock, Ar, chunkEnd );
        break;

        default:
            //No chunk found, seek back to the beginning and return false
            Ar.Seek( start );
            return false;
    }
    return true;
}

void
FOdysseyRasterBlockImport::Read( FOdysseyRasterBlock* iRasterBlock
                                            , FArchive &Ar
                                            , uint64 iChunkEnd )
{
    FOdysseyFile::ReadChunks( iChunkEnd
                              , Ar
                              , [iRasterBlock](uint32 iChunkID, uint64 iChunkLen, FArchive &Ar) -> void
        {
            switch ( iChunkID )
            {
                case FOdysseyFile::RasterBlock::CHUNK_RASTERBLOCK_BULKDATA :
                {
                    iRasterBlock->mBulkData.Serialize(Ar, iRasterBlock->mOwner);
                }
                break;

                case FOdysseyFile::RasterBlock::CHUNK_RASTERBLOCK_ID :
                {
                    Ar << iRasterBlock->mId; //unique ID identifying the block
                }
                break;
                
                case FOdysseyFile::RasterBlock::CHUNK_RASTERBLOCK_RESOLUTION :
                {
                    Ar << iRasterBlock->mWidth;
                    Ar << iRasterBlock->mHeight;
                }
                break;

                case FOdysseyFile::RasterBlock::CHUNK_RASTERBLOCK_FORMAT :
                {
                    int format = iRasterBlock->mFormat;
                    Ar << format;
                    iRasterBlock->mFormat = (::ULIS::eFormat)format;
                }
                break;

                default:
                // Mandatory
                    Ar.Seek( Ar.Tell() + iChunkLen );
                break;
            }
        } );
}
