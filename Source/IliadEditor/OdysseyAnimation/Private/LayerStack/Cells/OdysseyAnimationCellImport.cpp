// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/Cells/OdysseyAnimationCellImport.h"
#include "LayerStack/Cells/OdysseyAnimationCell.h"
#include "OdysseyFile.h"

bool
FOdysseyAnimationCellImport::Read( UOdysseyAnimationCell* iAnimationCell
                                            , FArchive &Ar )
{
    uint64 start = Ar.Tell();

    uint32 chunkID;
    uint64 chunkLen;
    uint64 chunkEnd;

    // Reads the first chunk (FOdysseyFile::Animation::CHUNK_CELL)
    Ar << chunkID;
    Ar << chunkLen;

    chunkEnd = Ar.Tell() + chunkLen;

    switch( chunkID )
    {
        case FOdysseyFile::Animation::CHUNK_CELL :
            //UE_LOG(LogTemp, Warning, TEXT("CHUNK_CELL") );

            FOdysseyAnimationCellImport::Read( iAnimationCell, Ar, chunkEnd );
        break;

        default:
            //No chunk found, seek back to the beginning and return false
            Ar.Seek( start );
            return false;
    }
    return true;
}

void
FOdysseyAnimationCellImport::Read( UOdysseyAnimationCell* iAnimationCell
                                            , FArchive &Ar
                                            , uint64 iChunkEnd )
{
    FOdysseyFile::ReadChunks( iChunkEnd
                              , Ar
                              , [iAnimationCell](uint32 iChunkID, uint64 iChunkLen, FArchive &Ar) -> void
        {
            switch ( iChunkID )
            {
                case FOdysseyFile::Animation::CHUNK_CELL_LENGTH :
                {
                    Ar << iAnimationCell->Exposure;
                }
                break;

                case FOdysseyFile::Animation::CHUNK_CELL_MARKID :
                {
                    Ar << iAnimationCell->Mark;
                }
                break;

                default:
                // Mandatory
                    Ar.Seek( Ar.Tell() + iChunkLen );
                break;
            }
        } );
}
