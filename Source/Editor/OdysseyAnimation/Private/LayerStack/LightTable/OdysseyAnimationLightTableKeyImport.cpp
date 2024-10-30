#include "LayerStack/LightTable/OdysseyAnimationLightTableKeyImport.h"
#include "LayerStack/LightTable/OdysseyAnimationLightTable.h"
#include "OdysseyFile.h"

bool
FOdysseyAnimationLightTableKeyImport::Read( FOdysseyAnimationLightTableKey* iKey
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
        case FOdysseyFile::Animation::CHUNK_LIGHTTABLE_KEY :
            FOdysseyAnimationLightTableKeyImport::Read( iKey, Ar, chunkEnd );
        break;

        default:
            //No chunk found, seek back to the beginning and return false
            Ar.Seek( start );
            return false;
    }
    return true;
}

void
FOdysseyAnimationLightTableKeyImport::Read( FOdysseyAnimationLightTableKey* iKey
                                            , FArchive &Ar
                                            , uint64 iChunkEnd )
{
    FOdysseyFile::ReadChunks( iChunkEnd
                              , Ar
                              , [iKey](uint32 iChunkID, uint64 iChunkLen, FArchive &Ar) -> void
        {           
            switch ( iChunkID )
            {
                case FOdysseyFile::Animation::CHUNK_LIGHTTABLE_KEY_ISACTIVATED:
                {
                    Ar << iKey->bIsActivated;
                }
                break;

                case FOdysseyFile::Animation::CHUNK_LIGHTTABLE_KEY_OPACITY:
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
