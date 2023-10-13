#include "LayerStack/Cells/OdysseyAnimationCellExport.h"
#include "LayerStack/Cells/OdysseyAnimationCell.h"
#include "OdysseyFile.h"

void
FOdysseyAnimationCellExport::WriteLength( FOdysseyAnimationCell* iAnimationCell
                                                       , FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::Animation::CHUNK_CELL_LENGTH
                            , Ar
                            , [iAnimationCell](FArchive &Ar) -> void
    {
        Ar << iAnimationCell->mLength;
    });
}

void
FOdysseyAnimationCellExport::Write( FOdysseyAnimationCell* iAnimationCell
                                             , FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::Animation::CHUNK_CELL
                            , Ar
                            , [iAnimationCell](FArchive &Ar) -> void
    {
        WriteLength( iAnimationCell, Ar );
    } );
}
