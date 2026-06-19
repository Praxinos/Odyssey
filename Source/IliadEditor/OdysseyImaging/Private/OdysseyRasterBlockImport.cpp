// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyRasterBlockImport.h"
#include "OdysseyRasterBlock.h"
#include "OdysseyFile.h"

bool
FOdysseyRasterBlockImport::Read( FOdysseyRasterBlock* iRasterBlock
                                            , FArchive &Ar )
{
    if (Ar.IsCriticalError())
        return true;

    uint64 start = Ar.Tell();

    uint32 chunkID;
    uint64 chunkLen;
    uint64 chunkEnd;

    // Reads the first chunk (FOdysseyFile::RasterBlock::CHUNK_RASTERBLOCK)
    Ar << chunkID;
    Ar << chunkLen;

    //PATCH:
    // If we read out of the Linker bounds (which is not the same as the Archive bounds)
    // The archive generates a CriticalError
    // We don't have access to the linker bounds, so we can only rely on the critical error state
    // We then need to seek to start again, like nothing ever happened
    // And clear the Critical Error state
    if (Ar.IsCriticalError())
    {
        Ar.Seek( start );
        Ar.ClearCriticalError();
        return true;
    }

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
