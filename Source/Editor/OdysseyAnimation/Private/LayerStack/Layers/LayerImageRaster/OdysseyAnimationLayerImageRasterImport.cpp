#include "LayerStack/Layers/LayerImageRaster/OdysseyAnimationLayerImageRasterImport.h"
#include "LayerStack/Layers/LayerImageRaster/OdysseyAnimationLayerImageRaster.h"
#include "LayerStack/Cells/CellImageRaster/OdysseyAnimationCellImageRaster.h"
#include "LayerStack/Cells/CellImageStagger/OdysseyAnimationCellImageStagger.h"
#include "LayerStack/LightTable/OdysseyAnimationLightTable.h"
#include "LayerStack/LightTable/OdysseyAnimationLightTableImport.h"
#include "LayerStack/Cells/OdysseyAnimationCellsContainerImport.h"
#include "OdysseyFile.h"


bool
FOdysseyAnimationLayerImageRasterImport::Read( UOdysseyAnimationLayerImageRaster* iAnimationLayerImageRaster
                                            , FArchive &Ar )
{
    uint64 start = Ar.Tell();

    uint32 chunkID;
    uint64 chunkLen;
    uint64 chunkEnd;

    // Reads the first chunk (FOdysseyFile::Animation::CHUNK_LAYERIMAGERASTER)
    Ar << chunkID;
    Ar << chunkLen;

    chunkEnd = Ar.Tell() + chunkLen;

    switch( chunkID )
    {
        case FOdysseyFile::Animation::CHUNK_LAYERIMAGERASTER :
            //UE_LOG(LogTemp, Warning, TEXT("CHUNK_LAYERIMAGERASTER") );

            FOdysseyAnimationLayerImageRasterImport::Read( iAnimationLayerImageRaster, Ar, chunkEnd );
        break;

        default:
            //No chunk found, seek back to the beginning and return false
            Ar.Seek( start );
            return false;
    }
    return true;
}

void
FOdysseyAnimationLayerImageRasterImport::Read( UOdysseyAnimationLayerImageRaster* iAnimationLayerImageRaster
                                            , FArchive &Ar
                                            , uint64 iChunkEnd )
{
    FOdysseyFile::ReadChunks( iChunkEnd
                              , Ar
                              , [iAnimationLayerImageRaster](uint32 iChunkID, uint64 iChunkLen, FArchive &Ar) -> void
        {
            switch ( iChunkID )
            {
                case FOdysseyFile::Animation::CHUNK_LAYERIMAGERASTER_CELLSCONTAINER :
                {
					//DEPRECATED: Keep for compatibility with Odyssey 2.0 and prior
					if (!FOdysseyAnimationCellsContainerImport::Read( iAnimationLayerImageRaster, Ar ))
					{
						//Old Style No Chunk Loading
						//Load or Save the offset
						Ar << iAnimationLayerImageRaster->CellsOffset;

						//Empty Cells to prepare for loading
						iAnimationLayerImageRaster->Cells.Empty();

						//Load or Save number of cells
						int32 numCells = 0;
						Ar << numCells;

						for ( int i = 0; i < numCells; i++ )
						{
							//Load the cell type
							FName cellType;
							Ar << cellType;

							if (cellType == TEXT("FOdysseyAnimationCellImageRaster"))
							{
								UOdysseyAnimationCellImageRaster* cell = NewObject<UOdysseyAnimationCellImageRaster>(iAnimationLayerImageRaster, UOdysseyAnimationCellImageRaster::StaticClass(), NAME_None, RF_Public | RF_Transactional);
								iAnimationLayerImageRaster->Cells.Add(cell);
								cell->OldSerialize(Ar);
							}
							else if (cellType == TEXT("FOdysseyAnimationCellImageStagger"))
							{
								UOdysseyAnimationCellImageStagger* cell = NewObject<UOdysseyAnimationCellImageStagger>(iAnimationLayerImageRaster, UOdysseyAnimationCellImageStagger::StaticClass(), NAME_None, RF_Public | RF_Transactional);
								iAnimationLayerImageRaster->Cells.Add(cell);
								cell->OldSerialize(Ar);
							}
						}
					}
                }
                break;

                case FOdysseyFile::Animation::CHUNK_LAYERIMAGERASTER_LIGHTTABLE :
                {
					//DEPRECATED: Keep for compatibility with Odyssey 2.0 and prior
					FOdysseyAnimationLightTableImport::Read( &iAnimationLayerImageRaster->Lighttable, Ar );
                }
                break;

                default:
				// Mandatory
                    Ar.Seek( Ar.Tell() + iChunkLen );
                break;
            }
        } );
}
