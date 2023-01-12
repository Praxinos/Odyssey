#include "Export/OdysseyVectorExport.h"

static void
WriteDeclareObjectEntry( UOdysseyVectorObject& iObject, FArchive &Ar )
{
    FOdysseyVectorExport::WriteChunk( FOdysseyVectorExport::CHUNK_DECLARE_OBJECT_ENTRY
                                    , Ar
                                    , [&iObject](FArchive &Ar) -> void
    {
        uint32 objectType = iObject.GetType();

        Ar << objectType;
    } );
}

void
FOdysseyVectorExport::WriteDeclareObjects( std::vector<UOdysseyVectorObject*>& vectorObjectArray, FArchive &Ar )
{
    FOdysseyVectorExport::WriteChunk( FOdysseyVectorExport::CHUNK_DECLARE_OBJECTS
                                    , Ar
                                    , [&vectorObjectArray](FArchive &Ar) -> void
    {
        for( uint32 i = 0; i < vectorObjectArray.size(); i++ )
        {
            UOdysseyVectorObject* vectorObject = vectorObjectArray[i];

            // Indexation used by other parts of the loading process
            vectorObject->SetID( i );

            WriteDeclareObjectEntry( *vectorObject, Ar );
        }
    } );
}

static void
WriteObjectTransformScaling( UOdysseyVectorObject& iObject, FArchive &Ar )
{
    FOdysseyVectorExport::WriteChunk( FOdysseyVectorExport::CHUNK_OBJECT_TRANSFORM_SCALING
                                    , Ar
                                    , [&iObject](FArchive &Ar) -> void
    {
        double scalingX = iObject.GetScalingX(),
               scalingY = iObject.GetScalingY();

        Ar << scalingX;
        Ar << scalingY;
    } );
}

static void
WriteObjectTransformRotation( UOdysseyVectorObject& iObject, FArchive &Ar )
{
    FOdysseyVectorExport::WriteChunk( FOdysseyVectorExport::CHUNK_OBJECT_TRANSFORM_ROTATION
                                    , Ar
                                    , [&iObject](FArchive &Ar) -> void
    {
        double rotation = iObject.GetRotation();

        Ar << rotation;
    } );
}

static void
WriteObjectTransformTranslation( UOdysseyVectorObject& iObject, FArchive &Ar )
{
    FOdysseyVectorExport::WriteChunk( FOdysseyVectorExport::CHUNK_OBJECT_TRANSFORM_TRANSLATION
                                    , Ar
                                    , [&iObject](FArchive &Ar) -> void
    {
        double translationX = iObject.GetTranslationX(),
               translationY = iObject.GetTranslationY();

        Ar << translationX;
        Ar << translationY;
    } );
}

static void
WriteObjectTransform( UOdysseyVectorObject& iObject, FArchive &Ar )
{
    FOdysseyVectorExport::WriteChunk( FOdysseyVectorExport::CHUNK_OBJECT_TRANSFORM
                                    , Ar
                                    , [&iObject](FArchive &Ar) -> void
    {
        WriteObjectTransformTranslation( iObject, Ar );
        WriteObjectTransformRotation( iObject, Ar );
        WriteObjectTransformScaling( iObject, Ar );
    } );
}

static void
WriteObjectParentID( UOdysseyVectorObject& iObject, FArchive &Ar )
{
    FOdysseyVectorExport::WriteChunk( FOdysseyVectorExport::CHUNK_OBJECT_PARENTID
                                    , Ar
                                    , [&iObject](FArchive &Ar) -> void
    {
        uint32 parentID = iObject.GetParent() ? iObject.GetParent()->GetID() : 0;

        Ar << parentID;
    } );
}

static void
WriteObjectID( UOdysseyVectorObject& iObject, FArchive &Ar )
{
    FOdysseyVectorExport::WriteChunk( FOdysseyVectorExport::CHUNK_OBJECT_ID
                                    , Ar
                                    , [&iObject](FArchive &Ar) -> void
    {
        uint32 objectID = iObject.GetID();

        Ar << objectID;
    } );
}

static void
WriteDefineObjectEntry( UOdysseyVectorObject& iObject, FArchive &Ar )
{
    FOdysseyVectorExport::WriteChunk( FOdysseyVectorExport::CHUNK_DEFINE_OBJECT_ENTRY
                                    , Ar
                                    , [&iObject](FArchive &Ar) -> void
    {
        WriteObjectID( iObject, Ar );
        WriteObjectParentID( iObject, Ar );
        WriteObjectTransform( iObject, Ar );

        switch( iObject.GetType() )
        {
            case UOdysseyVectorObject::VECTORPATHCUBICTYPE:
            {
                UOdysseyVectorPathCubic* cubicPath = Cast<UOdysseyVectorPathCubic>(&iObject);

                FOdysseyVectorExport::WriteObjectPathCubic( *cubicPath, Ar );
            }
            break;

            default:
            break;
        }
    } );
}

void
FOdysseyVectorExport::WriteDefineObjects( std::vector<UOdysseyVectorObject*>& vectorObjectArray, FArchive &Ar )
{
    FOdysseyVectorExport::WriteChunk( FOdysseyVectorExport::CHUNK_DEFINE_OBJECTS
                                    , Ar
                                    , [&vectorObjectArray](FArchive &Ar) -> void
    {
        for( uint32 i = 0; i < vectorObjectArray.size(); i++ )
        {
            UOdysseyVectorObject* vectorObject = vectorObjectArray[i];

            WriteDefineObjectEntry( *vectorObject, Ar );
        }
    } );
}
