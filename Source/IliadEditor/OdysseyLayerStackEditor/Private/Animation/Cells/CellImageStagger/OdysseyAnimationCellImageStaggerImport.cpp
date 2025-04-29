// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyAnimationCellImageStaggerImport.h"
#include "OdysseyAnimationCellImageStagger.h"
#include "OdysseyFile.h"

bool
FOdysseyAnimationCellImageStaggerImport::Read( UOdysseyAnimationCellImageStagger* iAnimationCellImageStagger
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
FOdysseyAnimationCellImageStaggerImport::Read( UOdysseyAnimationCellImageStagger* iAnimationCellImageStagger
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
                    uint32 behaviour;
                    Ar << behaviour;

                    behaviour = FMath::Max(uint32(0), behaviour - 1); //first value was "Invalid" which does not exist anymore
                    iAnimationCellImageStagger->Behaviour = (EOdysseyAnimationCellImageStaggerBehaviour)behaviour;
                }
                break;

                case FOdysseyFile::Animation::CHUNK_CELLIMAGESTAGGER_REACH :
                {
                    Ar << iAnimationCellImageStagger->Reach;
                }
                break;

                default:
                // Mandatory
                    Ar.Seek( Ar.Tell() + iChunkLen );
                break;
            }
        } );
}
