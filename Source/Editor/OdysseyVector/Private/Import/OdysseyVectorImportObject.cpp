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
                case FOdysseyVectorExport::CHUNK_OBJECTS_DECLARE_OBJECT :
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

void
FOdysseyVectorImport::ReadObjectsDefine( std::vector<UOdysseyVectorObject*>& vectorObjectArray, uint64 iChunkEnd, FArchive &Ar )
{
    FOdysseyVectorImport::ReadChunks( iChunkEnd
                                    , Ar
                                    , [&vectorObjectArray](uint32 iChunkID, uint64 iChunkLen, FArchive &Ar) -> void
        {
            static uint32 objectID;

            switch ( iChunkID )
            {
                case FOdysseyVectorExport::CHUNK_OBJECTS_DEFINE_OBJECT :
                break;

                case FOdysseyVectorExport::CHUNK_OBJECTS_DEFINE_OBJECT_ID:
                    Ar << objectID;
                break;

                case FOdysseyVectorExport::CHUNK_OBJECTS_DEFINE_OBJECT_PARENTID:
                {
                    uint32 parentID;

                    Ar << parentID;

                    vectorObjectArray[parentID]->AppendChild( vectorObjectArray[objectID] );
                }
                break;
/*
                case FOdysseyVectorExport::CHUNK_OBJECTS_DEFINE_OBJECT_TRANSFORM:
                break;

                case FOdysseyVectorExport::CHUNK_OBJECTS_DEFINE_OBJECT_TRANSFORM_TRANSLATION:
                break;

                case FOdysseyVectorExport::CHUNK_OBJECTS_DEFINE_OBJECT_TRANSFORM_ROTATION:
                break;

                case FOdysseyVectorExport::CHUNK_OBJECTS_DEFINE_OBJECT_TRANSFORM_SCALING:
                break;
*/
                default:
                // Mandatory
                    Ar.Seek( Ar.Tell() + iChunkLen );
                break;
            }    
        } );
}
