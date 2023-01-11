#include "Export/OdysseyVectorExport.h"

static void
WriteObjectsDeclareObject( UOdysseyVectorObject& iObject, FArchive &Ar )
{
    FOdysseyVectorExport::WriteChunk( FOdysseyVectorExport::CHUNK_OBJECTS_DECLARE_OBJECT
                                    , Ar
                                    , [&iObject](FArchive &Ar) -> void
    {
        uint32 objectType = iObject.GetType();

        Ar << objectType;
    } );
}

void
FOdysseyVectorExport::WriteObjectsDeclare( std::vector<UOdysseyVectorObject*>& vectorObjectArray, FArchive &Ar )
{
    FOdysseyVectorExport::WriteChunk( FOdysseyVectorExport::CHUNK_OBJECTS_DECLARE
                                    , Ar
                                    , [&vectorObjectArray](FArchive &Ar) -> void
    {
        for( uint32 i = 0; i < vectorObjectArray.size(); i++ )
        {
            UOdysseyVectorObject* vectorObject = vectorObjectArray[i];

            // Indexation used by other parts of the loading process
            vectorObject->SetID( i );

            WriteObjectsDeclareObject( *vectorObject, Ar );
        }
    } );
}

static void
WriteObjectsDefineObjectTransformScaling( UOdysseyVectorObject& iObject, FArchive &Ar )
{
    double scalingX = iObject.GetScalingX(),
           scalingY = iObject.GetScalingY();

    Ar << scalingX;
    Ar << scalingY;
}

static void
WriteObjectsDefineObjectTransformRotation( UOdysseyVectorObject& iObject, FArchive &Ar )
{
    double rotation = iObject.GetRotation();

    Ar << rotation;
}

static void
WriteObjectsDefineObjectTransformTranslation( UOdysseyVectorObject& iObject, FArchive &Ar )
{
    double translationX = iObject.GetTranslationX(),
           translationY = iObject.GetTranslationY();

    Ar << translationX;
    Ar << translationY;
}

static void
WriteObjectsDefineObjectTransform( UOdysseyVectorObject& iObject, FArchive &Ar )
{
    FOdysseyVectorExport::WriteChunk( FOdysseyVectorExport::CHUNK_OBJECTS_DEFINE_OBJECT_TRANSFORM
                                    , Ar
                                    , [&iObject](FArchive &Ar) -> void
    {
        WriteObjectsDefineObjectTransformTranslation( iObject, Ar );
        WriteObjectsDefineObjectTransformRotation( iObject, Ar );
        WriteObjectsDefineObjectTransformScaling( iObject, Ar );
    } );
}

static void
WriteObjectsDefineObjectParentID( UOdysseyVectorObject& iObject, FArchive &Ar )
{
    FOdysseyVectorExport::WriteChunk( FOdysseyVectorExport::CHUNK_OBJECTS_DEFINE_OBJECT_PARENTID
                                    , Ar
                                    , [&iObject](FArchive &Ar) -> void
    {
        uint32 parentID = iObject.GetParent() ? iObject.GetParent()->GetID() : 0;

        Ar << parentID;
    } );
}

static void
WriteObjectsDefineObjectID( UOdysseyVectorObject& iObject, FArchive &Ar )
{
    FOdysseyVectorExport::WriteChunk( FOdysseyVectorExport::CHUNK_OBJECTS_DEFINE_OBJECT_ID
                                    , Ar
                                    , [&iObject](FArchive &Ar) -> void
    {
        uint32 objectID = iObject.GetID();

        Ar << objectID;
    } );
}

static void
WriteObjectsDefineObject( UOdysseyVectorObject& iObject, FArchive &Ar )
{
    FOdysseyVectorExport::WriteChunk( FOdysseyVectorExport::CHUNK_OBJECTS_DEFINE_OBJECT
                                    , Ar
                                    , [&iObject](FArchive &Ar) -> void
    {
        WriteObjectsDefineObjectID( iObject, Ar );
        WriteObjectsDefineObjectParentID( iObject, Ar );
        WriteObjectsDefineObjectTransform( iObject, Ar );
    } );
}

void
FOdysseyVectorExport::WriteObjectsDefine( std::vector<UOdysseyVectorObject*>& vectorObjectArray, FArchive &Ar )
{
    FOdysseyVectorExport::WriteChunk( FOdysseyVectorExport::CHUNK_OBJECTS_DEFINE
                                    , Ar
                                    , [&vectorObjectArray](FArchive &Ar) -> void
    {
        for( uint32 i = 0; i < vectorObjectArray.size(); i++ )
        {
            UOdysseyVectorObject* vectorObject = vectorObjectArray[i];

            WriteObjectsDefineObject( *vectorObject, Ar );
        }
    } );
}
