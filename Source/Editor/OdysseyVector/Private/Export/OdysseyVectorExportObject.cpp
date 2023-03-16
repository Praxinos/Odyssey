#include "Export/OdysseyVectorExport.h"

static void
WriteDeclareObjectEntry( FOdysseyVectorObject& iObject, FArchive &Ar )
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
FOdysseyVectorExport::WriteDeclareObjects( std::vector<FOdysseyVectorObject*>& vectorObjectArray, FArchive &Ar )
{
    FOdysseyVectorExport::WriteChunk( FOdysseyVectorExport::CHUNK_DECLARE_OBJECTS
                                    , Ar
                                    , [&vectorObjectArray](FArchive &Ar) -> void
    {
        for( uint32 i = 0; i < vectorObjectArray.size(); i++ )
        {
            FOdysseyVectorObject* vectorObject = vectorObjectArray[i];

            // Indexation used by other parts of the loading process
            vectorObject->SetID( i );

            WriteDeclareObjectEntry( *vectorObject, Ar );
        }
    } );
}

static void
WriteObjectTransformScaling( FOdysseyVectorObject& iObject, FArchive &Ar )
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
WriteObjectTransformRotation( FOdysseyVectorObject& iObject, FArchive &Ar )
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
WriteObjectTransformTranslation( FOdysseyVectorObject& iObject, FArchive &Ar )
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
WriteObjectTransform( FOdysseyVectorObject& iObject, FArchive &Ar )
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
WriteObjectParentID( FOdysseyVectorObject& iObject, FArchive &Ar )
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
WriteObjectID( FOdysseyVectorObject& iObject, FArchive &Ar )
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
WriteObjectForegroundColor( FOdysseyVectorObject& iObject, FArchive &Ar )
{
    FOdysseyVectorExport::WriteChunk( FOdysseyVectorExport::CHUNK_OBJECT_FOREGROUNDCOLOR
                                    , Ar
                                    , [&iObject](FArchive &Ar) -> void
    {
        FColor foreground = iObject.GetForegroundColor();

        Ar << foreground.R;
        Ar << foreground.G;
        Ar << foreground.B;
        Ar << foreground.A;
    } );
}

static void
WriteObjectBackgroundColor( FOdysseyVectorObject& iObject, FArchive &Ar )
{
    FOdysseyVectorExport::WriteChunk( FOdysseyVectorExport::CHUNK_OBJECT_BACKGROUNDCOLOR
                                    , Ar
                                    , [&iObject](FArchive &Ar) -> void
    {
        FColor background = iObject.GetBackgroundColor();

        Ar << background.R;
        Ar << background.G;
        Ar << background.B;
        Ar << background.A;
    } );
}

static void
WriteDefineObjectEntry( FOdysseyVectorObject& iObject, FArchive &Ar )
{
    FOdysseyVectorExport::WriteChunk( FOdysseyVectorExport::CHUNK_DEFINE_OBJECT_ENTRY
                                    , Ar
                                    , [&iObject](FArchive &Ar) -> void
    {
        WriteObjectID( iObject, Ar );
        WriteObjectParentID( iObject, Ar );
        WriteObjectTransform( iObject, Ar );
        WriteObjectForegroundColor( iObject, Ar );
        WriteObjectBackgroundColor( iObject, Ar );

        switch( iObject.GetType() )
        {
            case FOdysseyVectorObject::VECTORPATHCUBICTYPE:
            {
                FOdysseyVectorPathCubic* cubicPath = static_cast<FOdysseyVectorPathCubic*>(&iObject);

                FOdysseyVectorExport::WriteObjectPathCubic( *cubicPath, Ar );
            }
            break;

            case FOdysseyVectorObject::VECTORGROUPPAINTTYPE:
            {
                FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>(&iObject);

                FOdysseyVectorExport::WriteObjectGroupPaint( *paintGroup, Ar );
            }
            break;

            case FOdysseyVectorObject::VECTORELLIPSETYPE:
            {
                FOdysseyVectorEllipse* circle = static_cast<FOdysseyVectorEllipse*>(&iObject);

                FOdysseyVectorExport::WriteObjectEllipse( *circle, Ar );
            }
            break;

            default:
            break;
        }
    } );
}

void
FOdysseyVectorExport::WriteDefineObjects( std::vector<FOdysseyVectorObject*>& vectorObjectArray, FArchive &Ar )
{
    FOdysseyVectorExport::WriteChunk( FOdysseyVectorExport::CHUNK_DEFINE_OBJECTS
                                    , Ar
                                    , [&vectorObjectArray](FArchive &Ar) -> void
    {
        for( uint32 i = 0; i < vectorObjectArray.size(); i++ )
        {
            FOdysseyVectorObject* vectorObject = vectorObjectArray[i];

            WriteDefineObjectEntry( *vectorObject, Ar );
        }
    } );
}
