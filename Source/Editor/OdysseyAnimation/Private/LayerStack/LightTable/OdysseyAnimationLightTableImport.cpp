#include "LayerStack/LightTable/OdysseyAnimationLightTableImport.h"
#include "LayerStack/LightTable/OdysseyAnimationLightTable.h"
#include "LayerStack/LightTable/OdysseyAnimationLightTableKeyImport.h"
#include "LayerStack/LightTable/OdysseyAnimationLightTableKeyExport.h"
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
                    Ar << iAnimationLightTable->DisplayPosition;
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
                    Ar << iAnimationLightTable->PreviousKeysContrast;
                    Ar << iAnimationLightTable->NextKeysContrast;
                }
                break;

                case FOdysseyFile::Animation::CHUNK_LIGHTTABLE_KEYS:
                {
                    for ( int i = 0; i < 10; i++ )
                    {
                        FOdysseyAnimationLightTableKeyExport::Write( &iAnimationLightTable->PreviousKeys[i], Ar );
                    }

					for ( int i = 0; i < 10; i++ )
                    {
                        FOdysseyAnimationLightTableKeyExport::Write( &iAnimationLightTable->NextKeys[i], Ar );
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
