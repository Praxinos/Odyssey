#include "Import/v2/OdysseyVectorImport.h"


void
FOdysseyVectorImportV2::ReadScene( FOdysseyVectorScene& iScene, uint64 iChunkEnd, FArchive &Ar )
{
    FOdysseyVectorImportV2::ReadChunks( iChunkEnd
                                    , Ar
                                    , [this,&iScene](uint32 iChunkID, uint64 iChunkLen, FArchive &Ar) -> void
        {
            switch( iChunkID )
            {
                case 0 : // dummy
                break;

                default:
                    FOdysseyVectorImportV2::ParseGroupPaintChunks( iScene, iChunkID, iChunkLen, Ar );
                break;
            }    
        } );
}
