// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyTextureLayerImageRasterImport.h"
#include "OdysseyTextureLayerImageRaster.h"
#include "OdysseyFile.h"

bool
FOdysseyTextureLayerImageRasterImport::Read( UOdysseyTextureLayerImageRaster* iTextureLayerImageRaster
                                            , FArchive &Ar )
{
    if (Ar.IsCriticalError())
        return true;

    uint64 start = Ar.Tell();
    uint64 end = Ar.TotalSize();
    if (end - start < 4 + 8) //check if we can read chunkID and chunkLen
        return false;

    uint32 chunkID;
    uint64 chunkLen;
    uint64 chunkEnd;

    // Reads the first chunk (FOdysseyFile::Texture::CHUNK_TEXTURELAYERIMAGERASTER)
    Ar << chunkID;
    Ar << chunkLen;

    //PATCH:
    // If we read out of the Linker bounds (which is not the same as the Archive bounds)
    // The archive generates a CriticalError
    // We don't have access to the linker bounds, so we can only rely on the critical error state
    // We then need to seek to start again, like nothing ever happened
    // And clear the Critical Error state
    if (Ar.IsCriticalError())
    {
        Ar.Seek( start );
        Ar.ClearCriticalError();
        return true;
    }

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
                    iTextureLayerImageRaster->RasterBlock = MakeShared<FOdysseyRasterBlock>(iTextureLayerImageRaster);
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
