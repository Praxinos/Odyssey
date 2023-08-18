#include "Import/v2/OdysseyVectorImport.h"
#include "OdysseyVectorPath.h"

FOdysseyVectorObject*
FOdysseyVectorImportV2::CreateObject( uint32 iObjectType )
{
    FOdysseyVectorObject* newObject = nullptr;

    switch ( iObjectType )
    {
        case FOdysseyVectorObject::VECTORROOTTYPE :
            // do nothing, the scene is already created by the vector layer
        break;

        case FOdysseyVectorObject::VECTORPATHTYPE :
            newObject = new FOdysseyVectorPath( FString("Path"));
        break;

        case FOdysseyVectorObject::VECTORGROUPPAINTTYPE :
            newObject = new FOdysseyVectorGroupPaint( FString("PaintGroup") );
        break;

        default :
            newObject = new FOdysseyVectorObject( FString("Object") );
        break;
    }

    return newObject;
}

void
FOdysseyVectorImportV2::ReadObjectsDeclare( uint64 iChunkEnd, FArchive &Ar )
{
    FOdysseyVectorImportV2::ReadChunks( iChunkEnd
                                    , Ar
                                    , [this](uint32 iChunkID, uint64 iChunkLen, FArchive &Ar) -> void
        {
            switch ( iChunkID )
            {
                case FOdysseyVectorExportV2::CHUNK_DECLARE_OBJECT_ENTRY :
                {
                    FOdysseyVectorObject* newObject;
                    uint32 objectType;

                    Ar << objectType;

                    newObject = CreateObject( objectType );

                    if ( newObject )
                    {
                        newObject->SetID( mObjectArray.size() );

                        mObjectArray.push_back( newObject );
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
FOdysseyVectorImportV2::ReadObjectTransform( FOdysseyVectorObject& iObject, uint64 iChunkEnd, FArchive &Ar )
{
    FOdysseyVectorImportV2::ReadChunks( iChunkEnd
                                    , Ar
                                    , [&iObject](uint32 iChunkID, uint64 iChunkLen, FArchive &Ar) -> void
        {
            switch ( iChunkID )
            {
                case FOdysseyVectorExportV2::CHUNK_OBJECT_TRANSFORM_TRANSLATION:
                {
                    double translationX;
                    double translationY;

                    Ar << translationX;
                    Ar << translationY;

                    iObject.Translate( translationX, translationY );
                }
                break;

                case FOdysseyVectorExportV2::CHUNK_OBJECT_TRANSFORM_ROTATION:
                {
                    double rotation;

                    Ar << rotation;

                    iObject.Rotate( rotation );
                }
                break;

                case FOdysseyVectorExportV2::CHUNK_OBJECT_TRANSFORM_SCALING:
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
FOdysseyVectorImportV2::ReadObjectBucket( FOdysseyVectorBucket& iBucket
                                        , uint64 iChunkEnd
                                        , FArchive &Ar )
{
    FOdysseyVectorImportV2::ReadChunks( iChunkEnd
                                    , Ar
                                    , [this,&iBucket](uint32 iChunkID, uint64 iChunkLen, FArchive &Ar) -> void
        {
            switch ( iChunkID )
            {
                case FOdysseyVectorExportV2::CHUNK_BUCKET_ENTRY:
                {
                    FOdysseyVectorImportV2::ReadBucket( iBucket, Ar.Tell() + iChunkLen, Ar);
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
FOdysseyVectorImportV2::ReadObject( FOdysseyVectorObject& iObject, uint64 iChunkEnd, FArchive &Ar )
{
    FOdysseyVectorImportV2::ReadChunks( iChunkEnd
                                    , Ar
                                    , [this,&iObject](uint32 iChunkID, uint64 iChunkLen, FArchive &Ar) -> void
        {
            switch( iChunkID )
            {
                case FOdysseyVectorExportV2::CHUNK_OBJECT_PARENTID:
                {
                    uint32 parentID;

                    Ar << parentID;

                    // objectID = 0 if we are on the root node. Ignore it.
                    if( iObject.GetID() )
                    {
                        mObjectArray[parentID]->AppendChild( &iObject );
                    }
                }
                break;

                case FOdysseyVectorExportV2::CHUNK_OBJECT_TRANSFORM:
                    ReadObjectTransform( iObject, Ar.Tell() + iChunkLen, Ar );
                break;

                case FOdysseyVectorExportV2::CHUNK_OBJECT_FOREGROUNDBUCKET:
                {
                    FOdysseyVectorBucket& foregroundBucket = iObject.GetForegroundBucket();

                    ReadObjectBucket( foregroundBucket, Ar.Tell() + iChunkLen, Ar);
                }
                break;

                case FOdysseyVectorExportV2::CHUNK_OBJECT_BACKGROUNDBUCKET:
                {
                    FOdysseyVectorBucket& backgroundBucket = iObject.GetBackgroundBucket();

                    ReadObjectBucket( backgroundBucket, Ar.Tell() + iChunkLen, Ar);
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
FOdysseyVectorImportV2::ReadObjectsDefine( uint64 iChunkEnd, FArchive &Ar )
{
    FOdysseyVectorImportV2::ReadChunks( iChunkEnd
                                    , Ar
                                    , [this](uint32 iChunkID, uint64 iChunkLen, FArchive &Ar) -> void
        {
            static uint32 objectID;

            switch( iChunkID )
            {
                case FOdysseyVectorExportV2::CHUNK_DEFINE_OBJECT_ENTRY :
                {
                    FOdysseyVectorObject* vectorObject = nullptr;

                    Ar << objectID;

                    vectorObject = mObjectArray[objectID];

                    if( vectorObject->GetClass() == FOdysseyVectorPath::StaticClass() )
                    {
                        FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(vectorObject);

                        FOdysseyVectorImportV2::ReadPath( *path, Ar.Tell() + iChunkLen - sizeof(uint32), Ar );
                        // immediately update invalidated segments and updates the path's BBox
                        path->Update( 0 );
                    }
                    else
                    if( vectorObject->GetClass() == FOdysseyVectorGroupPaint::StaticClass() )
                    {
                        FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>(vectorObject);

                        FOdysseyVectorImportV2::ReadGroupPaint( *paintGroup, Ar.Tell() + iChunkLen - sizeof(uint32), Ar );

                        paintGroup->Invalidate();
                    }
                    else
                    {
                        FOdysseyVectorImportV2::ReadObject( *vectorObject, Ar.Tell() + iChunkLen - sizeof(uint32), Ar );
                    }
                }
                break;

                default:
                // Mandatory
                    Ar.Seek( Ar.Tell() + iChunkLen );
                break;
            }    
        } );
}
