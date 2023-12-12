#include "Import/v1/OdysseyVectorImport.h"
#include "OdysseyVectorPath.h"
// from module OdysseyFile
#include "OdysseyFile.h"

FOdysseyVectorObject*
FOdysseyVectorImportV1::CreateObject( uint32 iObjectType )
{
    FOdysseyVectorObject* newObject = nullptr;

    switch ( iObjectType )
    {
        case FOdysseyFile::VectorV1::ObjectType::ROOT :
            // do nothing, the scene is already created by the vector layer
        break;

        case FOdysseyFile::VectorV1::ObjectType::PATH :
            newObject = new FOdysseyVectorPath( FString("Path"));
        break;

        case FOdysseyFile::VectorV1::ObjectType::GROUPPAINT :
            newObject = new FOdysseyVectorGroupPaint( FString("PaintGroup") );
        break;

        default :
            newObject = new FOdysseyVectorObject( FString("Object") );
        break;
    }

    return newObject;
}

void
FOdysseyVectorImportV1::ReadObjectsDeclare( std::vector<FOdysseyVectorObject*>& vectorObjectArray, uint64 iChunkEnd, FArchive &Ar )
{
    FOdysseyFile::ReadChunks( iChunkEnd
                            , Ar
                            , [&vectorObjectArray](uint32 iChunkID, uint64 iChunkLen, FArchive &Ar) -> void
        {
            switch ( iChunkID )
            {
                case FOdysseyFile::VectorV1::CHUNK_DECLARE_OBJECT_ENTRY :
                {
                    FOdysseyVectorObject* newObject;
                    uint32 objectType;

                    Ar << objectType;

                    newObject = CreateObject( objectType );

                    if ( newObject )
                    {
                        vectorObjectArray.push_back( newObject );
                    }
                }
                break;

                default :
                    // Mandatory
                    Ar.Seek( Ar.Tell() + iChunkLen );
                break;
            }    
        } );
}

void
FOdysseyVectorImportV1::ReadObjectsDefineObjectTransform( FOdysseyVectorObject& iObject, uint64 iChunkEnd, FArchive &Ar )
{
    FOdysseyFile::ReadChunks( iChunkEnd
                            , Ar
                            , [&iObject](uint32 iChunkID, uint64 iChunkLen, FArchive &Ar) -> void
        {
            switch ( iChunkID )
            {
                case FOdysseyFile::VectorV1::CHUNK_OBJECT_TRANSFORM_TRANSLATION:
                {
                    double translationX;
                    double translationY;

                    Ar << translationX;
                    Ar << translationY;

                    iObject.Translate( translationX, translationY );
                }
                break;

                case FOdysseyFile::VectorV1::CHUNK_OBJECT_TRANSFORM_ROTATION:
                {
                    double rotation;

                    Ar << rotation;

                    iObject.Rotate( rotation );
                }
                break;

                case FOdysseyFile::VectorV1::CHUNK_OBJECT_TRANSFORM_SCALING:
                {
                    double scalingX;
                    double scalingY;

                    Ar << scalingX;
                    Ar << scalingY;

                    iObject.Scale( scalingX, scalingY );
                }
                break;

                default:
                // Mandatory
                    Ar.Seek( Ar.Tell() + iChunkLen );
                break;
            }    
        } );
}

void
FOdysseyVectorImportV1::ReadObjectsDefineObjectBucket( FOdysseyVectorBucket& iBucket
                                                     , uint64 iChunkEnd
                                                     , FArchive &Ar )
{
    FOdysseyFile::ReadChunks( iChunkEnd
                            , Ar
                            , [&iBucket](uint32 iChunkID, uint64 iChunkLen, FArchive &Ar) -> void
        {
            switch ( iChunkID )
            {
                case FOdysseyFile::VectorV1::CHUNK_BUCKET_ENTRY:
                {
                    FOdysseyVectorImportV1::ReadBucket( iBucket, Ar.Tell() + iChunkLen, Ar);
                }
                break;

                default:
                // Mandatory
                    Ar.Seek( Ar.Tell() + iChunkLen );
                break;
            }    
        });
}

void
FOdysseyVectorImportV1::ReadObjectsDefine( std::vector<FOdysseyVectorObject*>& vectorObjectArray, uint64 iChunkEnd, FArchive &Ar )
{
    FOdysseyFile::ReadChunks( iChunkEnd
                            , Ar
                            , [&vectorObjectArray](uint32 iChunkID, uint64 iChunkLen, FArchive &Ar) -> void
        {
            static uint32 objectID;

            switch( iChunkID )
            {
                case FOdysseyFile::VectorV1::CHUNK_DEFINE_OBJECT_ENTRY :
                break;

                case FOdysseyFile::VectorV1::CHUNK_OBJECT_ID:
                    Ar << objectID;
                break;

                case FOdysseyFile::VectorV1::CHUNK_OBJECT_PARENTID:
                {
                    uint32 parentID;

                    Ar << parentID;

                    // objectID = 0 if we are on the root node. Ignore it.
                    if( objectID )
                    {
                        vectorObjectArray[parentID]->AppendChild( vectorObjectArray[objectID] );
                    }
                }
                break;

                case FOdysseyFile::VectorV1::CHUNK_OBJECT_TRANSFORM:
                    ReadObjectsDefineObjectTransform( *vectorObjectArray[objectID], Ar.Tell() + iChunkLen, Ar );
                break;

                case FOdysseyFile::VectorV1::CHUNK_OBJECT_FOREGROUNDBUCKET:
                {
                    FOdysseyVectorBucket& foregroundBucket = vectorObjectArray[objectID]->GetForegroundBucket();

                    ReadObjectsDefineObjectBucket( foregroundBucket, Ar.Tell() + iChunkLen, Ar);
                }
                break;

                case FOdysseyFile::VectorV1::CHUNK_OBJECT_BACKGROUNDBUCKET:
                {
                    FOdysseyVectorBucket& backgroundBucket = vectorObjectArray[objectID]->GetBackgroundBucket();

                    ReadObjectsDefineObjectBucket( backgroundBucket, Ar.Tell() + iChunkLen, Ar);
                }
                break;

                case FOdysseyFile::VectorV1::CHUNK_OBJECT_FOREGROUNDCOLOR:
                {
                    uint8 R, G, B, A;

                    Ar << R;
                    Ar << G;
                    Ar << B;
                    Ar << A;

                    vectorObjectArray[objectID]->GetForegroundBucket().SetSolidColor( R, G, B, A );
                }
                break;

                case FOdysseyFile::VectorV1::CHUNK_OBJECT_BACKGROUNDCOLOR:
                {
                    uint8 R, G, B, A;

                    Ar << R;
                    Ar << G;
                    Ar << B;
                    Ar << A;

                    vectorObjectArray[objectID]->GetBackgroundBucket().SetSolidColor( R, G, B, A );
                }
                break;

                case FOdysseyFile::VectorV1::CHUNK_OBJECT_PATH:
                {
                    FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>( vectorObjectArray[objectID] );

                    FOdysseyVectorImportV1::ReadPath( *path, Ar.Tell() + iChunkLen, Ar );
                    // immediately update invalidated segments and updates the path's BBox
                    path->Update( 0 );
                }
                break;

                case FOdysseyFile::VectorV1::CHUNK_OBJECT_GROUPPAINT:
                {
                    FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>( vectorObjectArray[objectID] );

                    FOdysseyVectorImportV1::ReadGroupPaint( *paintGroup, Ar.Tell() + iChunkLen, Ar );

                    paintGroup->Invalidate();
                }
                break;

                default:
                // Mandatory
                    Ar.Seek( Ar.Tell() + iChunkLen );
                break;
            }    
        } );
}
