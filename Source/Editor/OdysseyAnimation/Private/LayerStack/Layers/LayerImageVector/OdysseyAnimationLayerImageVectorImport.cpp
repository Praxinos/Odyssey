#include "LayerStack/Layers/LayerImageVector/OdysseyAnimationLayerImageVectorImport.h"
#include "LayerStack/Layers/LayerImageVector/OdysseyAnimationLayerImageVector.h"
#include "LayerStack/Cells/OdysseyAnimationCellsContainer.h"
#include "LayerStack/LightTable/OdysseyAnimationLightTable.h"
#include "OdysseyFile.h"

bool
FOdysseyAnimationLayerImageVectorImport::Read( UOdysseyAnimationLayerImageVector* iAnimationLayerImageVector
                                            , FArchive &Ar )
{
    uint64 start = Ar.Tell();

    uint32 chunkID;
    uint64 chunkLen;
    uint64 chunkEnd;

    // Reads the first chunk (FOdysseyFile::Animation::CHUNK_LAYERIMAGEVECTOR)
    Ar << chunkID;
    Ar << chunkLen;

    chunkEnd = Ar.Tell() + chunkLen;

    switch( chunkID )
    {
        case FOdysseyFile::Animation::CHUNK_LAYERIMAGEVECTOR :
            //UE_LOG(LogTemp, Warning, TEXT("CHUNK_LAYERIMAGEVECTOR") );

            FOdysseyAnimationLayerImageVectorImport::Read( iAnimationLayerImageVector, Ar, chunkEnd );
        break;

        default:
            //No chunk found, seek back to the beginning and return false
            Ar.Seek( start );
            return false;
    }
    return true;
}

void
FOdysseyAnimationLayerImageVectorImport::Read( UOdysseyAnimationLayerImageVector* iAnimationLayerImageVector
                                            , FArchive &Ar
                                            , uint64 iChunkEnd )
{
    FOdysseyFile::ReadChunks( iChunkEnd
                              , Ar
                              , [iAnimationLayerImageVector](uint32 iChunkID, uint64 iChunkLen, FArchive &Ar) -> void
        {
            switch ( iChunkID )
            {
                case FOdysseyFile::Animation::CHUNK_LAYERIMAGEVECTOR_CELLSCONTAINER :
                {
                    iAnimationLayerImageVector->mCellsContainer->Serialize(Ar);
                }
                break;

                case FOdysseyFile::Animation::CHUNK_LAYERIMAGEVECTOR_LIGHTTABLE :
                {
                    iAnimationLayerImageVector->mLightTable->Serialize(Ar);
                }
                break;

                default:
				// Mandatory
                    Ar.Seek( Ar.Tell() + iChunkLen );
                break;
            }
        } );
}
