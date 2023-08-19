#include "Import/v2/OdysseyVectorImport.h"
#include "AssetRegistry/AssetRegistryModule.h"

FOdysseyVectorImportV2::~FOdysseyVectorImportV2()
{
}

FOdysseyVectorImportV2::FOdysseyVectorImportV2()
    : mScene( nullptr )
{
}

void
FOdysseyVectorImportV2::ReadChunks( uint64 iChunkEnd, FArchive &Ar, std::function<void(uint32, uint64, FArchive&)> iCallback )
{
    UE_LOG( LogTemp, Warning, TEXT("ReadChunks") );

    while( Ar.Tell() != iChunkEnd )
    {
        uint32 chunkID;
        uint64 chunkLen;
        uint64 chunkAddress = Ar.Tell();

        Ar << chunkID;
        Ar << chunkLen;

        UE_LOG( LogTemp, Warning, TEXT("Reading Chunk %X %d at %X"), chunkID, chunkLen, chunkAddress );

        /*if ( chunkLen )
        {*/
            iCallback( chunkID, chunkLen, Ar );
        /*}*/
    }
}

void
FOdysseyVectorImportV2::Read( FOdysseyVectorScene* iScene, FArchive &Ar, uint64 iChunkEnd )
{
    if( iScene )
    {
        mObjectArray.clear();
        mObjectArray.reserve( 400 );

        mScene = iScene;

        FOdysseyVectorImportV2::ReadChunks( iChunkEnd
                                          , Ar
                                          , [this](uint32 iChunkID, uint64 iChunkLen, FArchive &Ar) -> void
            {
                switch ( iChunkID )
                {
                    case FOdysseyVectorExportV2::CHUNK_DECLARE_OBJECTS :
                         // this call populates vectorObjectArray
                         FOdysseyVectorImportV2::ReadObjectsDeclare( Ar.Tell() + iChunkLen, Ar );
                    break;

                    case FOdysseyVectorExportV2::CHUNK_DEFINE_OBJECTS :
                         FOdysseyVectorImportV2::ReadObjectsDefine( Ar.Tell() + iChunkLen, Ar );
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
