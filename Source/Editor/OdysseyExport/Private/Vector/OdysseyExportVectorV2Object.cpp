#include "Vector/OdysseyExportVectorV2.h" // from module OdysseyExport

void
FOdysseyExportVectorV2::WriteDeclareObjectEntry( FOdysseyVectorObject& iObject, FArchive &Ar )
{
    FOdysseyExport::WriteChunk( FOdysseyExportVectorV2::CHUNK_DECLARE_OBJECT_ENTRY
                                    , Ar
                                    , [&iObject](FArchive &Ar) -> void
    {
        uint32 objectType = iObject.GetType();

        Ar << objectType;
    } );
}

void
FOdysseyExportVectorV2::WriteDeclareObjects( std::vector<FOdysseyVectorObject*>& vectorObjectArray, FArchive &Ar )
{
    FOdysseyExport::WriteChunk( FOdysseyExportVectorV2::CHUNK_DECLARE_OBJECTS
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
FOdysseyExportVectorV2::WriteObjectTransformScaling( FOdysseyVectorObject& iObject, FArchive &Ar )
{
    FOdysseyExport::WriteChunk( FOdysseyExportVectorV2::CHUNK_OBJECT_TRANSFORM_SCALING
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
FOdysseyExportVectorV2::WriteObjectTransformRotation( FOdysseyVectorObject& iObject, FArchive &Ar )
{
    FOdysseyExport::WriteChunk( FOdysseyExportVectorV2::CHUNK_OBJECT_TRANSFORM_ROTATION
                                    , Ar
                                    , [&iObject](FArchive &Ar) -> void
    {
        double rotation = iObject.GetRotation();

        Ar << rotation;
    } );
}

void
FOdysseyExportVectorV2::WriteObjectTransformTranslation( FOdysseyVectorObject& iObject, FArchive &Ar )
{
    FOdysseyExport::WriteChunk( FOdysseyExportVectorV2::CHUNK_OBJECT_TRANSFORM_TRANSLATION
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
FOdysseyExportVectorV2::WriteObjectTransform( FOdysseyVectorObject& iObject, FArchive &Ar )
{
    FOdysseyExport::WriteChunk( FOdysseyExportVectorV2::CHUNK_OBJECT_TRANSFORM
                                    , Ar
                                    , [&iObject](FArchive &Ar) -> void
    {
        WriteObjectTransformTranslation( iObject, Ar );
        WriteObjectTransformRotation( iObject, Ar );
        WriteObjectTransformScaling( iObject, Ar );
    } );
}

void
FOdysseyExportVectorV2::WriteObjectParentID( FOdysseyVectorObject& iObject, FArchive &Ar )
{
    FOdysseyExport::WriteChunk( FOdysseyExportVectorV2::CHUNK_OBJECT_PARENTID
                                    , Ar
                                    , [&iObject](FArchive &Ar) -> void
    {
        uint32 parentID = iObject.GetParent() ? iObject.GetParent()->GetID() : 0;

        Ar << parentID;
    } );
}

void
FOdysseyExportVectorV2::WriteObjectForegroundBucket( FOdysseyVectorObject& iObject, FArchive &Ar )
{
    FOdysseyExport::WriteChunk( FOdysseyExportVectorV2::CHUNK_OBJECT_FOREGROUNDBUCKET
                                    , Ar
                                    , [&iObject](FArchive &Ar) -> void
    {
        FOdysseyVectorBucket& foregroundBucket = iObject.GetForegroundBucket();

        FOdysseyExportVectorV2::WriteBucket( foregroundBucket, Ar );
    } );
}

void
FOdysseyExportVectorV2::WriteObjectBackgroundBucket( FOdysseyVectorObject& iObject, FArchive &Ar )
{
    FOdysseyExport::WriteChunk( FOdysseyExportVectorV2::CHUNK_OBJECT_BACKGROUNDBUCKET
                                    , Ar
                                    , [&iObject](FArchive &Ar) -> void
    {
        FOdysseyVectorBucket& backgroundBucket = iObject.GetBackgroundBucket();

        FOdysseyExportVectorV2::WriteBucket( backgroundBucket, Ar );
    } );
}

void
FOdysseyExportVectorV2::WriteObjectForegroundColor( FOdysseyVectorObject& iObject, FArchive &Ar )
{
    FOdysseyExport::WriteChunk( FOdysseyExportVectorV2::CHUNK_OBJECT_FOREGROUNDCOLOR
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
FOdysseyExportVectorV2::WriteObjectBackgroundColor( FOdysseyVectorObject& iObject, FArchive &Ar )
{
    FOdysseyExport::WriteChunk( FOdysseyExportVectorV2::CHUNK_OBJECT_BACKGROUNDCOLOR
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
FOdysseyExportVectorV2::WriteObjectChunks( FOdysseyVectorObject& iObject, FArchive &Ar )
{
    WriteObjectParentID( iObject, Ar );
    WriteObjectTransform( iObject, Ar );
    WriteObjectForegroundBucket( iObject, Ar );
    WriteObjectBackgroundBucket( iObject, Ar );
}

void
FOdysseyExportVectorV2::WriteObject( FOdysseyVectorObject& iObject, FArchive &Ar )
{
    FOdysseyExport::WriteChunk( FOdysseyExportVectorV2::CHUNK_OBJECT
                                      , Ar
                                      , [&iObject](FArchive &Ar) -> void
    {
        WriteObjectChunks( iObject, Ar );
    } );
}

void
FOdysseyExportVectorV2::WriteDefineObjectID( FOdysseyVectorObject& iObject, FArchive &Ar )
{
    FOdysseyExport::WriteChunk( FOdysseyExportVectorV2::CHUNK_DEFINE_OBJECT_ID
                                    , Ar
                                    , [&iObject](FArchive &Ar) -> void
    {
        uint32 objectID = iObject.GetID();

        Ar << objectID;
    } );
}

void
FOdysseyExportVectorV2::WriteDefineObjectEntry( FOdysseyVectorObject& iObject, FArchive &Ar )
{
    FOdysseyExport::WriteChunk( FOdysseyExportVectorV2::CHUNK_DEFINE_OBJECT_ENTRY
                                    , Ar
                                    , [&iObject](FArchive &Ar) -> void
    {
        WriteDefineObjectID( iObject, Ar );

        switch( iObject.GetType() )
        {
            case FOdysseyVectorObject::VECTORPATHTYPE:
            {
                FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(&iObject);

                FOdysseyExportVectorV2::WritePath( *path, Ar );
            }
            break;

            case FOdysseyVectorObject::VECTORGROUPPAINTTYPE:
            {
                FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>(&iObject);

                FOdysseyExportVectorV2::WriteGroupPaint( *paintGroup, Ar );
            }
            break;

            case FOdysseyVectorObject::VECTORROOTTYPE:
            {
                FOdysseyVectorScene* scene = static_cast<FOdysseyVectorScene*>(&iObject);

                FOdysseyExportVectorV2::WriteScene( *scene, Ar );
            }
            break;

            default:
                FOdysseyExportVectorV2::WriteObject( iObject, Ar );
            break;
        }
    } );
}

void
FOdysseyExportVectorV2::WriteDefineObjects( std::vector<FOdysseyVectorObject*>& vectorObjectArray, FArchive &Ar )
{
    FOdysseyExport::WriteChunk( FOdysseyExportVectorV2::CHUNK_DEFINE_OBJECTS
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
