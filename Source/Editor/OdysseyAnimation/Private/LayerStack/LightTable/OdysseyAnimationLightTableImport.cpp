#include "LayerStack/LightTable/OdysseyAnimationLightTableImport.h"
#include "LayerStack/LightTable/OdysseyAnimationLightTable.h"
#include "LayerStack/LightTable/OdysseyAnimationLightTableKeyImport.h"
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
                    Ar << iAnimationLightTable->mDisplayPosition;
                }
                break;

                case FOdysseyFile::Animation::CHUNK_LIGHTTABLE_COLORS:
                {
                    Ar << iAnimationLightTable->mPreviousKeysColor;
                    Ar << iAnimationLightTable->mNextKeysColor;
                }
                break;

                case FOdysseyFile::Animation::CHUNK_LIGHTTABLE_CONTRAST:
                {
                    Ar << iAnimationLightTable->mPreviousKeysContrast;
                    Ar << iAnimationLightTable->mNextKeysContrast;
                }
                break;

                case FOdysseyFile::Animation::CHUNK_LIGHTTABLE_KEYS:
                {
                    for ( FOdysseyAnimationLightTableKey& key : iAnimationLightTable->mKeys )
                    {
                        FOdysseyAnimationLightTableKeyExport::Write( &key, Ar );
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
