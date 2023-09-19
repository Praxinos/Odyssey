#include "LayerStack/Cells/CellImageVector/OdysseyAnimationCellImageVectorExport.h"
#include "LayerStack/Cells/CellImageVector/OdysseyAnimationCellImageVector.h"
#include "Export/v2/OdysseyVectorExport.h"
#include "OdysseyFile.h"

void
FOdysseyAnimationCellImageVectorExport::WriteResolution( FOdysseyAnimationCellImageVector* iAnimationCellImageVector
                                                       , FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::Animation::CHUNK_CELLIMAGEVECTOR_RESOLUTION
                            , Ar
                            , [iAnimationCellImageVector](FArchive &Ar) -> void
    {
        uint32 width = iAnimationCellImageVector->GetWidth();
        uint32 height = iAnimationCellImageVector->GetHeight();

        Ar << width;
        Ar << height;
    } );
}

void
FOdysseyAnimationCellImageVectorExport::WriteVectorBlockID( FOdysseyAnimationCellImageVector* iAnimationCellImageVector
                                                          , FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::Animation::CHUNK_CELLIMAGEVECTOR_BLOCK_ID
                            , Ar
                            , [iAnimationCellImageVector](FArchive &Ar) -> void
    {
        FGuid vectorBlockID = iAnimationCellImageVector->GetVectorBlockId();

        Ar << vectorBlockID;
    } );
}

void
FOdysseyAnimationCellImageVectorExport::WriteVectorBlock( FOdysseyAnimationCellImageVector* iAnimationCellImageVector
                                                        , FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::Animation::CHUNK_CELLIMAGEVECTOR_BLOCK
                            , Ar
                            , [iAnimationCellImageVector](FArchive &Ar) -> void
    {
        WriteVectorBlockID( iAnimationCellImageVector, Ar );
    } );
}

void
FOdysseyAnimationCellImageVectorExport::Write( FOdysseyAnimationCellImageVector* iAnimationCellImageVector
                                             , FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::Animation::CHUNK_CELLIMAGEVECTOR
                            , Ar
                            , [iAnimationCellImageVector](FArchive &Ar) -> void
    {
        FOdysseyVectorEngine* vectorEngine = iAnimationCellImageVector->GetEngine();

        WriteResolution( iAnimationCellImageVector, Ar );
        WriteVectorBlock( iAnimationCellImageVector, Ar );
        // engine might be NULL because this function is sometimes called even before loading anything
        // so we have to check the validity of the pointer
        if( vectorEngine )
        {
            FOdysseyVectorExportV2::Write( vectorEngine->GetScene(), Ar );
        }
    } );
}
