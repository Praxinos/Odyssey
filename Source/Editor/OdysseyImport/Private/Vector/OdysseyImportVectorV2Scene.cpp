#include "Vector/OdysseyImportVectorV2.h"

void
FOdysseyImportVectorV2::ReadScene( FOdysseyVectorScene& iScene, uint64 iChunkEnd, FArchive &Ar )
{
    FOdysseyImport::ReadChunks( iChunkEnd
                                    , Ar
                                    , [this,&iScene](uint32 iChunkID, uint64 iChunkLen, FArchive &Ar) -> void
        {
            switch( iChunkID )
            {
                case 0 : // dummy
                break;

                default:
                    FOdysseyImportVectorV2::ParseGroupPaintChunks( iScene, iChunkID, iChunkLen, Ar );
                break;
            }    
        } );
}
