#include "LayerStack/Cells/CellImageRaster/OdysseyAnimationCellImageRasterImport.h"
#include "LayerStack/Cells/CellImageRaster/OdysseyAnimationCellImageRaster.h"
#include "OdysseyFile.h"

bool
FOdysseyAnimationCellImageRasterImport::Read( FOdysseyAnimationCellImageRaster* iAnimationCellImageRaster
                                            , FArchive &Ar )
{
    uint64 start = Ar.Tell();

    uint32 chunkID;
    uint64 chunkLen;
    uint64 chunkEnd;

    // Reads the first chunk (FOdysseyFile::Animation::CHUNK_CELLIMAGERASTER)
    Ar << chunkID;
    Ar << chunkLen;

    chunkEnd = Ar.Tell() + chunkLen;

    switch( chunkID )
    {
        case FOdysseyFile::Animation::CHUNK_CELLIMAGERASTER :
            UE_LOG(LogTemp, Warning, TEXT("CHUNK_CELLIMAGERASTER") );

            FOdysseyAnimationCellImageRasterImport::Read( iAnimationCellImageRaster, Ar, chunkEnd );
        break;

        default:
            //No chunk found, seek back to the beginning and return false
            Ar.Seek( start );
            return false;
    }
    return true;
}

void
FOdysseyAnimationCellImageRasterImport::Read( FOdysseyAnimationCellImageRaster* iAnimationCellImageRaster
                                            , FArchive &Ar
                                            , uint64 iChunkEnd )
{
    FOdysseyFile::ReadChunks( iChunkEnd
                              , Ar
                              , [iAnimationCellImageRaster](uint32 iChunkID, uint64 iChunkLen, FArchive &Ar) -> void
        {
            switch ( iChunkID )
            {
                case FOdysseyFile::Animation::CHUNK_CELLIMAGERASTER_RASTERBLOCK :
                {
                    Ar << *iAnimationCellImageRaster->mRasterBlock;
                }
                break;

                default:
				// Mandatory
                    Ar.Seek( Ar.Tell() + iChunkLen );
                break;
            }
        } );
}
