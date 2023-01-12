#include "Import/OdysseyVectorImport.h"

void FOdysseyVectorImport::ReadChunks( uint64 iChunkEnd, FArchive &Ar, std::function<void(uint32, uint64, FArchive&)> iCallback )
{
    UE_LOG( LogTemp, Warning, TEXT("ReadChunks") );

    while( Ar.Tell() != iChunkEnd )
    {
        uint64 chunkLen;
        uint32 chunkID;

        Ar << chunkID;
        Ar << chunkLen;

        UE_LOG( LogTemp, Warning, TEXT("Reading Chunk %X %d"), chunkID, chunkLen );

        /*if ( chunkLen )
        {*/
            iCallback( chunkID, chunkLen, Ar );
        /*}*/
    }
}

void
FOdysseyVectorImport::Read( FOdysseyVectorEngine* iVEngine, FArchive &Ar )
{
    uint32 chunkID;
    uint64 chunkLen;
    uint64 currentAddress;
    uint64 chunkEnd;


    // Reads the first chunk (CHUNK_VECTOR_MAGIC)
    Ar << chunkID;
    Ar << chunkLen;

    currentAddress = Ar.Tell();

    chunkEnd = currentAddress + chunkLen;

    UE_LOG(LogTemp,Warning,TEXT("chunkID %X %d %d"), chunkID, chunkLen, iVEngine );

    if( iVEngine )
    {
        std::vector<UOdysseyVectorObject*> vectorObjectArray;

        // first record must be the scene
        vectorObjectArray.push_back( iVEngine->GetScene() );

        FOdysseyVectorImport::ReadChunks( chunkEnd
                                        , Ar
                                        , [iVEngine,&vectorObjectArray](uint32 iChunkID, uint64 iChunkLen, FArchive &Ar) -> void
            {
                switch ( iChunkID )
                {
                    case FOdysseyVectorExport::CHUNK_DECLARE_OBJECTS :
                         // this call populates vectorObjectArray
                         FOdysseyVectorImport::ReadObjectsDeclare( vectorObjectArray, Ar.Tell() + iChunkLen, Ar );
                    break;

                    case FOdysseyVectorExport::CHUNK_DEFINE_OBJECTS :
                         FOdysseyVectorImport::ReadObjectsDefine( vectorObjectArray, Ar.Tell() + iChunkLen, Ar );
                    break;

                    default:
                    // Mandatory
                        Ar.Seek( Ar.Tell() + iChunkLen );
                    break;
                }
            } );
    }

    // Jump to the end of the junk, regardless of the fact that we've read nested chunks or not.
    // if we have read them, we'll just jump to the location where we already are.
    Ar.Seek( chunkEnd );
}
