#include "AnimationCellImageVector/OdysseyImportAnimationCellImageVector.h"
#include "LayerStack/Cells/CellImageVector/OdysseyAnimationCellImageVector.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Vector/OdysseyImportVectorV2.h"

void
FOdysseyImportAnimationCellImageVector::Read( FOdysseyAnimationCellImageVector* iAnimationCellImageVector
                                            , FArchive &Ar
                                            , uint64 iChunkEnd )
{
    FOdysseyImport::ReadChunks( iChunkEnd
                              , Ar
                              , [iAnimationCellImageVector](uint32 iChunkID, uint64 iChunkLen, FArchive &Ar) -> void
        {
            switch ( iChunkID )
            {
                case FOdysseyExportAnimationCellImageVector::CHUNK_ANIMATIONCELLIMAGEVECTOR_RESOLUTION :
                {
                    uint32 width;
                    uint32 height;

                    Ar << width;
                    Ar << height;

                    iAnimationCellImageVector->SetWidth( width );
                    iAnimationCellImageVector->SetHeight( height );
                }
                break;

                // container
                case FOdysseyExportAnimationCellImageVector::CHUNK_ANIMATIONCELLIMAGEVECTOR_BLOCK:
                break;

                case FOdysseyExportAnimationCellImageVector::CHUNK_ANIMATIONCELLIMAGEVECTOR_BLOCK_ID:
                {
                    FGuid vectorBlockID;

                    Ar << vectorBlockID;

                    iAnimationCellImageVector->SetVectorBlockId( vectorBlockID );
                }
                break;

                case FOdysseyExportVectorV2::CHUNK_VECTOR_MAGIC_V2:
                {
                    FOdysseyVectorEngine* vectorEngine = iAnimationCellImageVector->GetEngine();
                    FOdysseyImportVectorV2 importerV2 = FOdysseyImportVectorV2();

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
