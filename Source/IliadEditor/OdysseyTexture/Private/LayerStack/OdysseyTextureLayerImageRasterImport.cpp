// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/OdysseyTextureLayerImageRasterImport.h"
#include "LayerStack/OdysseyTextureLayerImageRaster.h"
#include "OdysseyFile.h"

bool
FOdysseyTextureLayerImageRasterImport::Read( UOdysseyTextureLayerImageRaster* iTextureLayerImageRaster
                                            , FArchive &Ar )
{
    uint64 start = Ar.Tell();

    uint32 chunkID;
    uint64 chunkLen;
    uint64 chunkEnd;

    // Reads the first chunk (FOdysseyFile::Texture::CHUNK_TEXTURELAYERIMAGERASTER)
    Ar << chunkID;
    Ar << chunkLen;

    chunkEnd = Ar.Tell() + chunkLen;

    switch( chunkID )
    {
        case FOdysseyFile::Texture::CHUNK_TEXTURELAYERIMAGERASTER :
            //UE_LOG(LogTemp, Warning, TEXT("CHUNK_TEXTURELAYERIMAGERASTER") );

            FOdysseyTextureLayerImageRasterImport::Read( iTextureLayerImageRaster, Ar, chunkEnd );
        break;

        default:
            //No chunk found, seek back to the beginning and return false
            Ar.Seek( start );
            return false;
    }
    return true;
}

void
FOdysseyTextureLayerImageRasterImport::Read( UOdysseyTextureLayerImageRaster* iTextureLayerImageRaster
                                            , FArchive &Ar
                                            , uint64 iChunkEnd )
{
    FOdysseyFile::ReadChunks( iChunkEnd
                              , Ar
                              , [iTextureLayerImageRaster](uint32 iChunkID, uint64 iChunkLen, FArchive &Ar) -> void
        {
            switch ( iChunkID )
            {
                case FOdysseyFile::Texture::CHUNK_TEXTURELAYERIMAGERASTER_RASTERBLOCK :
                {
                    Ar << *iTextureLayerImageRaster->RasterBlock;
                }
                break;

                default:
                // Mandatory
                    Ar.Seek( Ar.Tell() + iChunkLen );
                break;
            }
        } );
}
