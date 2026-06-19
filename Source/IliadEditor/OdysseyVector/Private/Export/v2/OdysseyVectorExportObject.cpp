// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Export/v2/OdysseyVectorExport.h"
// from module OdysseyFile
#include "OdysseyFile.h"
#include "OdysseyVectorObject.h"
#include "OdysseyVectorPath.h"
#include "OdysseyVectorGroup.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorTagInbetweener.h"

uint32
FOdysseyVectorExportV2::GetObjectType( FOdysseyVectorObject& iObject )
{
    if( iObject.GetClass() == FOdysseyVectorPath::StaticClass() )
        return FOdysseyFile::VectorV2::ObjectType::PATH;

    if( iObject.GetClass() == FOdysseyVectorGroup::StaticClass() )
        return FOdysseyFile::VectorV2::ObjectType::GROUP;

    if( iObject.GetClass() == FOdysseyVectorGroupPaint::StaticClass() )
        return FOdysseyFile::VectorV2::ObjectType::GROUPPAINT;

    return FOdysseyFile::VectorV2::ObjectType::NONE;
}

void
FOdysseyVectorExportV2::WriteDeclareObjectEntry( FOdysseyVectorObject& iObject, FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_DECLARE_OBJECT_ENTRY
                            , Ar
                            , [&iObject](FArchive &Ar) -> void
    {
        uint32 objectType = GetObjectType( iObject );

        Ar << objectType;
    } );
}

void
FOdysseyVectorExportV2::WriteDeclareObjects( std::vector<FOdysseyVectorObject*>& vectorObjectArray, FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_DECLARE_OBJECTS
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
FOdysseyVectorExportV2::WriteObjectTransformSkew( FOdysseyVectorObject& iObject, FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_OBJECT_TRANSFORM_SKEW
                            , Ar
                            , [&iObject](FArchive &Ar) -> void
    {
        double skewX = iObject.GetSkewX(),
               skewY = iObject.GetSkewY();

        Ar << skewX;
        Ar << skewY;
    } );
}

void
FOdysseyVectorExportV2::WriteObjectTransformScaling( FOdysseyVectorObject& iObject, FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_OBJECT_TRANSFORM_SCALING
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
FOdysseyVectorExportV2::WriteObjectTransformRotation( FOdysseyVectorObject& iObject, FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_OBJECT_TRANSFORM_ROTATION
                            , Ar
                            , [&iObject](FArchive &Ar) -> void
    {
        double rotation = iObject.GetRotation();

        Ar << rotation;
    } );
}

void
FOdysseyVectorExportV2::WriteObjectTransformTranslation( FOdysseyVectorObject& iObject, FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_OBJECT_TRANSFORM_TRANSLATION
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
FOdysseyVectorExportV2::WriteObjectTransform( FOdysseyVectorObject& iObject, FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_OBJECT_TRANSFORM
                            , Ar
                            , [&iObject](FArchive &Ar) -> void
    {
        WriteObjectTransformTranslation( iObject, Ar );
        WriteObjectTransformRotation( iObject, Ar );
        WriteObjectTransformScaling( iObject, Ar );
        WriteObjectTransformSkew( iObject, Ar );
    } );
}

void
FOdysseyVectorExportV2::WriteObjectParentID( FOdysseyVectorObject& iObject, FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_OBJECT_PARENTID
                            , Ar
                            , [&iObject](FArchive &Ar) -> void
    {
        uint32 parentID = iObject.GetParent() ? iObject.GetParent()->GetID() : 0;

        Ar << parentID;
    } );
}

void
FOdysseyVectorExportV2::WriteObjectName( FOdysseyVectorObject& iObject, FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_OBJECT_NAME
                            , Ar
                            , [&iObject](FArchive &Ar) -> void
    {
        FString& name = iObject.GetName();

        Ar << name;
    } );
}

void
FOdysseyVectorExportV2::WriteObjectForegroundBucket( FOdysseyVectorObject& iObject, FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_OBJECT_FOREGROUNDBUCKET
                            , Ar
                            , [&iObject](FArchive &Ar) -> void
    {
        FOdysseyVectorBucket& foregroundBucket = iObject.GetForegroundBucket();

        FOdysseyVectorExportV2::WriteBucket( foregroundBucket, Ar );
    } );
}

void
FOdysseyVectorExportV2::WriteObjectBackgroundBucket( FOdysseyVectorObject& iObject, FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_OBJECT_BACKGROUNDBUCKET
                            , Ar
                            , [&iObject](FArchive &Ar) -> void
    {
        FOdysseyVectorBucket& backgroundBucket = iObject.GetBackgroundBucket();

        FOdysseyVectorExportV2::WriteBucket( backgroundBucket, Ar );
    } );
}

void
FOdysseyVectorExportV2::WriteObjectTags( FOdysseyVectorObject& iObject, FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_OBJECT_TAGS
                            , Ar
                            , [&iObject](FArchive &Ar) -> void
    {
        for( FOdysseyVectorTag* tag : iObject.GetTagList() )
        {
            if( tag->GetClass() == FOdysseyVectorTagInbetweener::StaticClass() )
            {
                FOdysseyVectorTagInbetweener* inbetweenerTag = static_cast<FOdysseyVectorTagInbetweener*>(tag);

                FOdysseyVectorExportV2::WriteTagInbetweener( *inbetweenerTag, Ar );
            }
        }
    } );
}

void
FOdysseyVectorExportV2::WriteObjectForegroundColor( FOdysseyVectorObject& iObject, FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_OBJECT_FOREGROUNDCOLOR
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
FOdysseyVectorExportV2::WriteObjectBackgroundColor( FOdysseyVectorObject& iObject, FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_OBJECT_BACKGROUNDCOLOR
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
FOdysseyVectorExportV2::WriteObjectVisibility( FOdysseyVectorObject& iObject, FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_OBJECT_VISIBILITY
                            , Ar
                            , [&iObject](FArchive &Ar) -> void
    {
        uint32 visible = iObject.IsVisible( false ) ? 1 : 0;

        Ar << visible;
    } );
}

void
FOdysseyVectorExportV2::WriteObjectOpacity( FOdysseyVectorObject& iObject, FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_OBJECT_OPACITY
                            , Ar
                            , [&iObject](FArchive &Ar) -> void
    {
        double opacity = iObject.GetOpacity();

        Ar << opacity;
    } );
}

void
FOdysseyVectorExportV2::WriteObjectExpansion( FOdysseyVectorObject& iObject, FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_OBJECT_EXPANSION
                            , Ar
                            , [&iObject](FArchive &Ar) -> void
    {
        uint32 expanded = static_cast<uint32>(iObject.IsExpanded());

        Ar << expanded;
    } );
}

void
FOdysseyVectorExportV2::WriteObjectChunks( FOdysseyVectorObject& iObject, FArchive &Ar )
{
    WriteObjectParentID( iObject, Ar );
    WriteObjectName( iObject, Ar );
    WriteObjectTransform( iObject, Ar );
    WriteObjectVisibility( iObject, Ar );
    WriteObjectOpacity( iObject, Ar );
    WriteObjectExpansion( iObject, Ar );
    WriteObjectForegroundBucket( iObject, Ar );
    WriteObjectBackgroundBucket( iObject, Ar );
    WriteObjectTags( iObject, Ar );
}

void
FOdysseyVectorExportV2::WriteObject( FOdysseyVectorObject& iObject, FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_OBJECT
                            , Ar
                            , [&iObject](FArchive &Ar) -> void
    {
        WriteObjectChunks( iObject, Ar );
    } );
}

void
FOdysseyVectorExportV2::WriteDefineObjectID( FOdysseyVectorObject& iObject, FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_DEFINE_OBJECT_ID
                            , Ar
                            , [&iObject](FArchive &Ar) -> void
    {
        uint32 objectID = iObject.GetID();

        Ar << objectID;
    } );
}

void
FOdysseyVectorExportV2::WriteDefineObjectEntry( FOdysseyVectorObject& iObject, FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_DEFINE_OBJECT_ENTRY
                            , Ar
                            , [&iObject](FArchive &Ar) -> void
    {
        WriteDefineObjectID( iObject, Ar );

        switch( GetObjectType( iObject ) )
        {
            case FOdysseyFile::VectorV2::ObjectType::PATH:
            {
                FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(&iObject);

                FOdysseyVectorExportV2::WritePath( *path, Ar );
            }
            break;

            case FOdysseyFile::VectorV2::ObjectType::GROUP:
            {
                FOdysseyVectorGroup* group = static_cast<FOdysseyVectorGroup*>(&iObject);

                FOdysseyVectorExportV2::WriteGroup( *group, Ar );
            }
            break;

            case FOdysseyFile::VectorV2::ObjectType::GROUPPAINT:
            {
                FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>(&iObject);

                FOdysseyVectorExportV2::WriteGroupPaint( *paintGroup, Ar );
            }
            break;

            default:
                FOdysseyVectorExportV2::WriteObject( iObject, Ar );
            break;
        }
    } );
}

void
FOdysseyVectorExportV2::WriteDefineObjects( std::vector<FOdysseyVectorObject*>& vectorObjectArray, FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_DEFINE_OBJECTS
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
