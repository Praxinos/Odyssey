#include "Import/v2/OdysseyVectorImport.h"
// from module OdysseyFile
#include "OdysseyFile.h"

void
FOdysseyVectorImportV2::ParseGroupChunks( FOdysseyVectorGroup& iGroup
                                        , uint32 iChunkID
                                        , uint64 iChunkLen
                                        , FArchive &Ar )
{
/* skeleton if more cases must be handled
    switch( iChunkID )
    {
        default:
            FOdysseyVectorImportV2::ParseObjectChunks( iGroup, iChunkID, iChunkLen, Ar );
        break;
    }    
*/
    FOdysseyVectorImportV2::ParseObjectChunks( iGroup, iChunkID, iChunkLen, Ar );
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
