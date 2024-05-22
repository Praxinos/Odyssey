#include "LayerStack/Layers/LayerImageRaster/OdysseyAnimationLayerImageRasterExport.h"
#include "LayerStack/Layers/LayerImageRaster/OdysseyAnimationLayerImageRaster.h"
#include "LayerStack/Cells/OdysseyAnimationCellsContainer.h"
#include "LayerStack/LightTable/OdysseyAnimationLightTable.h"
#include "OdysseyFile.h"

void
FOdysseyAnimationLayerImageRasterExport::WriteCellsContainer( UOdysseyAnimationLayerImageRaster* iAnimationLayerImageRaster
                                                       , FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::Animation::CHUNK_LAYERIMAGERASTER_CELLSCONTAINER
                            , Ar
                            , [iAnimationLayerImageRaster](FArchive &Ar) -> void
    {
        iAnimationLayerImageRaster->mCellsContainer->Serialize(Ar);
    });
}

void
FOdysseyAnimationLayerImageRasterExport::WriteLightTable( UOdysseyAnimationLayerImageRaster* iAnimationLayerImageRaster
                                                       , FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::Animation::CHUNK_LAYERIMAGERASTER_LIGHTTABLE
                            , Ar
                            , [iAnimationLayerImageRaster](FArchive &Ar) -> void
    {
        iAnimationLayerImageRaster->mLightTable->Serialize(Ar);
    });
}

void
FOdysseyAnimationLayerImageRasterExport::Write( UOdysseyAnimationLayerImageRaster* iAnimationLayerImageRaster
                                             , FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::Animation::CHUNK_LAYERIMAGERASTER
                            , Ar
                            , [iAnimationLayerImageRaster](FArchive &Ar) -> void
    {
        WriteCellsContainer( iAnimationLayerImageRaster, Ar );
        WriteLightTable( iAnimationLayerImageRaster, Ar );
    } );
}
