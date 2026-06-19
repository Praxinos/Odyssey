// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyLighttableImport.h"

#if WITH_EDITOR
#include "OdysseyLighttable.h"
#include "OdysseyLighttableKeyImport.h"
#include <functional>

static const uint32 CHUNK_LIGHTTABLE = 0x5240eec2; //container
static const uint32 CHUNK_LIGHTTABLE_DISPLAY_POSITION = 0x472555c; //EOdysseyLighttableDisplayPosition
static const uint32 CHUNK_LIGHTTABLE_COLORS = 0x854e88aa; //EOdysseyLighttableDisplayPosition
static const uint32 CHUNK_LIGHTTABLE_CONTRAST = 0x4e98233c; //FLinearColor(PreviousKeysColor) + FLinearColor(NextKeysColor)
static const uint32 CHUNK_LIGHTTABLE_KEYS = 0x825ce6e5; //20x FOdysseyLighttableKey


void
ReadLighttableChunks( uint64 iChunkEnd, FArchive &Ar, std::function<void(uint32, uint64, FArchive&)> iCallback )
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
FOdysseyLighttableImport::Read( FOdysseyLighttable* iLighttable
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
        case CHUNK_LIGHTTABLE :
            FOdysseyLighttableImport::Read( iLighttable, Ar, chunkEnd );
        break;

        default:
            //No chunk found, seek back to the beginning and return false
            Ar.Seek( start );
            return false;
    }
    return true;
}

void
FOdysseyLighttableImport::Read( FOdysseyLighttable* iLighttable
                                            , FArchive &Ar
                                            , uint64 iChunkEnd )
{
    ReadLighttableChunks( iChunkEnd
                              , Ar
                              , [iLighttable](uint32 iChunkID, uint64 iChunkLen, FArchive &Ar) -> void
        {

            switch ( iChunkID )
            {
                case CHUNK_LIGHTTABLE_DISPLAY_POSITION:
                {
                    uint32 displayPosition;
                    Ar << displayPosition;
                    iLighttable->DisplayPosition = (EOdysseyLighttableDisplayPosition)displayPosition;
                }
                break;

                case CHUNK_LIGHTTABLE_COLORS:
                {
                    Ar << iLighttable->PreviousKeysColor;
                    Ar << iLighttable->NextKeysColor;
                }
                break;

                case CHUNK_LIGHTTABLE_CONTRAST:
                {
                    float prev = 0.f;
                    float next = 0.f;
                    Ar << prev;
                    Ar << next;
                    iLighttable->PreviousKeysContrast = prev * 100.f;
                    iLighttable->NextKeysContrast = next * 100.f;
                }
                break;

                case CHUNK_LIGHTTABLE_KEYS:
                {
                    for ( int i = 0; i < 10; i++ )
                    {
                        FOdysseyLighttableKeyImport::Read( &iLighttable->PreviousKeys[i], Ar );
                    }

                    for ( int i = 0; i < 10; i++ )
                    {
                        FOdysseyLighttableKeyImport::Read( &iLighttable->NextKeys[i], Ar );
                    }
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
