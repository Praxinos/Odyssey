#include "Import/OdysseyVectorImport.h"
#include "Export/OdysseyVectorExport.h"

void FOdysseyVectorImport::ReadChunks( uint64 iChunkEnd, FArchive &Ar, std::function<void(uint32, uint64, FArchive&)> iCallback )
{
    UE_LOG( LogTemp, Warning, TEXT("ReadChunks") );

    while( Ar.Tell() != iChunkEnd )
    {
        uint64 currentAddress;
        uint64 chunkLen;
        uint32 chunkID;

        Ar << chunkID;
        Ar << chunkLen;

        currentAddress = Ar.Tell();

        if ( chunkLen )
        {
            iCallback( chunkID, chunkLen, Ar );
        }

        Ar.Seek( currentAddress + chunkLen );
    }
}

void
FOdysseyVectorImport::Read( UOdysseyVectorRoot& iScene, uint64 iChunkEnd, FArchive &Ar )
{
    FOdysseyVectorImport::ReadChunks( iChunkEnd
                                    , Ar
                                    , [&iScene](uint32 iChunkID, uint64 iChunkLen, FArchive &Ar) -> void
        {
            UE_LOG( LogTemp, Warning, TEXT("ReadChunks %X %d"), iChunkID, iChunkLen );

            switch ( iChunkID )
            {
                case FOdysseyVectorExport::EXPORT_OBJECTS_DECLARE :
                    UE_LOG( LogTemp, Warning, TEXT("EXPORT_OBJECTS_DECLARE") );
                break;

                case FOdysseyVectorExport::EXPORT_OBJECTS_DECLARE_OBJECT :
                {
                    uint32 objectType;

                    Ar << objectType;

                    UE_LOG( LogTemp, Warning, TEXT("EXPORT_OBJECTS_DECLARE_OBJECT %d"), objectType );
                }
                break;

                default :
                break;
            }    
        } );
}
