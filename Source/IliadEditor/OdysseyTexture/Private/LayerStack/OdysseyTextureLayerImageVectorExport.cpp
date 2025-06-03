// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyTextureLayerImageVectorExport.h"
#include "OdysseyTextureLayerImageVector.h"
#include "Export/v2/OdysseyVectorExport.h"
#include "OdysseyFile.h"
#include "OdysseyVectorEngine.h"

void
FOdysseyTextureLayerImageVectorExport::Write( UOdysseyTextureLayerImageVector* iTextureLayerImageVector
                                             , FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::Texture::CHUNK_TEXTURELAYERIMAGEVECTOR
                            , Ar
                            , [iTextureLayerImageVector](FArchive &Ar) -> void
    {
        FOdysseyVectorCell* vectorCell = iTextureLayerImageVector->GetVectorCell().Get();
        // engine might be NULL because this function is sometimes called even before loading anything
        // so we have to check the validity of the pointer
        if( vectorCell )
        {
            FOdysseyVectorExportV2::Write( vectorCell->GetScene(), Ar );
        }
    } );
}
