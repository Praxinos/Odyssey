#include "Import/v1/OdysseyVectorImport.h"
#include "AssetRegistry/AssetRegistryModule.h"
// from module OdysseyFile
#include "OdysseyFile.h"

void
FOdysseyVectorImportV1::Read( FOdysseyVectorGroupPaint* iScene, FArchive &Ar, uint64 iChunkEnd )
{
    // prevent reloading when the scene already exists.
    // This happens when undoing layer deletion for example.
    // Emptying the scene would create inconsistencies in the undo layer stack.
    if( iScene && ( iScene->GetChildrenList().size() == 0 ) )
    {
        std::vector<FOdysseyVectorObject*> vectorObjectArray;

        // first record must be the scene
        vectorObjectArray.push_back( iScene );

        FOdysseyFile::ReadChunks( iChunkEnd
                                , Ar
                                , [&vectorObjectArray](uint32 iChunkID, uint64 iChunkLen, FArchive &Ar) -> void
            {
                switch ( iChunkID )
                {
                    case FOdysseyFile::VectorV1::CHUNK_DECLARE_OBJECTS :
                         // this call populates vectorObjectArray
                         FOdysseyVectorImportV1::ReadObjectsDeclare( vectorObjectArray, Ar.Tell() + iChunkLen, Ar );
                    break;

                    case FOdysseyFile::VectorV1::CHUNK_DEFINE_OBJECTS :
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
