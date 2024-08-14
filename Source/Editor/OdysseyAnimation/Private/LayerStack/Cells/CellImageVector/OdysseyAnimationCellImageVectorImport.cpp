#include "LayerStack/Cells/CellImageVector/OdysseyAnimationCellImageVectorImport.h"
#include "LayerStack/Cells/CellImageVector/OdysseyAnimationCellImageVector.h"
#include "Import/v2/OdysseyVectorImport.h"
#include "OdysseyFile.h"

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

                    //DEPRECATED: No need to use those values, but we still need to read them
					if( iAnimationCellImageVector->GetEngine() == nullptr )
                    {
						iAnimationCellImageVector->mEngine = new FOdysseyVectorEngine( new FOdysseyVectorGroupPaint( "Scene" )
                                       , (double)width
                                       , (double)height );
                    }
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
                    FOdysseyVectorEngine* vectorEngine = iAnimationCellImageVector->GetEngine();
                    FOdysseyVectorImportV2 importerV2 = FOdysseyVectorImportV2();

                    //UE_LOG(LogTemp, Warning, TEXT("CHUNK_VECTOR_MAGIC_V2") );

                    importerV2.Read( vectorEngine->GetScene(), Ar, Ar.Tell() + iChunkLen );
                }
                break;

                default:
				// Mandatory
                    Ar.Seek( Ar.Tell() + iChunkLen );
                break;
            }
        } );
}
