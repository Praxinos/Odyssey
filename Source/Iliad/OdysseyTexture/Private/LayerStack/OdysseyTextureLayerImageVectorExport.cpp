#include "LayerStack/OdysseyTextureLayerImageVectorExport.h"
#include "LayerStack/OdysseyTextureLayerImageVector.h"
#include "Export/v2/OdysseyVectorExport.h"
#include "OdysseyFile.h"

void
FOdysseyTextureLayerImageVectorExport::Write( UOdysseyTextureLayerImageVector* iTextureLayerImageVector
                                             , FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::Texture::CHUNK_TEXTURELAYERIMAGEVECTOR
                            , Ar
                            , [iTextureLayerImageVector](FArchive &Ar) -> void
    {
        FOdysseyVectorEngine* vectorEngine = iTextureLayerImageVector->GetEngine();
        // engine might be NULL because this function is sometimes called even before loading anything
        // so we have to check the validity of the pointer
        if( vectorEngine )
        {
            FOdysseyVectorExportV2::Write( vectorEngine->GetScene(), Ar );
        }
    } );
}
