// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyTextureLayerImageVectorImport.h"
#include "OdysseyTextureLayerImageVector.h"
#include "Import/v2/OdysseyVectorImport.h"
#include "OdysseyFile.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorCell.h"

bool
FOdysseyTextureLayerImageVectorImport::Read( UOdysseyTextureLayerImageVector* iTextureLayerImageVector
                                            , FArchive &Ar )
{
    if (Ar.AtEnd() || Ar.IsCriticalError())
        return true;

    uint64 start = Ar.Tell();

    uint32 chunkID;
    uint64 chunkLen;
    uint64 chunkEnd;

    // Reads the first chunk (FOdysseyFile::Texture::CHUNK_TEXTURELAYERIMAGEVECTOR)
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
        case FOdysseyFile::Texture::CHUNK_TEXTURELAYERIMAGEVECTOR :
            //UE_LOG(LogTemp, Warning, TEXT("CHUNK_TEXTURELAYERIMAGEVECTOR") );

            FOdysseyTextureLayerImageVectorImport::Read( iTextureLayerImageVector, Ar, chunkEnd );
        break;

        default:
            //No chunk found, seek back to the beginning and return false
            Ar.Seek( start );
            return false;
    }
    return true;
}

void
FOdysseyTextureLayerImageVectorImport::Read( UOdysseyTextureLayerImageVector* iTextureLayerImageVector
                                            , FArchive &Ar
                                            , uint64 iChunkEnd )
{
    FOdysseyFile::ReadChunks( iChunkEnd
                              , Ar
                              , [iTextureLayerImageVector](uint32 iChunkID, uint64 iChunkLen, FArchive &Ar) -> void
        {
            switch ( iChunkID )
            {
                case FOdysseyFile::VectorV2::CHUNK_VECTOR_MAGIC_V2:
                {
                    FOdysseyVectorCell* vectorCell = iTextureLayerImageVector->GetVectorCell().Get();
                    FOdysseyVectorImportV2* importerV2 = iTextureLayerImageVector->GetImporterV2();

                    //UE_LOG(LogTemp, Warning, TEXT("CHUNK_VECTOR_MAGIC_V2") );

                    importerV2->Read( vectorCell->GetScene(), Ar, Ar.Tell() + iChunkLen );
                }
                break;

                default:
                // Mandatory
                    Ar.Seek( Ar.Tell() + iChunkLen );
                break;
            }
        } );
}
