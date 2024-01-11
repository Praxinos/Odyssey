#include "LayerStack/Layers/LayerImageRaster/OdysseyAnimationLayerImageRasterImport.h"
#include "LayerStack/Layers/LayerImageRaster/OdysseyAnimationLayerImageRaster.h"
#include "OdysseyFile.h"

bool
FOdysseyAnimationLayerImageRasterImport::Read( UOdysseyAnimationLayerImageRaster* iAnimationLayerImageRaster
                                            , FArchive &Ar )
{
    uint64 start = Ar.Tell();

    uint32 chunkID;
    uint64 chunkLen;
    uint64 chunkEnd;

    // Reads the first chunk (FOdysseyFile::Animation::CHUNK_LAYERIMAGERASTER)
    Ar << chunkID;
    Ar << chunkLen;

    chunkEnd = Ar.Tell() + chunkLen;

    switch( chunkID )
    {
        case FOdysseyFile::Animation::CHUNK_LAYERIMAGERASTER :
            UE_LOG(LogTemp, Warning, TEXT("CHUNK_LAYERIMAGERASTER") );

            FOdysseyAnimationLayerImageRasterImport::Read( iAnimationLayerImageRaster, Ar, chunkEnd );
        break;

        default:
            //No chunk found, seek back to the beginning and return false
            Ar.Seek( start );
            return false;
    }
    return true;
}

void
FOdysseyAnimationLayerImageRasterImport::Read( UOdysseyAnimationLayerImageRaster* iAnimationLayerImageRaster
                                            , FArchive &Ar
                                            , uint64 iChunkEnd )
{
    FOdysseyFile::ReadChunks( iChunkEnd
                              , Ar
                              , [iAnimationLayerImageRaster](uint32 iChunkID, uint64 iChunkLen, FArchive &Ar) -> void
        {
            switch ( iChunkID )
            {
                case FOdysseyFile::Animation::CHUNK_LAYERIMAGERASTER_CELLSCONTAINER :
                {
                    iAnimationLayerImageRaster->mCellsContainer->Serialize(Ar);
                }
                break;

                case FOdysseyFile::Animation::CHUNK_LAYERIMAGERASTER_LIGHTTABLE :
                {
                    iAnimationLayerImageRaster->mLightTable->Serialize(Ar);
                }
                break;

                default:
				// Mandatory
                    Ar.Seek( Ar.Tell() + iChunkLen );
                break;
            }
        } );
}
