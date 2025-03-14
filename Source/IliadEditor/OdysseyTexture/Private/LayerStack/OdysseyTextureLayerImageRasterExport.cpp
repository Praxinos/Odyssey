// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/OdysseyTextureLayerImageRasterExport.h"
#include "LayerStack/OdysseyTextureLayerImageRaster.h"
#include "OdysseyFile.h"

void
FOdysseyTextureLayerImageRasterExport::WriteRasterBlock( UOdysseyTextureLayerImageRaster* iTextureLayerImageRaster
                                                       , FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::Texture::CHUNK_TEXTURELAYERIMAGERASTER_RASTERBLOCK
                            , Ar
                            , [iTextureLayerImageRaster](FArchive &Ar) -> void
    {
        Ar << *iTextureLayerImageRaster->RasterBlock;
    });
}

void
FOdysseyTextureLayerImageRasterExport::Write( UOdysseyTextureLayerImageRaster* iTextureLayerImageRaster
                                             , FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::Texture::CHUNK_TEXTURELAYERIMAGERASTER
                            , Ar
                            , [iTextureLayerImageRaster](FArchive &Ar) -> void
    {
        WriteRasterBlock( iTextureLayerImageRaster, Ar );
    } );
}
