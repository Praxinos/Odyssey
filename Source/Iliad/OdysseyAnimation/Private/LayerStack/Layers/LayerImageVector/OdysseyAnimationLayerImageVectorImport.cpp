// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyAnimationLayerImageVectorImport.h"

#if WITH_EDITOR
#include "OdysseyAnimationLayerImageVector.h"
#include "OdysseyAnimationCellImageVector.h"
#include "OdysseyAnimationCellsContainerImport.h"
#include "OdysseyLayerCellImageStagger.h"
#include "OdysseyLighttable.h"
#include "OdysseyLighttableImport.h"
#include "OdysseyFile.h"


bool
FOdysseyAnimationLayerImageVectorImport::Read( UOdysseyAnimationLayerImageVector* iAnimationLayerImageVector
                                            , FArchive &Ar )
{
    uint64 start = Ar.Tell();

    uint32 chunkID;
    uint64 chunkLen;
    uint64 chunkEnd;

    // Reads the first chunk (FOdysseyFile::Animation::CHUNK_LAYERIMAGEVECTOR)
    Ar << chunkID;
    Ar << chunkLen;

    chunkEnd = Ar.Tell() + chunkLen;

    switch( chunkID )
    {
        case FOdysseyFile::Animation::CHUNK_LAYERIMAGEVECTOR :
            //UE_LOG(LogTemp, Warning, TEXT("CHUNK_LAYERIMAGEVECTOR") );

            FOdysseyAnimationLayerImageVectorImport::Read( iAnimationLayerImageVector, Ar, chunkEnd );
        break;

        default:
            //No chunk found, seek back to the beginning and return false
            Ar.Seek( start );
            return false;
    }
    return true;
}

void
FOdysseyAnimationLayerImageVectorImport::Read( UOdysseyAnimationLayerImageVector* iAnimationLayerImageVector
                                            , FArchive &Ar
                                            , uint64 iChunkEnd )
{
    FOdysseyFile::ReadChunks( iChunkEnd
                              , Ar
                              , [iAnimationLayerImageVector](uint32 iChunkID, uint64 iChunkLen, FArchive &Ar) -> void
        {
            switch ( iChunkID )
            {
                case FOdysseyFile::Animation::CHUNK_LAYERIMAGEVECTOR_CELLSCONTAINER :
                {
                    //DEPRECATED: Keep for compatibility with Odyssey 2.0 and prior
                    if (!FOdysseyAnimationCellsContainerImport::Read( iAnimationLayerImageVector, Ar ))
                    {
                        //Old Style No Chunk Loading
                        //Load or Save the offset
                        Ar << iAnimationLayerImageVector->CellsOffset;

                        //Empty Cells to prepare for loading
                        iAnimationLayerImageVector->RemoveAllCells();

                        //Load or Save number of cells
                        int32 numCells = 0;
                        Ar << numCells;

                        for ( int i = 0; i < numCells; i++ )
                        {
                            //Load the cell type
                            FName cellType;
                            Ar << cellType;

                            if (cellType == TEXT("FOdysseyAnimationCellImageVector"))
                            {
                                UOdysseyLayerCell* cell = iAnimationLayerImageVector->AddCell(UOdysseyAnimationCellImageVector::StaticClass());
                                cell->OldSerialize(Ar);
                            }
                            else if (cellType == TEXT("FOdysseyAnimationCellImageStagger"))
                            {
                                UOdysseyLayerCell* cell = iAnimationLayerImageVector->AddCell(UOdysseyLayerCellImageStagger::StaticClass());
                                cell->OldSerialize(Ar);
                            }
                        }
                    }
                }
                break;

                case FOdysseyFile::Animation::CHUNK_LAYERIMAGEVECTOR_Lighttable :
                {
                    //DEPRECATED: Keep for compatibility with Odyssey 2.0 and prior
                    FOdysseyLighttableImport::Read( &iAnimationLayerImageVector->Lighttable, Ar );
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
