#include "Vector/OdysseyImportVectorV2.h"
#include "AssetRegistry/AssetRegistryModule.h"

FOdysseyImportVectorV2::~FOdysseyImportVectorV2()
{
}

FOdysseyImportVectorV2::FOdysseyImportVectorV2()
    : mScene( nullptr )
{
}

void
FOdysseyImportVectorV2::Read( FOdysseyVectorScene* iScene, FArchive &Ar, uint64 iChunkEnd )
{
    if( iScene )
    {
        mObjectArray.clear();
        mObjectArray.reserve( 400 );

        mScene = iScene;

        FOdysseyImport::ReadChunks( iChunkEnd
                                          , Ar
                                          , [this](uint32 iChunkID, uint64 iChunkLen, FArchive &Ar) -> void
            {
                switch ( iChunkID )
                {
                    case FOdysseyExportVectorV2::CHUNK_DECLARE_OBJECTS :
                         // this call populates vectorObjectArray
                         FOdysseyImportVectorV2::ReadObjectsDeclare( Ar.Tell() + iChunkLen, Ar );
                    break;

                    case FOdysseyExportVectorV2::CHUNK_DEFINE_OBJECTS :
                         FOdysseyImportVectorV2::ReadObjectsDefine( Ar.Tell() + iChunkLen, Ar );
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
