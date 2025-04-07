// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Import/v2/OdysseyVectorImport.h"
// from module OdysseyFile
#include "OdysseyFile.h"
#include "OdysseyVectorGroup.h"

void
FOdysseyVectorImportV2::ParseGroupChunks( FOdysseyVectorGroup& iGroup
                                        , uint32 iChunkID
                                        , uint64 iChunkLen
                                        , FArchive &Ar )
{
    switch( iChunkID )
    {
        case FOdysseyFile::VectorV2::CHUNK_GROUP_HUDCOLOR :
            uint8 R, G, B, A;

            Ar << R;
            Ar << G;
            Ar << B;
            Ar << A;

            iGroup.SetHUDColor( FColor( R, G, B, A ) );
        break;

        default:
            FOdysseyVectorImportV2::ParseObjectChunks( iGroup, iChunkID, iChunkLen, Ar );
        break;
    }
}

void
FOdysseyVectorImportV2::ReadGroup( FOdysseyVectorGroup& iGroup, uint64 iChunkEnd, FArchive &Ar )
{
    FOdysseyFile::ReadChunks( iChunkEnd
                            , Ar
                            , [this,&iGroup](uint32 iChunkID, uint64 iChunkLen, FArchive &Ar) -> void
        {
            ParseGroupChunks( iGroup, iChunkID, iChunkLen, Ar );
        } );
}
