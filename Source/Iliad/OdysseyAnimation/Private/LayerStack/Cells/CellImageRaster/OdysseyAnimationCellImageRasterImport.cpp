// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyAnimationCellImageRasterImport.h"

#if WITH_EDITOR
#include "OdysseyAnimationCellImageRaster.h"
#include "OdysseyFile.h"
#include "OdysseyRasterBlock.h"

bool
FOdysseyAnimationCellImageRasterImport::Read( UOdysseyAnimationCellImageRaster* iAnimationCellImageRaster
                                            , FArchive &Ar )
{
    if (Ar.AtEnd() || Ar.IsCriticalError())
        return true;

    uint64 start = Ar.Tell();

    uint32 chunkID;
    uint64 chunkLen;
    uint64 chunkEnd;

    // Reads the first chunk (FOdysseyFile::Animation::CHUNK_CELLIMAGERASTER)
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
        case FOdysseyFile::Animation::CHUNK_CELLIMAGERASTER :
            //UE_LOG(LogTemp, Warning, TEXT("CHUNK_CELLIMAGERASTER") );

            FOdysseyAnimationCellImageRasterImport::Read( iAnimationCellImageRaster, Ar, chunkEnd );
        break;

        default:
            //No chunk found, seek back to the beginning and return false

            Ar.Seek( start );

            return false;
    }
    return true;
}

void
FOdysseyAnimationCellImageRasterImport::Read( UOdysseyAnimationCellImageRaster* iAnimationCellImageRaster
                                            , FArchive &Ar
                                            , uint64 iChunkEnd )
{
    FOdysseyFile::ReadChunks( iChunkEnd
                              , Ar
                              , [iAnimationCellImageRaster](uint32 iChunkID, uint64 iChunkLen, FArchive &Ar) -> void
        {
            switch ( iChunkID )
            {
                case FOdysseyFile::Animation::CHUNK_CELLIMAGERASTER_RASTERBLOCK :
                {
                    iAnimationCellImageRaster->mRasterBlock = MakeShared<FOdysseyRasterBlock>(iAnimationCellImageRaster);
                    Ar << *iAnimationCellImageRaster->mRasterBlock;
                }
                break;

                default:
                // Mandatory
                    Ar.Seek( Ar.Tell() + iChunkLen );
                break;
            }
        } );
}
#endif
