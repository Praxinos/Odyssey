#include "Export/v1/OdysseyVectorExport.h"
// from module OdysseyFile
#include "OdysseyFile.h"

void
FOdysseyVectorExportV1::Write( FOdysseyVectorGroupPaint* iScene, FArchive &Ar )
{
    // write the chunk even if iVEngine is nullptr to prevent "expected size mismatch" when unreal reads the data.
    // iVEngine is nullptr when unreal preloads the file.
    // The reading process can then just skip the chunk no matter its size.

    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV1::CHUNK_VECTOR_MAGIC_V1
                            , Ar
                            , [iScene](FArchive &Ar) -> void
    {
        if( iScene )
        {
            std::vector<FOdysseyVectorObject*> vectorObjectArray;

            FOdysseyVectorObject::TreeToArray ( iScene, vectorObjectArray );

            FOdysseyVectorExportV1::WriteDeclareObjects( vectorObjectArray, Ar );
            FOdysseyVectorExportV1::WriteDefineObjects( vectorObjectArray, Ar );
        }
    } );
}
