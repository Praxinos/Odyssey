// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyAnimationCellsContainerImport.h"

#if WITH_EDITOR
#include "OdysseyLayerCellImageStagger.h"
#include "OdysseyAnimationLayer.h"
#include "OdysseyAnimationCellImageRaster.h"
#include "OdysseyAnimationCellImageVector.h"
#include "OdysseyFile.h"

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
    iAnimationLayer->UpdateCellsIndexInLayer();
    return true;
}

void
FOdysseyAnimationCellsContainerImport::Read( UOdysseyAnimationLayer* iAnimationLayer
                                            , FArchive &Ar
                                            , uint64 iChunkEnd )
{
    iAnimationLayer->RemoveAllCells();

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
                        iAnimationLayer->AddCell(UOdysseyAnimationCellImageRaster::StaticClass());
                    }
                    else if (cellType == TEXT("FOdysseyAnimationCellImageVector"))
                    {
                        iAnimationLayer->AddCell(UOdysseyAnimationCellImageVector::StaticClass());
                    }
                    else if (cellType == TEXT("FOdysseyAnimationCellImageStagger"))
                    {
                        iAnimationLayer->AddCell(UOdysseyLayerCellImageStagger::StaticClass());
                    }
                }
                break;

                case FOdysseyFile::Animation::CHUNK_CELLSCONTAINER_CELL :
                {
                    iAnimationLayer->GetCells().Last()->OldSerialize(Ar);
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
