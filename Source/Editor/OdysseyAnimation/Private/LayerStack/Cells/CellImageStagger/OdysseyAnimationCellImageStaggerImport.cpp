#include "LayerStack/Cells/CellImageStagger/OdysseyAnimationCellImageStaggerImport.h"
#include "LayerStack/Cells/CellImageStagger/OdysseyAnimationCellImageStagger.h"
#include "OdysseyFile.h"

bool
FOdysseyAnimationCellImageStaggerImport::Read( FOdysseyAnimationCellImageStagger* iAnimationCellImageStagger
                                            , FArchive &Ar )
{
    uint64 start = Ar.Tell();

    uint32 chunkID;
    uint64 chunkLen;
    uint64 chunkEnd;

    // Reads the first chunk (FOdysseyFile::Animation::CHUNK_CELLIMAGESTAGGER)
    Ar << chunkID;
    Ar << chunkLen;

    chunkEnd = Ar.Tell() + chunkLen;

    switch( chunkID )
    {
        case FOdysseyFile::Animation::CHUNK_CELLIMAGESTAGGER :
            //UE_LOG(LogTemp, Warning, TEXT("CHUNK_CELLIMAGESTAGGER") );

            FOdysseyAnimationCellImageStaggerImport::Read( iAnimationCellImageStagger, Ar, chunkEnd );
        break;

        default:
            //No chunk found, seek back to the beginning and return false
            Ar.Seek( start );
            return false;
    }
    return true;
}

void
FOdysseyAnimationCellImageStaggerImport::Read( FOdysseyAnimationCellImageStagger* iAnimationCellImageStagger
                                            , FArchive &Ar
                                            , uint64 iChunkEnd )
{
    FOdysseyFile::ReadChunks( iChunkEnd
                              , Ar
                              , [iAnimationCellImageStagger](uint32 iChunkID, uint64 iChunkLen, FArchive &Ar) -> void
        {
            switch ( iChunkID )
            {
                case FOdysseyFile::Animation::CHUNK_CELLIMAGESTAGGER_BEHAVIOUR :
                {
                    Ar << iAnimationCellImageStagger->mBehaviour;
                }
                break;

                case FOdysseyFile::Animation::CHUNK_CELLIMAGESTAGGER_REACH :
                {
                    Ar << iAnimationCellImageStagger->mReach;
                }
                break;

                default:
				// Mandatory
                    Ar.Seek( Ar.Tell() + iChunkLen );
                break;
            }
        } );
}
