// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyAnimationCellImageRasterExport.h"

#if WITH_EDITOR
#include "OdysseyAnimationCellImageRaster.h"
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
#endif
