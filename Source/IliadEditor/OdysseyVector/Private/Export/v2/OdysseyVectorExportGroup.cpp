// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Export/v2/OdysseyVectorExport.h"
// from module OdysseyFile
#include "OdysseyFile.h"
#include "OdysseyVectorGroup.h"

// Write chunks without encapsulation within the GroupPaint chunk header
void
FOdysseyVectorExportV2::WriteGroupChunks( FOdysseyVectorGroup& iGroup, FArchive &Ar )
{
    // inherited chunks
    WriteObjectChunks( iGroup, Ar );
    // own chunks
    // ...Nothing to write.
}

// Write chunks with encapsulation within the GroupPaint chunk header
void
FOdysseyVectorExportV2::WriteGroup( FOdysseyVectorGroup& iGroup, FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_GROUP
                            , Ar
                            , [&iGroup](FArchive &Ar) -> void
    {
        WriteGroupChunks( iGroup, Ar );
    } );
}
