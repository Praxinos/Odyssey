// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Export/v2/OdysseyVectorExport.h"
// from module OdysseyFile
#include "OdysseyFile.h"
#include "OdysseyVectorGroup.h"

void
FOdysseyVectorExportV2::WriteGroupHUDColor( FOdysseyVectorGroup& iGroup, FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_GROUP_HUDCOLOR
                            , Ar
                            , [&iGroup](FArchive &Ar) -> void
    {
        FColor HUDColor = iGroup.GetHUDColor();
        uint8 R = HUDColor.R
            , G = HUDColor.G
            , B = HUDColor.G
            , A = HUDColor.A;

        Ar << R;
        Ar << G;
        Ar << B;
        Ar << A;
    } );
}

// Write chunks without encapsulation within the GroupPaint chunk header
void
FOdysseyVectorExportV2::WriteGroupChunks( FOdysseyVectorGroup& iGroup, FArchive &Ar )
{
    // inherited chunks
    WriteObjectChunks( iGroup, Ar );
    // own chunks
    WriteGroupHUDColor( iGroup, Ar );
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
