#include "Import/OdysseyVectorImport.h"
#include "OdysseyVectorPathCubic.h"

static UOdysseyVectorObject*
CreateObject( uint32 iObjectType )
{
    UOdysseyVectorObject* newObject = nullptr;

    switch ( iObjectType )
    {
        case UOdysseyVectorObject::VECTORROOTTYPE :
            // do nothing, the scene is already created by the vector layer
        break;

        case UOdysseyVectorObject::VECTORPATHCUBICTYPE :
            newObject = NewObject<UOdysseyVectorPathCubic>();
        break;

        default :
            newObject = NewObject<UOdysseyVectorObject>();
        break;
    }

    return newObject;
}

void
FOdysseyVectorImport::ReadObjectsDeclare( std::vector<UOdysseyVectorObject*>& vectorObjectArray, uint64 iChunkEnd, FArchive &Ar )
{
    FOdysseyVectorImport::ReadChunks( iChunkEnd
                                    , Ar
                                    , [&vectorObjectArray](uint32 iChunkID, uint64 iChunkLen, FArchive &Ar) -> void
        {
            switch ( iChunkID )
            {
                case FOdysseyVectorExport::CHUNK_DECLARE_OBJECT_ENTRY :
                {
                    UOdysseyVectorObject* newObject;
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

static void
ReadObjectsDefineObjectTransform( UOdysseyVectorObject& iObject, uint64 iChunkEnd, FArchive &Ar )
{
    FOdysseyVectorImport::ReadChunks( iChunkEnd
                                    , Ar
                                    , [&iObject](uint32 iChunkID, uint64 iChunkLen, FArchive &Ar) -> void
        {
            switch ( iChunkID )
            {
                case FOdysseyVectorExport::CHUNK_OBJECT_TRANSFORM_TRANSLATION:
                {
                    double translationX;
                    double translationY;

                    Ar << translationX;
                    Ar << translationY;

                    iObject.Translate( translationX, translationY );
                }
                break;

                case FOdysseyVectorExport::CHUNK_OBJECT_TRANSFORM_ROTATION:
                {
                    double rotation;

                    Ar << rotation;

                    iObject.Rotate( rotation );
                }
                break;

                case FOdysseyVectorExport::CHUNK_OBJECT_TRANSFORM_SCALING:
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
FOdysseyVectorImport::ReadObjectsDefine( std::vector<UOdysseyVectorObject*>& vectorObjectArray, uint64 iChunkEnd, FArchive &Ar )
{
    FOdysseyVectorImport::ReadChunks( iChunkEnd
                                    , Ar
                                    , [&vectorObjectArray](uint32 iChunkID, uint64 iChunkLen, FArchive &Ar) -> void
        {
            static uint32 objectID;

            switch( iChunkID )
            {
                case FOdysseyVectorExport::CHUNK_DEFINE_OBJECT_ENTRY :
                break;

                case FOdysseyVectorExport::CHUNK_OBJECT_ID:
                    Ar << objectID;
                break;

                case FOdysseyVectorExport::CHUNK_OBJECT_PARENTID:
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

                case FOdysseyVectorExport::CHUNK_OBJECT_TRANSFORM:
                    ReadObjectsDefineObjectTransform( *vectorObjectArray[objectID], Ar.Tell() + iChunkLen, Ar );
                break;

                case FOdysseyVectorExport::CHUNK_OBJECT_PATHCUBIC:
                {
                    UOdysseyVectorPathCubic* cubicPath = Cast<UOdysseyVectorPathCubic>( vectorObjectArray[objectID] );

                    FOdysseyVectorImport::ReadObjectPathCubic( *cubicPath, Ar.Tell() + iChunkLen, Ar );

                }
                break;

                default:
                // Mandatory
                    Ar.Seek( Ar.Tell() + iChunkLen );
                break;
            }    
        } );
}
