#include "Vector/OdysseyExportVectorV1.h"

void
FOdysseyExportVectorV1::WriteDeclareObjectEntry( FOdysseyVectorObject& iObject, FArchive &Ar )
{
    FOdysseyExport::WriteChunk( FOdysseyExportVectorV1::CHUNK_DECLARE_OBJECT_ENTRY
                                    , Ar
                                    , [&iObject](FArchive &Ar) -> void
    {
        uint32 objectType = iObject.GetType();

        Ar << objectType;
    } );
}

void
FOdysseyExportVectorV1::WriteDeclareObjects( std::vector<FOdysseyVectorObject*>& vectorObjectArray, FArchive &Ar )
{
    FOdysseyExport::WriteChunk( FOdysseyExportVectorV1::CHUNK_DECLARE_OBJECTS
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

void
FOdysseyExportVectorV1::WriteObjectTransformScaling( FOdysseyVectorObject& iObject, FArchive &Ar )
{
    FOdysseyExport::WriteChunk( FOdysseyExportVectorV1::CHUNK_OBJECT_TRANSFORM_SCALING
                                    , Ar
                                    , [&iObject](FArchive &Ar) -> void
    {
        double scalingX = iObject.GetScalingX(),
               scalingY = iObject.GetScalingY();

        Ar << scalingX;
        Ar << scalingY;
    } );
}

void
FOdysseyExportVectorV1::WriteObjectTransformRotation( FOdysseyVectorObject& iObject, FArchive &Ar )
{
    FOdysseyExport::WriteChunk( FOdysseyExportVectorV1::CHUNK_OBJECT_TRANSFORM_ROTATION
                                    , Ar
                                    , [&iObject](FArchive &Ar) -> void
    {
        double rotation = iObject.GetRotation();

        Ar << rotation;
    } );
}

void
FOdysseyExportVectorV1::WriteObjectTransformTranslation( FOdysseyVectorObject& iObject, FArchive &Ar )
{
    FOdysseyExport::WriteChunk( FOdysseyExportVectorV1::CHUNK_OBJECT_TRANSFORM_TRANSLATION
                                    , Ar
                                    , [&iObject](FArchive &Ar) -> void
    {
        double translationX = iObject.GetTranslationX(),
               translationY = iObject.GetTranslationY();

        Ar << translationX;
        Ar << translationY;
    } );
}

void
FOdysseyExportVectorV1::WriteObjectTransform( FOdysseyVectorObject& iObject, FArchive &Ar )
{
    FOdysseyExport::WriteChunk( FOdysseyExportVectorV1::CHUNK_OBJECT_TRANSFORM
                                    , Ar
                                    , [&iObject](FArchive &Ar) -> void
    {
        WriteObjectTransformTranslation( iObject, Ar );
        WriteObjectTransformRotation( iObject, Ar );
        WriteObjectTransformScaling( iObject, Ar );
    } );
}

void
FOdysseyExportVectorV1::WriteObjectParentID( FOdysseyVectorObject& iObject, FArchive &Ar )
{
    FOdysseyExport::WriteChunk( FOdysseyExportVectorV1::CHUNK_OBJECT_PARENTID
                                    , Ar
                                    , [&iObject](FArchive &Ar) -> void
    {
        uint32 parentID = iObject.GetParent() ? iObject.GetParent()->GetID() : 0;

        Ar << parentID;
    } );
}

void
FOdysseyExportVectorV1::WriteObjectID( FOdysseyVectorObject& iObject, FArchive &Ar )
{
    FOdysseyExport::WriteChunk( FOdysseyExportVectorV1::CHUNK_OBJECT_ID
                                    , Ar
                                    , [&iObject](FArchive &Ar) -> void
    {
        uint32 objectID = iObject.GetID();

        Ar << objectID;
    } );
}

void
FOdysseyExportVectorV1::WriteObjectForegroundBucket( FOdysseyVectorObject& iObject, FArchive &Ar )
{
    FOdysseyExport::WriteChunk( FOdysseyExportVectorV1::CHUNK_OBJECT_FOREGROUNDBUCKET
                                    , Ar
                                    , [&iObject](FArchive &Ar) -> void
    {
        FOdysseyVectorBucket& foregroundBucket = iObject.GetForegroundBucket();

        FOdysseyExportVectorV1::WriteBucket( foregroundBucket, Ar );
    } );
}

void
FOdysseyExportVectorV1::WriteObjectBackgroundBucket( FOdysseyVectorObject& iObject, FArchive &Ar )
{
    FOdysseyExport::WriteChunk( FOdysseyExportVectorV1::CHUNK_OBJECT_BACKGROUNDBUCKET
                                    , Ar
                                    , [&iObject](FArchive &Ar) -> void
    {
        FOdysseyVectorBucket& backgroundBucket = iObject.GetBackgroundBucket();

        FOdysseyExportVectorV1::WriteBucket( backgroundBucket, Ar );
    } );
}

void
FOdysseyExportVectorV1::WriteObjectForegroundColor( FOdysseyVectorObject& iObject, FArchive &Ar )
{
    FOdysseyExport::WriteChunk( FOdysseyExportVectorV1::CHUNK_OBJECT_FOREGROUNDCOLOR
                                    , Ar
                                    , [&iObject](FArchive &Ar) -> void
    {
        FColor foreground = iObject.GetForegroundBucket().GetSolidColor();

        Ar << foreground.R;
        Ar << foreground.G;
        Ar << foreground.B;
        Ar << foreground.A;
    } );
}

void
FOdysseyExportVectorV1::WriteObjectBackgroundColor( FOdysseyVectorObject& iObject, FArchive &Ar )
{
    FOdysseyExport::WriteChunk( FOdysseyExportVectorV1::CHUNK_OBJECT_BACKGROUNDCOLOR
                                    , Ar
                                    , [&iObject](FArchive &Ar) -> void
    {
        FColor background = iObject.GetBackgroundBucket().GetSolidColor();

        Ar << background.R;
        Ar << background.G;
        Ar << background.B;
        Ar << background.A;
    } );
}

void
FOdysseyExportVectorV1::WriteDefineObjectEntry( FOdysseyVectorObject& iObject, FArchive &Ar )
{
    FOdysseyExport::WriteChunk( FOdysseyExportVectorV1::CHUNK_DEFINE_OBJECT_ENTRY
                                    , Ar
                                    , [&iObject](FArchive &Ar) -> void
    {
        WriteObjectID( iObject, Ar );
        WriteObjectParentID( iObject, Ar );
        WriteObjectTransform( iObject, Ar );
        //WriteObjectForegroundColor( iObject, Ar ); // deprecated
        //WriteObjectBackgroundColor( iObject, Ar ); // deprecated
        WriteObjectForegroundBucket( iObject, Ar );
        WriteObjectBackgroundBucket( iObject, Ar );

        switch( iObject.GetType() )
        {
            case FOdysseyVectorObject::VECTORPATHTYPE:
            {
                FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(&iObject);

                FOdysseyExportVectorV1::WritePath( *path, Ar );
            }
            break;

            case FOdysseyVectorObject::VECTORGROUPPAINTTYPE:
            {
                FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>(&iObject);

                FOdysseyExportVectorV1::WriteGroupPaint( *paintGroup, Ar );
            }
            break;

            default:
            break;
        }
    } );
}

void
FOdysseyExportVectorV1::WriteDefineObjects( std::vector<FOdysseyVectorObject*>& vectorObjectArray, FArchive &Ar )
{
    FOdysseyExport::WriteChunk( FOdysseyExportVectorV1::CHUNK_DEFINE_OBJECTS
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
