// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyLighttableKeyImport.h"

#if WITH_EDITOR
#include "OdysseyLighttable.h"
#include <functional>

static const uint32 CHUNK_LIGHTTABLE_KEY = 0x5b785423; //FOdysseyLighttableKey
static const uint32 CHUNK_LIGHTTABLE_KEY_ISACTIVATED = 0xea8a5d86; //bool
static const uint32 CHUNK_LIGHTTABLE_KEY_OPACITY = 0x21ab66; //float


void
ReadLighttableKeyChunks( uint64 iChunkEnd, FArchive &Ar, std::function<void(uint32, uint64, FArchive&)> iCallback )
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
FOdysseyLighttableKeyImport::Read( FOdysseyLighttableKey* iKey
                                            , FArchive &Ar )
{
    uint64 start = Ar.Tell();

    uint32 chunkID;
    uint64 chunkLen;
    uint64 chunkEnd;

    // Reads the first chunk (CHUNK_LIGHTTABLE)
    Ar << chunkID;
    Ar << chunkLen;

    chunkEnd = Ar.Tell() + chunkLen;

    switch( chunkID )
    {
        case CHUNK_LIGHTTABLE_KEY :
            FOdysseyLighttableKeyImport::Read( iKey, Ar, chunkEnd );
        break;

        default:
            //No chunk found, seek back to the beginning and return false
            Ar.Seek( start );
            return false;
    }
    return true;
}

void
FOdysseyLighttableKeyImport::Read( FOdysseyLighttableKey* iKey
                                            , FArchive &Ar
                                            , uint64 iChunkEnd )
{
    ReadLighttableKeyChunks( iChunkEnd
                              , Ar
                              , [iKey](uint32 iChunkID, uint64 iChunkLen, FArchive &Ar) -> void
        {
            switch ( iChunkID )
            {
                case CHUNK_LIGHTTABLE_KEY_ISACTIVATED:
                {
                    Ar << iKey->bIsActivated;
                }
                break;

                case CHUNK_LIGHTTABLE_KEY_OPACITY:
                {
                    float opacity = 0.f;
                    Ar << opacity;
                    iKey->Opacity = opacity * 100.f;
                }
                break;

                default:
                // Mandatory
                    Ar.Seek( Ar.Tell() + iChunkLen );
                break;
            }
        } );
}
#endif
