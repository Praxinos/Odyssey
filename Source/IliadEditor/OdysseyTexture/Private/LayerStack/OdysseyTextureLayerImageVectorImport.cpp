// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/OdysseyTextureLayerImageVectorImport.h"
#include "LayerStack/OdysseyTextureLayerImageVector.h"
#include "Import/v2/OdysseyVectorImport.h"
#include "OdysseyFile.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorCell.h"

bool
FOdysseyTextureLayerImageVectorImport::Read( UOdysseyTextureLayerImageVector* iTextureLayerImageVector
                                            , FArchive &Ar )
{
    uint64 start = Ar.Tell();

    uint32 chunkID;
    uint64 chunkLen;
    uint64 chunkEnd;

    // Reads the first chunk (FOdysseyFile::Texture::CHUNK_TEXTURELAYERIMAGEVECTOR)
    Ar << chunkID;
    Ar << chunkLen;

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
                    FOdysseyVectorCell* vectorCell = iTextureLayerImageVector->GetVectorCell();
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
