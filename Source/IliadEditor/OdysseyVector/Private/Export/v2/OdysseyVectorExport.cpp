// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Export/v2/OdysseyVectorExport.h"
// from module OdysseyFile
#include "OdysseyFile.h"
#include "OdysseyVectorObject.h"
#include "OdysseyVectorGroupPaint.h"

void
FOdysseyVectorExportV2::Write( FOdysseyVectorGroupPaint* iScene, FArchive &Ar )
{
    // write the chunk even if iScene is nullptr to prevent "expected size mismatch" when unreal reads the data.
    // iScene is nullptr when unreal preloads the file.
    // The reading process can then just skip the chunk no matter its size.

    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_VECTOR_MAGIC_V2
                            , Ar
                            , [iScene](FArchive &Ar) -> void
    {
        if( iScene )
        {
            std::vector<FOdysseyVectorObject*> vectorObjectArray;

            FOdysseyVectorObject::TreeToArray ( iScene, vectorObjectArray );

            FOdysseyVectorExportV2::WriteDeclareObjects( vectorObjectArray, Ar );
            FOdysseyVectorExportV2::WriteDefineObjects( vectorObjectArray, Ar );
        }
    } );
}
