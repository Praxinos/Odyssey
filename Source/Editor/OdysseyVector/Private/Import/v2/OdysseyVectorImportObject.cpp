#include "Import/v2/OdysseyVectorImport.h"
#include "OdysseyVectorPath.h"
// from module OdysseyFile
#include "OdysseyFile.h"
#include "OdysseyVectorPath.h"
#include "OdysseyVectorGroup.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorTagInbetweener.h"
#include "OdysseyVectorEngine.h"

FOdysseyVectorObject*
FOdysseyVectorImportV2::CreateObject( uint32 iObjectType )
{
    FOdysseyVectorObject* newObject = nullptr;

    switch ( iObjectType )
    {
        // Legacy, when FOdysseyVectorScene was a thing.
        case FOdysseyFile::VectorV2::ObjectType::ROOT :
            // don't create anything, the scene is already created by the vector layer
            newObject = mScene;
        break;

        case FOdysseyFile::VectorV2::ObjectType::PATH :
            newObject = new FOdysseyVectorPath( FString("Path"));
        break;

        case FOdysseyFile::VectorV2::ObjectType::GROUP :
            newObject = new FOdysseyVectorGroup( FString("Group"));
        break;

        case FOdysseyFile::VectorV2::ObjectType::GROUPPAINT :
            // the root paintgroup
            if( mObjectArray.size() == 0 )
            {
                newObject = mScene;
            }
            else
            {
                newObject = new FOdysseyVectorGroupPaint( FString("PaintGroup") );
            }
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
    FOdysseyFile::ReadChunks( iChunkEnd
                            , Ar
                            , [this](uint32 iChunkID, uint64 iChunkLen, FArchive &Ar) -> void
        {
            switch ( iChunkID )
            {
                case FOdysseyFile::VectorV2::CHUNK_DECLARE_OBJECT_ENTRY :
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
    FOdysseyFile::ReadChunks( iChunkEnd
                            , Ar
                            , [&iObject](uint32 iChunkID, uint64 iChunkLen, FArchive &Ar) -> void
        {
            switch ( iChunkID )
            {
                case FOdysseyFile::VectorV2::CHUNK_OBJECT_TRANSFORM_TRANSLATION:
                {
                    double translationX;
                    double translationY;

                    Ar << translationX;
                    Ar << translationY;

                    iObject.Translate( translationX, translationY );
                }
                break;

                case FOdysseyFile::VectorV2::CHUNK_OBJECT_TRANSFORM_ROTATION:
                {
                    double rotation;

                    Ar << rotation;

                    iObject.Rotate( rotation );
                }
                break;

                case FOdysseyFile::VectorV2::CHUNK_OBJECT_TRANSFORM_SCALING:
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
    FOdysseyFile::ReadChunks( iChunkEnd
                            , Ar
                            , [this,&iBucket](uint32 iChunkID, uint64 iChunkLen, FArchive &Ar) -> void
        {
            switch ( iChunkID )
            {
                case FOdysseyFile::VectorV2::CHUNK_BUCKET_ENTRY:
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

// function to use when the chunk header is already read.
void
FOdysseyVectorImportV2::ParseObjectChunks( FOdysseyVectorObject& iObject
                                         , uint32 iChunkID
                                         , uint64 iChunkLen
                                         , FArchive &Ar )
{
    switch( iChunkID )
    {
        case FOdysseyFile::VectorV2::CHUNK_OBJECT_PARENTID:
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

        case FOdysseyFile::VectorV2::CHUNK_OBJECT_NAME:
        {
            FString name;

            Ar << name;

            iObject.SetName( name );
        }
        break;

        case FOdysseyFile::VectorV2::CHUNK_OBJECT_TRANSFORM:  // container
            ReadObjectTransform( iObject, Ar.Tell() + iChunkLen, Ar );
        break;

        case FOdysseyFile::VectorV2::CHUNK_OBJECT_EXPANSION:
        {
            uint32 expanded;

            Ar << expanded;

            iObject.SetExpanded( expanded ? true : false );
        }
        break;

        case FOdysseyFile::VectorV2::CHUNK_OBJECT_OPACITY:
        {
            double opacity;

            Ar << opacity;

            iObject.SetOpacity( opacity ? 1.0f : 0.0f );
        }
        break;

        case FOdysseyFile::VectorV2::CHUNK_OBJECT_FOREGROUNDBUCKET: // container
        {
            FOdysseyVectorBucket& foregroundBucket = iObject.GetForegroundBucket();

            ReadObjectBucket( foregroundBucket, Ar.Tell() + iChunkLen, Ar);
        }
        break;

        case FOdysseyFile::VectorV2::CHUNK_OBJECT_BACKGROUNDBUCKET: // container
        {
            FOdysseyVectorBucket& backgroundBucket = iObject.GetBackgroundBucket();

            ReadObjectBucket( backgroundBucket, Ar.Tell() + iChunkLen, Ar);
        }
        break;

        case FOdysseyFile::VectorV2::CHUNK_OBJECT_TAGS: // container
        break;

        case FOdysseyFile::VectorV2::CHUNK_TAGINBETWEENER:
        {
            FOdysseyVectorEngine* vectorEngine = iObject.GetEngine();

            FOdysseyVectorTagInbetweener* inbetweenerTag = new FOdysseyVectorTagInbetweener( vectorEngine->GetSharedEnv()
                                                                                           , &iObject
                                                                                           , 0
                                                                                           , 0
                                                                                           , 0 );

            iObject.AddTag( inbetweenerTag );

            ReadTagInbetweener( *inbetweenerTag, Ar.Tell() + iChunkLen, Ar );
        }
        break;

        default:
        // Mandatory
            Ar.Seek( Ar.Tell() + iChunkLen );
        break;
    }    
}

void
FOdysseyVectorImportV2::ReadObject( FOdysseyVectorObject& iObject, uint64 iChunkEnd, FArchive &Ar )
{
    FOdysseyFile::ReadChunks( iChunkEnd
                            , Ar
                            , [this,&iObject](uint32 iChunkID, uint64 iChunkLen, FArchive &Ar) -> void
        {
            ParseObjectChunks( iObject, iChunkID, iChunkLen, Ar );
        } );
}

void
FOdysseyVectorImportV2::ReadObjectsDefine( uint64 iChunkEnd, FArchive &Ar )
{
    FOdysseyFile::ReadChunks( iChunkEnd
                            , Ar
                            , [this](uint32 iChunkID, uint64 iChunkLen, FArchive &Ar) -> void
        {
            static FOdysseyVectorObject* vectorObject;

            switch( iChunkID )
            {
                case FOdysseyFile::VectorV2::CHUNK_DEFINE_OBJECT_ENTRY :
                break;

                case FOdysseyFile::VectorV2::CHUNK_DEFINE_OBJECT_ID :
                {
                    uint32 objectID;

                    Ar << objectID;

                    vectorObject = mObjectArray[objectID];
                }
                break;

                case FOdysseyFile::VectorV2::CHUNK_GROUP :
                {
                    FOdysseyVectorGroup* group = static_cast<FOdysseyVectorGroup*>(vectorObject);

                    FOdysseyVectorImportV2::ReadGroup( *group, Ar.Tell() + iChunkLen, Ar );
 
                    group->Invalidate();
                }
                break;

                case FOdysseyFile::VectorV2::CHUNK_PATH :
                {
                    FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(vectorObject);

                    FOdysseyVectorImportV2::ReadPath( *path, Ar.Tell() + iChunkLen, Ar );
                    // immediately update invalidated segments and updates the path's BBox
                    path->Update( 0 );
                }
                break;

                case FOdysseyFile::VectorV2::CHUNK_SCENE: // legacy
                case FOdysseyFile::VectorV2::CHUNK_GROUPPAINT :
                {
                    FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>(vectorObject);

                    FOdysseyVectorImportV2::ReadGroupPaint( *paintGroup, Ar.Tell() + iChunkLen, Ar );

                    paintGroup->Invalidate();
                }
                break;

                case FOdysseyFile::VectorV2::CHUNK_OBJECT :
                    FOdysseyVectorImportV2::ReadObject( *vectorObject, Ar.Tell() + iChunkLen, Ar );
                break;

                default:
                // Mandatory
                    Ar.Seek( Ar.Tell() + iChunkLen );
                break;
            }    
        } );
}
