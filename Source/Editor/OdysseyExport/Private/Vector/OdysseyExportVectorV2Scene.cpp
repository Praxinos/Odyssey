#include "Vector/OdysseyExportVectorV2.h" // from module OdysseyExport

// Write chunks without encapsulation within the GroupPaint chunk header
void
FOdysseyExportVectorV2::WriteSceneChunks( FOdysseyVectorScene& iScene, FArchive &Ar )
{
    // inherited chunks
    WriteGroupPaintChunks( iScene, Ar );
    // own chunks
}

// Write chunks with encapsulation within the GroupPaint chunk header
void
FOdysseyExportVectorV2::WriteScene( FOdysseyVectorScene& iScene, FArchive &Ar )
{
    FOdysseyExport::WriteChunk( FOdysseyExportVectorV2::CHUNK_SCENE
                                    , Ar
                                    , [&iScene](FArchive &Ar) -> void
    {
        WriteSceneChunks( iScene, Ar );
    } );
}
