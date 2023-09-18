#include "Vector/OdysseyExportVectorV1.h"

void
FOdysseyExportVectorV1::Write( FOdysseyVectorScene* iScene, FArchive &Ar )
{
    // write the chunk even if iVEngine is nullptr to prevent "expected size mismatch" when unreal reads the data.
    // iVEngine is nullptr when unreal preloads the file.
    // The reading process can then just skip the chunk no matter its size.

    FOdysseyExport::WriteChunk( FOdysseyExportVectorV1::CHUNK_VECTOR_MAGIC_V1
                              , Ar
                              , [iScene](FArchive &Ar) -> void
    {
        if( iScene )
        {
            std::vector<FOdysseyVectorObject*> vectorObjectArray;

            FOdysseyVectorObject::TreeToArray ( iScene, vectorObjectArray );

            FOdysseyExportVectorV1::WriteDeclareObjects( vectorObjectArray, Ar );
            FOdysseyExportVectorV1::WriteDefineObjects( vectorObjectArray, Ar );
        }
    } );
}
