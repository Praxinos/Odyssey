// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

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
        if (iAnimationCellImageRaster->mRasterBlock)
            WriteRasterBlock( iAnimationCellImageRaster, Ar );
    } );
}
