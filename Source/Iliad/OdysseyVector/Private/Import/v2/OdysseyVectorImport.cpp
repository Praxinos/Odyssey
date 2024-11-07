#include "Import/v2/OdysseyVectorImport.h"
#include "AssetRegistry/AssetRegistryModule.h"
// from module OdysseyFile
#include "OdysseyFile.h"

FOdysseyVectorImportV2::~FOdysseyVectorImportV2()
{
}

FOdysseyVectorImportV2::FOdysseyVectorImportV2()
    : mScene( nullptr )
{
}

void
FOdysseyVectorImportV2::Read( FOdysseyVectorGroupPaint* iScene, FArchive &Ar, uint64 iChunkEnd )
{
    // prevent reloading when the scene already exists.
    // This happens when undoing layer deletion for example.
    // Emptying the scene would create inconsistencies in the undo layer stack.
    if( iScene && ( iScene->GetChildrenList().size() == 0 ) )
    {
        mObjectArray.clear();
        mObjectArray.reserve( 400 );

        mScene = iScene;

        FOdysseyFile::ReadChunks( iChunkEnd
                                , Ar
                                , [this](uint32 iChunkID, uint64 iChunkLen, FArchive &Ar) -> void
            {
                switch ( iChunkID )
                {
                    case FOdysseyFile::VectorV2::CHUNK_DECLARE_OBJECTS :
                         // this call populates vectorObjectArray
                         FOdysseyVectorImportV2::ReadObjectsDeclare( Ar.Tell() + iChunkLen, Ar );
                    break;

                    case FOdysseyFile::VectorV2::CHUNK_DEFINE_OBJECTS :
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
