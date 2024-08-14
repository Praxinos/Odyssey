#include "LayerStack/Cells/CellImageRaster/OdysseyAnimationCellImageRasterExport.h"
#include "LayerStack/Cells/CellImageRaster/OdysseyAnimationCellImageRaster.h"
#include "OdysseyFile.h"
#include "OdysseyRasterBlock.h"

void
FOdysseyAnimationCellImageRasterExport::WriteRasterBlock( UOdysseyAnimationCellImageRaster* iAnimationCellImageRaster
                                                       , FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::Animation::CHUNK_CELLIMAGERASTER_RASTERBLOCK
                            , Ar
                            , [iAnimationCellImageRaster](FArchive &Ar) -> void
    {
        Ar << *iAnimationCellImageRaster->mRasterBlock;
    });
}

void
FOdysseyAnimationCellImageRasterExport::Write( UOdysseyAnimationCellImageRaster* iAnimationCellImageRaster
                                             , FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::Animation::CHUNK_CELLIMAGERASTER
                            , Ar
                            , [iAnimationCellImageRaster](FArchive &Ar) -> void
    {
        WriteRasterBlock( iAnimationCellImageRaster, Ar );
    } );
}
