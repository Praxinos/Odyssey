// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/Cells/CellImageVector/OdysseyAnimationCellImageVectorExport.h"
#include "LayerStack/Cells/CellImageVector/OdysseyAnimationCellImageVector.h"
#include "LayerStack/Layers/LayerImageVector/OdysseyAnimationLayerImageVector.h"
#include "OdysseyAnimation.h"
#include "Export/v2/OdysseyVectorExport.h"
#include "OdysseyFile.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorLayer.h"
#include "OdysseyVectorCell.h"

void
FOdysseyAnimationCellImageVectorExport::WriteResolution( UOdysseyAnimationCellImageVector* iAnimationCellImageVector
                                                       , FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::Animation::CHUNK_CELLIMAGEVECTOR_RESOLUTION
                            , Ar
                            , [iAnimationCellImageVector](FArchive &Ar) -> void
    {
        uint32 width = iAnimationCellImageVector->GetLayer()->GetAnimation()->GetWidth();
        uint32 height = iAnimationCellImageVector->GetLayer()->GetAnimation()->GetHeight();

        Ar << width;
        Ar << height;
    } );
}

void
FOdysseyAnimationCellImageVectorExport::WriteVectorBlockID( UOdysseyAnimationCellImageVector* iAnimationCellImageVector
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
FOdysseyAnimationCellImageVectorExport::WriteVectorBlock( UOdysseyAnimationCellImageVector* iAnimationCellImageVector
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
FOdysseyAnimationCellImageVectorExport::Write( UOdysseyAnimationCellImageVector* iAnimationCellImageVector
                                             , FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::Animation::CHUNK_CELLIMAGEVECTOR
                            , Ar
                            , [iAnimationCellImageVector](FArchive &Ar) -> void
    {
        FOdysseyVectorCell* vectorCell = iAnimationCellImageVector->GetVectorCell();

        WriteResolution( iAnimationCellImageVector, Ar );
        WriteVectorBlock( iAnimationCellImageVector, Ar );
        // engine might be NULL because this function is sometimes called even before loading anything
        // so we have to check the validity of the pointer
        if( vectorCell )
        {
            FOdysseyVectorExportV2::Write( vectorCell->GetScene(), Ar );
        }
    } );
}
