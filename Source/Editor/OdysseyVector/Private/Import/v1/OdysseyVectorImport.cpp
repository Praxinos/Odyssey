#include "Import/v1/OdysseyVectorImport.h"
#include "AssetRegistry/AssetRegistryModule.h"

void
FOdysseyVectorImportV1::ReadChunks( uint64 iChunkEnd, FArchive &Ar, std::function<void(uint32, uint64, FArchive&)> iCallback )
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
FOdysseyVectorImportV1::Read( FOdysseyVectorScene* iScene, FArchive &Ar, uint64 iChunkEnd )
{
    if( iScene )
    {
        std::vector<FOdysseyVectorObject*> vectorObjectArray;

        // first record must be the scene
        vectorObjectArray.push_back( iScene );

        FOdysseyVectorImportV1::ReadChunks( iChunkEnd
                                          , Ar
                                          , [&vectorObjectArray](uint32 iChunkID, uint64 iChunkLen, FArchive &Ar) -> void
            {
                switch ( iChunkID )
                {
                    case FOdysseyVectorExportV1::CHUNK_DECLARE_OBJECTS :
                         // this call populates vectorObjectArray
                         FOdysseyVectorImportV1::ReadObjectsDeclare( vectorObjectArray, Ar.Tell() + iChunkLen, Ar );
                    break;

                    case FOdysseyVectorExportV1::CHUNK_DEFINE_OBJECTS :
                         FOdysseyVectorImportV1::ReadObjectsDefine( vectorObjectArray, Ar.Tell() + iChunkLen, Ar );
                    break;

                    default:
                    // Mandatory
                        Ar.Seek( Ar.Tell() + iChunkLen );
                    break;
                }
            } );

        iScene->UpdateMatrix();
        iScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );
    }

    // Jump to the end of the junk, regardless of the fact that we've read nested chunks or not.
    // if we have read them, we'll just jump to the location where we already are.
    Ar.Seek( iChunkEnd );
}
