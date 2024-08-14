#include "LayerStack/Cells/OdysseyAnimationCellsContainerImport.h"
#include "OdysseyFile.h"
#include "LayerStack/Cells/OdysseyAnimationCell.h"

bool
FOdysseyAnimationCellsContainerImport::Read( UOdysseyAnimationLayer* iAnimationLayer
                                            , FArchive &Ar )
{
    uint64 start = Ar.Tell();

    uint32 chunkID;
    uint64 chunkLen;
    uint64 chunkEnd;

    // Reads the first chunk (FOdysseyFile::Animation::CHUNK_CELLSCONTAINER)
    Ar << chunkID;
    Ar << chunkLen;

    chunkEnd = Ar.Tell() + chunkLen;

    switch( chunkID )
    {
        case FOdysseyFile::Animation::CHUNK_CELLSCONTAINER :
            //UE_LOG(LogTemp, Warning, TEXT("CHUNK_CELLSCONTAINER") );

            FOdysseyAnimationCellsContainerImport::Read( iAnimationLayer, Ar, chunkEnd );
        break;

        default:
            //No chunk found, seek back to the beginning and return false
            Ar.Seek( start );
            return false;
    }
    return true;
}

void
FOdysseyAnimationCellsContainerImport::Read( UOdysseyAnimationLayer* iAnimationLayer
                                            , FArchive &Ar
                                            , uint64 iChunkEnd )
{
    iAnimationLayer->Cells.Empty();

    FOdysseyFile::ReadChunks( iChunkEnd
                              , Ar
                              , [iAnimationLayer](uint32 iChunkID, uint64 iChunkLen, FArchive &Ar) -> void
        {
            switch ( iChunkID )
            {
                case FOdysseyFile::Animation::CHUNK_CELLSCONTAINER_OFFSET :
                {
                    Ar << iAnimationLayer->CellsOffset;
                }
                break;

                //Container
                case FOdysseyFile::Animation::CHUNK_CELLSCONTAINER_CELLS :
                break;

                case FOdysseyFile::Animation::CHUNK_CELLSCONTAINER_CELLTYPE :
                {
                    FName cellType;
                    Ar << cellType;

					if (cellType == TEXT("FOdysseyAnimationCellImageRaster"))
					{
						UOdysseyAnimationCellImageRaster* cell = NewObject<UOdysseyAnimationCellImageRaster>(iAnimationLayer, UOdysseyAnimationCellImageRaster::StaticClass(), NAME_None, RF_Public | RF_Transactional);
                    	iAnimationLayer->Cells.Add(cell);
					}
					else if (cellType == TEXT("FOdysseyAnimationCellImageVector"))
					{
						UOdysseyAnimationCellImageVector* cell = NewObject<UOdysseyAnimationCellImageVector>(iAnimationLayer, UOdysseyAnimationCellImageVector::StaticClass(), NAME_None, RF_Public | RF_Transactional);
						iAnimationLayer->Cells.Add(cell);
					}
					else if (cellType == TEXT("FOdysseyAnimationCellImageStagger"))
					{
						UOdysseyAnimationCellImageStagger* cell = NewObject<UOdysseyAnimationCellImageStagger>(iAnimationLayer, UOdysseyAnimationCellImageStagger::StaticClass(), NAME_None, RF_Public | RF_Transactional);
						iAnimationLayer->Cells.Add(cell);
					}
                }
                break;
                
                case FOdysseyFile::Animation::CHUNK_CELLSCONTAINER_CELL :
                {
                    iAnimationLayer->Cells.Last()->OldSerialize(Ar);
                }
                break;

                default:
				// Mandatory
                    Ar.Seek( Ar.Tell() + iChunkLen );
                break;
            }
        } );
}
