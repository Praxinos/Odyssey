// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyLayerCellImageStaggerImport.h"
#include "OdysseyLayerCellImageStagger.h"
#include <functional>

static const uint32 CHUNK_CELLIMAGESTAGGER = 0x559ff069; // container
static const uint32 CHUNK_CELLIMAGESTAGGER_BEHAVIOUR = 0xdd0542e9; // uint32 (Behaviour)
static const uint32 CHUNK_CELLIMAGESTAGGER_REACH = 0x98930ed0; // uint32 (Reach)

void
ReadLayerCellImageStaggerChunks( uint64 iChunkEnd, FArchive &Ar, std::function<void(uint32, uint64, FArchive&)> iCallback )
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
FOdysseyLayerCellImageStaggerImport::Read( UOdysseyLayerCellImageStagger* iLayerCellImageStagger
                                            , FArchive &Ar )
{
    uint64 start = Ar.Tell();

    uint32 chunkID;
    uint64 chunkLen;
    uint64 chunkEnd;

    // Reads the first chunk (CHUNK_CELLIMAGESTAGGER)
    Ar << chunkID;
    Ar << chunkLen;

    chunkEnd = Ar.Tell() + chunkLen;

    switch( chunkID )
    {
        case CHUNK_CELLIMAGESTAGGER :
            //UE_LOG(LogTemp, Warning, TEXT("CHUNK_CELLIMAGESTAGGER") );

            FOdysseyLayerCellImageStaggerImport::Read( iLayerCellImageStagger, Ar, chunkEnd );
        break;

        default:
            //No chunk found, seek back to the beginning and return false
            Ar.Seek( start );
            return false;
    }
    return true;
}

void
FOdysseyLayerCellImageStaggerImport::Read( UOdysseyLayerCellImageStagger* iLayerCellImageStagger
                                            , FArchive &Ar
                                            , uint64 iChunkEnd )
{
    ReadLayerCellImageStaggerChunks( iChunkEnd
                              , Ar
                              , [iLayerCellImageStagger](uint32 iChunkID, uint64 iChunkLen, FArchive &Ar) -> void
        {
            switch ( iChunkID )
            {
                case CHUNK_CELLIMAGESTAGGER_BEHAVIOUR :
                {
                    uint32 behaviour;
                    Ar << behaviour;

                    behaviour = FMath::Max(uint32(0), behaviour - 1); //first value was "Invalid" which does not exist anymore
                    iLayerCellImageStagger->Behaviour = (EOdysseyLayerCellImageStaggerBehaviour)behaviour;
                }
                break;

                case CHUNK_CELLIMAGESTAGGER_REACH :
                {
                    Ar << iLayerCellImageStagger->Reach;
                }
                break;

                default:
                // Mandatory
                    Ar.Seek( Ar.Tell() + iChunkLen );
                break;
            }
        } );
}
