#include "AnimationCellImageVector/OdysseyExportAnimationCellImageVector.h" // from module OdysseyExport
#include "LayerStack/Cells/CellImageVector/OdysseyAnimationCellImageVector.h"
#include "Vector/OdysseyExportVectorV2.h"

void
FOdysseyExportAnimationCellImageVector::WriteResolution( FOdysseyAnimationCellImageVector* iAnimationCellImageVector
                                                       , FArchive &Ar )
{
    FOdysseyExport::WriteChunk( FOdysseyExportAnimationCellImageVector::CHUNK_ANIMATIONCELLIMAGEVECTOR_RESOLUTION
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
FOdysseyExportAnimationCellImageVector::WriteVectorBlockID( FOdysseyAnimationCellImageVector* iAnimationCellImageVector
                                                          , FArchive &Ar )
{
    FOdysseyExport::WriteChunk( FOdysseyExportAnimationCellImageVector::CHUNK_ANIMATIONCELLIMAGEVECTOR_BLOCK_ID
                              , Ar
                              , [iAnimationCellImageVector](FArchive &Ar) -> void
    {
        FGuid vectorBlockID = iAnimationCellImageVector->GetVectorBlockId();

        Ar << vectorBlockID;
    } );
}

void
FOdysseyExportAnimationCellImageVector::WriteVectorBlock( FOdysseyAnimationCellImageVector* iAnimationCellImageVector
                                                        , FArchive &Ar )
{
    FOdysseyExport::WriteChunk( FOdysseyExportAnimationCellImageVector::CHUNK_ANIMATIONCELLIMAGEVECTOR_BLOCK
                              , Ar
                              , [iAnimationCellImageVector](FArchive &Ar) -> void
    {
        WriteVectorBlockID( iAnimationCellImageVector, Ar );
    } );
}

void
FOdysseyExportAnimationCellImageVector::Write( FOdysseyAnimationCellImageVector* iAnimationCellImageVector
                                             , FArchive &Ar )
{
    FOdysseyExport::WriteChunk( FOdysseyExportAnimationCellImageVector::CHUNK_ANIMATIONCELLIMAGEVECTOR
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
            FOdysseyExportVectorV2::Write( vectorEngine->GetScene(), Ar );
        }
    } );
}
