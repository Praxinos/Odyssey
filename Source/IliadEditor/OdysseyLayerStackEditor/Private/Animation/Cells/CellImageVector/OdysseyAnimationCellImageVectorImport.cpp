// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyAnimationCellImageVectorImport.h"
#include "OdysseyAnimationCellImageVector.h"
#include "Import/v2/OdysseyVectorImport.h"
#include "OdysseyFile.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorCell.h"

bool
FOdysseyAnimationCellImageVectorImport::Read( UOdysseyAnimationCellImageVector* iAnimationCellImageVector
                                            , FArchive &Ar )
{
    uint64 start = Ar.Tell();

    uint32 chunkID;
    uint64 chunkLen;
    uint64 chunkEnd;

    // Reads the first chunk (FOdysseyFile::Animation::CHUNK_CELLIMAGEVECTOR)
    Ar << chunkID;
    Ar << chunkLen;

    chunkEnd = Ar.Tell() + chunkLen;

    switch( chunkID )
    {
        case FOdysseyFile::Animation::CHUNK_CELLIMAGEVECTOR :
            //UE_LOG(LogTemp, Warning, TEXT("CHUNK_CELLIMAGEVECTOR") );

            FOdysseyAnimationCellImageVectorImport::Read( iAnimationCellImageVector, Ar, chunkEnd );
        break;

        default:
            //No chunk found, seek back to the beginning and return false
            Ar.Seek( start );
            return false;
    }
    return true;
}

void
FOdysseyAnimationCellImageVectorImport::Read( UOdysseyAnimationCellImageVector* iAnimationCellImageVector
                                            , FArchive &Ar
                                            , uint64 iChunkEnd )
{
    FOdysseyFile::ReadChunks( iChunkEnd
                              , Ar
                              , [iAnimationCellImageVector](uint32 iChunkID, uint64 iChunkLen, FArchive &Ar) -> void
        {
            switch ( iChunkID )
            {
                case FOdysseyFile::Animation::CHUNK_CELLIMAGEVECTOR_RESOLUTION :
                {
                    uint32 width;
                    uint32 height;

                    Ar << width;
                    Ar << height;
                }
                break;

                // container
                case FOdysseyFile::Animation::CHUNK_CELLIMAGEVECTOR_BLOCK:
                break;

                case FOdysseyFile::Animation::CHUNK_CELLIMAGEVECTOR_BLOCK_ID:
                {
                    FGuid vectorBlockID;

                    Ar << vectorBlockID;

                    iAnimationCellImageVector->SetVectorBlockId( vectorBlockID );
                }
                break;

                case FOdysseyFile::VectorV2::CHUNK_VECTOR_MAGIC_V2:
                {
                    FOdysseyVectorCell* vectorCell = iAnimationCellImageVector->GetVectorCell();
                    FOdysseyVectorImportV2* importerV2 = iAnimationCellImageVector->GetImporterV2();

                    //UE_LOG(LogTemp, Warning, TEXT("CHUNK_VECTOR_MAGIC_V2") );

                    importerV2->Read( vectorCell->GetScene(), Ar, Ar.Tell() + iChunkLen );
                }
                break;

                default:
                // Mandatory
                    Ar.Seek( Ar.Tell() + iChunkLen );
                break;
            }
        } );
}
