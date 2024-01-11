#include "LayerStack/Cells/OdysseyAnimationCellImport.h"
#include "LayerStack/Cells/OdysseyAnimationCell.h"
#include "OdysseyFile.h"

bool
FOdysseyAnimationCellImport::Read( FOdysseyAnimationCell* iAnimationCell
                                            , FArchive &Ar )
{
    uint64 start = Ar.Tell();

    uint32 chunkID;
    uint64 chunkLen;
    uint64 chunkEnd;

    // Reads the first chunk (FOdysseyFile::Animation::CHUNK_CELL)
    Ar << chunkID;
    Ar << chunkLen;

    chunkEnd = Ar.Tell() + chunkLen;

    switch( chunkID )
    {
        case FOdysseyFile::Animation::CHUNK_CELL :
            //UE_LOG(LogTemp, Warning, TEXT("CHUNK_CELL") );

            FOdysseyAnimationCellImport::Read( iAnimationCell, Ar, chunkEnd );
        break;

        default:
            //No chunk found, seek back to the beginning and return false
            Ar.Seek( start );
            return false;
    }
    return true;
}

void
FOdysseyAnimationCellImport::Read( FOdysseyAnimationCell* iAnimationCell
                                            , FArchive &Ar
                                            , uint64 iChunkEnd )
{
    FOdysseyFile::ReadChunks( iChunkEnd
                              , Ar
                              , [iAnimationCell](uint32 iChunkID, uint64 iChunkLen, FArchive &Ar) -> void
        {
            switch ( iChunkID )
            {
                case FOdysseyFile::Animation::CHUNK_CELL_LENGTH :
                {
                    Ar << iAnimationCell->mLength;
                }
                break;

                default:
				// Mandatory
                    Ar.Seek( Ar.Tell() + iChunkLen );
                break;
            }
        } );
}
