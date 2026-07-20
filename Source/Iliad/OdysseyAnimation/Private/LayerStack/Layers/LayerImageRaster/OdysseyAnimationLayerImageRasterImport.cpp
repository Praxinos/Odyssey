// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyAnimationLayerImageRasterImport.h"

#if WITH_EDITOR
#include "OdysseyAnimationLayerImageRaster.h"
#include "OdysseyAnimationCellImageRaster.h"
#include "OdysseyLayerCellImageStagger.h"
#include "OdysseyLighttable.h"
#include "OdysseyLighttableImport.h"
#include "OdysseyAnimationCellsContainerImport.h"
#include "OdysseyFile.h"

bool
FOdysseyAnimationLayerImageRasterImport::Read( UOdysseyAnimationLayerImageRaster* iAnimationLayerImageRaster
                                            , FArchive &Ar )
{
    if (Ar.AtEnd() || Ar.IsCriticalError())
        return true;

    uint64 start = Ar.Tell();

    uint32 chunkID;
    uint64 chunkLen;
    uint64 chunkEnd;

    // Reads the first chunk (FOdysseyFile::Animation::CHUNK_LAYERIMAGERASTER)
    Ar << chunkID;
    Ar << chunkLen;

    //PATCH:
    // If we read out of the Linker bounds (which is not the same as the Archive bounds)
    // The archive generates a CriticalError
    // We don't have access to the linker bounds, so we can only rely on the critical error state
    // We then need to seek to start again, like nothing ever happened
    // And clear the Critical Error state
    if (Ar.IsCriticalError())
    {
        Ar.Seek( start );
        Ar.ClearCriticalError();
        return true;
    }

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
                        iAnimationLayerImageRaster->RemoveAllCells();

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
                                UOdysseyLayerCell* cell = iAnimationLayerImageRaster->AddCell(UOdysseyAnimationCellImageRaster::StaticClass());
                                cell->OldSerialize(Ar);
                            }
                            else if (cellType == TEXT("FOdysseyAnimationCellImageStagger"))
                            {
                                UOdysseyLayerCell* cell = iAnimationLayerImageRaster->AddCell(UOdysseyLayerCellImageStagger::StaticClass());
                                cell->OldSerialize(Ar);
                            }
                        }
                    }
                }
                break;

                case FOdysseyFile::Animation::CHUNK_LAYERIMAGERASTER_Lighttable :
                {
                    //DEPRECATED: Keep for compatibility with Odyssey 2.0 and prior
                    FOdysseyLighttableImport::Read( &iAnimationLayerImageRaster->Lighttable, Ar );
                }
                break;

                default:
                // Mandatory
                    Ar.Seek( Ar.Tell() + iChunkLen );
                break;
            }
        } );
}
#endif
