#include "LayerStack/Layers/LayerImageVector/OdysseyAnimationLayerImageVectorExport.h"
#include "LayerStack/Layers/LayerImageVector/OdysseyAnimationLayerImageVector.h"
#include "OdysseyFile.h"

void
FOdysseyAnimationLayerImageVectorExport::WriteCellsContainer( UOdysseyAnimationLayerImageVector* iAnimationLayerImageVector
                                                       , FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::Animation::CHUNK_LAYERIMAGEVECTOR_CELLSCONTAINER
                            , Ar
                            , [iAnimationLayerImageVector](FArchive &Ar) -> void
    {
        iAnimationLayerImageVector->mCellsContainer->Serialize(Ar);
    });
}

void
FOdysseyAnimationLayerImageVectorExport::WriteLightTable( UOdysseyAnimationLayerImageVector* iAnimationLayerImageVector
                                                       , FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::Animation::CHUNK_LAYERIMAGEVECTOR_LIGHTTABLE
                            , Ar
                            , [iAnimationLayerImageVector](FArchive &Ar) -> void
    {
        iAnimationLayerImageVector->mLightTable->Serialize(Ar);
    });
}

void
FOdysseyAnimationLayerImageVectorExport::Write( UOdysseyAnimationLayerImageVector* iAnimationLayerImageVector
                                             , FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::Animation::CHUNK_LAYERIMAGEVECTOR
                            , Ar
                            , [iAnimationLayerImageVector](FArchive &Ar) -> void
    {
        WriteCellsContainer( iAnimationLayerImageVector, Ar );
        WriteLightTable( iAnimationLayerImageVector, Ar );
    } );
}
