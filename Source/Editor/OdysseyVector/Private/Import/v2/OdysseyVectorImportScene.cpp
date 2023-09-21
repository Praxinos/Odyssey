#include "Import/v2/OdysseyVectorImport.h"
// from module OdysseyFile
#include "OdysseyFile.h"

void
FOdysseyVectorImportV2::ReadScene( FOdysseyVectorScene& iScene, uint64 iChunkEnd, FArchive &Ar )
{
    FOdysseyFile::ReadChunks( iChunkEnd
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
