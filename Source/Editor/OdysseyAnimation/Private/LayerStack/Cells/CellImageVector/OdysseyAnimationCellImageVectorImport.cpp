#include "LayerStack/Cells/CellImageVector/OdysseyAnimationCellImageVectorImport.h"
#include "LayerStack/Cells/CellImageVector/OdysseyAnimationCellImageVector.h"
#include "Import/v2/OdysseyVectorImport.h"
#include "OdysseyFile.h"

void
FOdysseyAnimationCellImageVectorImport::Read( FOdysseyAnimationCellImageVector* iAnimationCellImageVector
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

                    iAnimationCellImageVector->SetWidth( width );
                    iAnimationCellImageVector->SetHeight( height );

                    if( iAnimationCellImageVector->GetEngine() == nullptr )
                    {
                        iAnimationCellImageVector->Init( width, height );
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

                    UE_LOG(LogTemp, Warning, TEXT("CHUNK_VECTOR_MAGIC_V2") );

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
