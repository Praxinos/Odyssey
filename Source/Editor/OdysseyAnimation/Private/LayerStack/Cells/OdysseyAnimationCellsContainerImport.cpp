#include "LayerStack/Cells/OdysseyAnimationCellsContainerImport.h"
#include "LayerStack/Cells/OdysseyAnimationCellsContainer.h"
#include "OdysseyFile.h"

bool
FOdysseyAnimationCellsContainerImport::Read( FOdysseyAnimationCellsContainer* iAnimationCellsContainer
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
            UE_LOG(LogTemp, Warning, TEXT("CHUNK_CELLSCONTAINER") );

            FOdysseyAnimationCellsContainerImport::Read( iAnimationCellsContainer, Ar, chunkEnd );
        break;

        default:
            //No chunk found, seek back to the beginning and return false
            Ar.Seek( start );
            return false;
    }
    return true;
}

void
FOdysseyAnimationCellsContainerImport::Read( FOdysseyAnimationCellsContainer* iAnimationCellsContainer
                                            , FArchive &Ar
                                            , uint64 iChunkEnd )
{
    iAnimationCellsContainer->mCells.Empty();

    FOdysseyFile::ReadChunks( iChunkEnd
                              , Ar
                              , [iAnimationCellsContainer](uint32 iChunkID, uint64 iChunkLen, FArchive &Ar) -> void
        {
            switch ( iChunkID )
            {
                case FOdysseyFile::Animation::CHUNK_CELLSCONTAINER_OFFSET :
                {
                    Ar << iAnimationCellsContainer->mOffset;
                }
                break;

                //Container
                case FOdysseyFile::Animation::CHUNK_CELLSCONTAINER_CELLS :
                break;

                
                case FOdysseyFile::Animation::CHUNK_CELLSCONTAINER_CELLTYPE :
                {
                    FName cellType;
                    Ar << cellType;

                    //Create a cell of the given type
                    TSharedPtr<FOdysseyAnimationCell> cell = iAnimationCellsContainer->mCreateCell.Execute(cellType, true);
                    checkf(!!cell, TEXT("Failed to create a cell of the given type"));
                    iAnimationCellsContainer->mCells.Add(cell);
                }
                break;

                
                case FOdysseyFile::Animation::CHUNK_CELLSCONTAINER_CELL :
                {
                    iAnimationCellsContainer->mCells.Last()->Serialize(Ar);
                }
                break;

                default:
				// Mandatory
                    Ar.Seek( Ar.Tell() + iChunkLen );
                break;
            }
        } );
}
