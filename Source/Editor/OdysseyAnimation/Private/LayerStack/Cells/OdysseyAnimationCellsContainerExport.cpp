#include "LayerStack/Cells/OdysseyAnimationCellsContainerExport.h"
#include "LayerStack/Cells/OdysseyAnimationCellsContainer.h"
#include "LayerStack/Cells/OdysseyAnimationCell.h"
#include "OdysseyFile.h"

void
FOdysseyAnimationCellsContainerExport::WriteOffset( FOdysseyAnimationCellsContainer* iAnimationCellsContainer
                                                       , FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::Animation::CHUNK_CELLSCONTAINER_OFFSET
                            , Ar
                            , [iAnimationCellsContainer](FArchive &Ar) -> void
    {
        Ar << iAnimationCellsContainer->mOffset;
    });
}

void
FOdysseyAnimationCellsContainerExport::WriteCells( FOdysseyAnimationCellsContainer* iAnimationCellsContainer
                                                       , FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::Animation::CHUNK_CELLSCONTAINER_CELLS
                            , Ar
                            , [iAnimationCellsContainer](FArchive &Ar) -> void
    {
        for ( int i = 0; i < iAnimationCellsContainer->mCells.Num(); i++ )
        {
            WriteCellType( iAnimationCellsContainer->mCells[i], Ar );
            WriteCell( iAnimationCellsContainer->mCells[i], Ar );
        }
    });
}

void
FOdysseyAnimationCellsContainerExport::WriteCellType( TSharedPtr<FOdysseyAnimationCell> iCell
                                                       , FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::Animation::CHUNK_CELLSCONTAINER_CELLTYPE
                            , Ar
                            , [iCell](FArchive &Ar) -> void
    {
        //Save the Cell Type
        FName cellType = iCell->GetType();
        Ar << cellType;
    });
}

void
FOdysseyAnimationCellsContainerExport::WriteCell( TSharedPtr<FOdysseyAnimationCell> iCell
                                                       , FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::Animation::CHUNK_CELLSCONTAINER_CELL
                            , Ar
                            , [iCell](FArchive &Ar) -> void
    {
        //Save the Cell
        iCell->Serialize(Ar);
    });
}

void
FOdysseyAnimationCellsContainerExport::Write( FOdysseyAnimationCellsContainer* iAnimationCellsContainer
                                             , FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::Animation::CHUNK_CELLSCONTAINER
                            , Ar
                            , [iAnimationCellsContainer](FArchive &Ar) -> void
    {
        WriteOffset( iAnimationCellsContainer, Ar );
        WriteCells( iAnimationCellsContainer, Ar );
    } );
}
