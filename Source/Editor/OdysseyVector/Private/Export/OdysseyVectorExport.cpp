#include "Export/OdysseyVectorExport.h"

void
FOdysseyVectorExport::WriteChunk( uint32 iChunkID, FArchive &Ar, std::function<void(FArchive &Ar)> iCallback )
{
    uint64 chunkLen = 0;
    uint64 chunkLenAddress;
    uint64 currentAddress;

    Ar << iChunkID;

    chunkLenAddress = Ar.Tell();
    // write dummy value, we will set it at the end
    Ar << chunkLen;
    // write data
    iCallback( Ar );

    currentAddress = Ar.Tell();
    // compute the size of this chunk
    chunkLen = currentAddress - chunkLenAddress - sizeof( uint64 );

    Ar.Seek( chunkLenAddress );

    Ar << chunkLen;

    Ar.Seek( currentAddress );
}

static void
WriteObjectsDeclare( UOdysseyVectorRoot* iScene, FArchive &Ar )
{
    FOdysseyVectorExport::WriteChunk( FOdysseyVectorExport::EXPORT_OBJECTS_DECLARE
                                    , Ar
                                    , [iScene](FArchive &Ar) -> void
    {
        std::vector<UOdysseyVectorObject*> vectorObjectArray;
        uint32 vectorObjectID = 0;
        uint32 ObjectCount = UOdysseyVectorObject::TreeToArray ( iScene
                                                                , vectorObjectID
                                                                , vectorObjectArray );

        for( uint32 i = 0; i < ObjectCount; i++ )
        {
            UOdysseyVectorObject* vectorObject = vectorObjectArray[i];

            FOdysseyVectorExport::WriteObjectsDeclareObject( *vectorObject, Ar );
        }
    } );
}

void
FOdysseyVectorExport::Write( UOdysseyVectorRoot& iScene, FArchive &Ar )
{
    WriteObjectsDeclare( &iScene, Ar );
}
