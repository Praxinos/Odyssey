// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Export/v1/OdysseyVectorExport.h"
// from module OdysseyFile
#include "OdysseyFile.h"
#include "OdysseyVectorPath.h"
#include "OdysseyVectorGroup.h"
#include "OdysseyVectorGroupPaint.h"

uint32
FOdysseyVectorExportV1::GetObjectType( FOdysseyVectorObject& iObject )
{
    if( iObject.GetClass() == FOdysseyVectorPath::StaticClass() )
        return FOdysseyFile::VectorV1::ObjectType::PATH;

    if( iObject.GetClass() == FOdysseyVectorGroup::StaticClass() )
        return FOdysseyFile::VectorV1::ObjectType::GROUP;

    if( iObject.GetClass() == FOdysseyVectorGroupPaint::StaticClass() )
        return FOdysseyFile::VectorV1::ObjectType::GROUPPAINT;

    return FOdysseyFile::VectorV1::ObjectType::NONE;
}

void
FOdysseyVectorExportV1::WriteDeclareObjectEntry( FOdysseyVectorObject& iObject, FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV1::CHUNK_DECLARE_OBJECT_ENTRY
                            , Ar
                            , [&iObject](FArchive &Ar) -> void
    {
        uint32 objectType = GetObjectType( iObject );

        Ar << objectType;
    } );
}

void
FOdysseyVectorExportV1::WriteDeclareObjects( std::vector<FOdysseyVectorObject*>& vectorObjectArray, FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV1::CHUNK_DECLARE_OBJECTS
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
FOdysseyVectorExportV1::WriteObjectTransformScaling( FOdysseyVectorObject& iObject, FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV1::CHUNK_OBJECT_TRANSFORM_SCALING
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
FOdysseyVectorExportV1::WriteObjectTransformRotation( FOdysseyVectorObject& iObject, FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV1::CHUNK_OBJECT_TRANSFORM_ROTATION
                            , Ar
                            , [&iObject](FArchive &Ar) -> void
    {
        double rotation = iObject.GetRotation();

        Ar << rotation;
    } );
}

void
FOdysseyVectorExportV1::WriteObjectTransformTranslation( FOdysseyVectorObject& iObject, FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV1::CHUNK_OBJECT_TRANSFORM_TRANSLATION
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
FOdysseyVectorExportV1::WriteObjectTransform( FOdysseyVectorObject& iObject, FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV1::CHUNK_OBJECT_TRANSFORM
                            , Ar
                            , [&iObject](FArchive &Ar) -> void
    {
        WriteObjectTransformTranslation( iObject, Ar );
        WriteObjectTransformRotation( iObject, Ar );
        WriteObjectTransformScaling( iObject, Ar );
    } );
}

void
FOdysseyVectorExportV1::WriteObjectParentID( FOdysseyVectorObject& iObject, FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV1::CHUNK_OBJECT_PARENTID
                            , Ar
                            , [&iObject](FArchive &Ar) -> void
    {
        uint32 parentID = iObject.GetParent() ? iObject.GetParent()->GetID() : 0;

        Ar << parentID;
    } );
}

void
FOdysseyVectorExportV1::WriteObjectID( FOdysseyVectorObject& iObject, FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV1::CHUNK_OBJECT_ID
                            , Ar
                            , [&iObject](FArchive &Ar) -> void
    {
        uint32 objectID = iObject.GetID();

        Ar << objectID;
    } );
}

void
FOdysseyVectorExportV1::WriteObjectForegroundBucket( FOdysseyVectorObject& iObject, FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV1::CHUNK_OBJECT_FOREGROUNDBUCKET
                            , Ar
                            , [&iObject](FArchive &Ar) -> void
    {
        FOdysseyVectorBucket& foregroundBucket = iObject.GetForegroundBucket();

        FOdysseyVectorExportV1::WriteBucket( foregroundBucket, Ar );
    } );
}

void
FOdysseyVectorExportV1::WriteObjectBackgroundBucket( FOdysseyVectorObject& iObject, FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV1::CHUNK_OBJECT_BACKGROUNDBUCKET
                            , Ar
                            , [&iObject](FArchive &Ar) -> void
    {
        FOdysseyVectorBucket& backgroundBucket = iObject.GetBackgroundBucket();

        FOdysseyVectorExportV1::WriteBucket( backgroundBucket, Ar );
    } );
}

void
FOdysseyVectorExportV1::WriteObjectForegroundColor( FOdysseyVectorObject& iObject, FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV1::CHUNK_OBJECT_FOREGROUNDCOLOR
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
FOdysseyVectorExportV1::WriteObjectBackgroundColor( FOdysseyVectorObject& iObject, FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV1::CHUNK_OBJECT_BACKGROUNDCOLOR
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
FOdysseyVectorExportV1::WriteDefineObjectEntry( FOdysseyVectorObject& iObject, FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV1::CHUNK_DEFINE_OBJECT_ENTRY
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

        switch( GetObjectType( iObject ) )
        {
            case FOdysseyFile::VectorV1::ObjectType::PATH:
            {
                FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(&iObject);

                FOdysseyVectorExportV1::WritePath( *path, Ar );
            }
            break;

            case FOdysseyFile::VectorV1::ObjectType::GROUPPAINT:
            {
                FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>(&iObject);

                FOdysseyVectorExportV1::WriteGroupPaint( *paintGroup, Ar );
            }
            break;

            default:
            break;
        }
    } );
}

void
FOdysseyVectorExportV1::WriteDefineObjects( std::vector<FOdysseyVectorObject*>& vectorObjectArray, FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV1::CHUNK_DEFINE_OBJECTS
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
