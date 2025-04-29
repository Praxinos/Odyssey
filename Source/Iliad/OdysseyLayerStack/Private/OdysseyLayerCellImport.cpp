// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyLayerCellImport.h"
#include "OdysseyLayerCell.h"
#include <functional>

static const uint32 CHUNK_CELL =  0xf93591b3; // container
static const uint32 CHUNK_CELL_LENGTH = 0x5dbe40b7; // uint32 (Length)
static const uint32 CHUNK_CELL_MARKID = 0x866d04a6; // FString (MarkId)

void
ReadLayerCellChunks( uint64 iChunkEnd, FArchive &Ar, std::function<void(uint32, uint64, FArchive&)> iCallback )
{
    //UE_LOG( LogTemp, Warning, TEXT("ReadChunks") );

    while( Ar.Tell() != iChunkEnd )
    {
        uint64 chunkLen;
        uint32 chunkID;

        Ar << chunkID;
        Ar << chunkLen;

        //UE_LOG( LogTemp, Warning, TEXT("Reading Chunk %X %d"), chunkID, chunkLen );

        /*if ( chunkLen )
        {*/
            iCallback( chunkID, chunkLen, Ar );
        /*}*/
    }
}

bool
FOdysseyLayerCellImport::Read( UOdysseyLayerCell* iCell
                                            , FArchive &Ar )
{
    uint64 start = Ar.Tell();

    uint32 chunkID;
    uint64 chunkLen;
    uint64 chunkEnd;

    // Reads the first chunk (CHUNK_CELL)
    Ar << chunkID;
    Ar << chunkLen;

    chunkEnd = Ar.Tell() + chunkLen;

    switch( chunkID )
    {
        case CHUNK_CELL :
            //UE_LOG(LogTemp, Warning, TEXT("CHUNK_CELL") );

            FOdysseyLayerCellImport::Read( iCell, Ar, chunkEnd );
        break;

        default:
            //No chunk found, seek back to the beginning and return false
            Ar.Seek( start );
            return false;
    }
    return true;
}

void
FOdysseyLayerCellImport::Read( UOdysseyLayerCell* iCell
                                            , FArchive &Ar
                                            , uint64 iChunkEnd )
{
    ReadLayerCellChunks( iChunkEnd
                              , Ar
                              , [iCell](uint32 iChunkID, uint64 iChunkLen, FArchive &Ar) -> void
        {
            switch ( iChunkID )
            {
                case CHUNK_CELL_LENGTH :
                {
                    Ar << iCell->Exposure;
                }
                break;

                case CHUNK_CELL_MARKID :
                {
#if WITH_EDITOR
                    Ar << iCell->Mark;
#endif
                }
                break;

                default:
                // Mandatory
                    Ar.Seek( Ar.Tell() + iChunkLen );
                break;
            }
        } );
}
