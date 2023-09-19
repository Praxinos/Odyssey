#include "Export/v2/OdysseyVectorExport.h"
// from module OdysseyFile
#include "OdysseyFile.h"

// Write chunks without encapsulation within the GroupPaint chunk header
void
FOdysseyVectorExportV2::WriteSceneChunks( FOdysseyVectorScene& iScene, FArchive &Ar )
{
    // inherited chunks
    WriteGroupPaintChunks( iScene, Ar );
    // own chunks
}

// Write chunks with encapsulation within the GroupPaint chunk header
void
FOdysseyVectorExportV2::WriteScene( FOdysseyVectorScene& iScene, FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_SCENE
                            , Ar
                            , [&iScene](FArchive &Ar) -> void
    {
        WriteSceneChunks( iScene, Ar );
    } );
}
