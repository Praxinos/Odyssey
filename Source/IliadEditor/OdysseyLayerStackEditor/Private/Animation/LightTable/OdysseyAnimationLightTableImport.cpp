// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyAnimationLightTableImport.h"
#include "OdysseyAnimationLightTable.h"
#include "OdysseyAnimationLightTableKeyImport.h"
#include "OdysseyFile.h"

bool
FOdysseyAnimationLightTableImport::Read( FOdysseyAnimationLightTable* iAnimationLightTable
                                            , FArchive &Ar )
{
    uint64 start = Ar.Tell();

    uint32 chunkID;
    uint64 chunkLen;
    uint64 chunkEnd;

    // Reads the first chunk (FOdysseyFile::Animation::CHUNK_LightTable)
    Ar << chunkID;
    Ar << chunkLen;

    chunkEnd = Ar.Tell() + chunkLen;

    switch( chunkID )
    {
        case FOdysseyFile::Animation::CHUNK_LIGHTTABLE :
            FOdysseyAnimationLightTableImport::Read( iAnimationLightTable, Ar, chunkEnd );
        break;

        default:
            //No chunk found, seek back to the beginning and return false
            Ar.Seek( start );
            return false;
    }
    return true;
}

void
FOdysseyAnimationLightTableImport::Read( FOdysseyAnimationLightTable* iAnimationLightTable
                                            , FArchive &Ar
                                            , uint64 iChunkEnd )
{
    FOdysseyFile::ReadChunks( iChunkEnd
                              , Ar
                              , [iAnimationLightTable](uint32 iChunkID, uint64 iChunkLen, FArchive &Ar) -> void
        {

            switch ( iChunkID )
            {
                case FOdysseyFile::Animation::CHUNK_LIGHTTABLE_DISPLAY_POSITION:
                {
                    uint32 displayPosition;
                    Ar << displayPosition;
                    iAnimationLightTable->DisplayPosition = (EOdysseyLightTableDisplayPosition)displayPosition;
                }
                break;

                case FOdysseyFile::Animation::CHUNK_LIGHTTABLE_COLORS:
                {
                    Ar << iAnimationLightTable->PreviousKeysColor;
                    Ar << iAnimationLightTable->NextKeysColor;
                }
                break;

                case FOdysseyFile::Animation::CHUNK_LIGHTTABLE_CONTRAST:
                {
                    float prev = 0.f;
                    float next = 0.f;
                    Ar << prev;
                    Ar << next;
                    iAnimationLightTable->PreviousKeysContrast = prev * 100.f;
                    iAnimationLightTable->NextKeysContrast = next * 100.f;
                }
                break;

                case FOdysseyFile::Animation::CHUNK_LIGHTTABLE_KEYS:
                {
                    for ( int i = 0; i < 10; i++ )
                    {
                        FOdysseyAnimationLightTableKeyImport::Read( &iAnimationLightTable->PreviousKeys[i], Ar );
                    }

                    for ( int i = 0; i < 10; i++ )
                    {
                        FOdysseyAnimationLightTableKeyImport::Read( &iAnimationLightTable->NextKeys[i], Ar );
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
