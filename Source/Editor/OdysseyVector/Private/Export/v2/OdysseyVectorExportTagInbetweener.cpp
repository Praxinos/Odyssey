#include "Export/v2/OdysseyVectorExport.h"
#include "Palette/OdysseyPaletteEntry.h"
// from module OdysseyFile
#include "OdysseyFile.h"
#include "OdysseyVectorTagInbetweener.h"

void
FOdysseyVectorExportV2::WriteTagInbetweenerFFDGridGeometry( FOdysseyVectorTagInbetweener& iInbetweenerTag
                                                          , FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_TAGINBETWEENER_FFDGRID_GEOMETRY
                            , Ar
                            , [&iInbetweenerTag](FArchive &Ar) -> void
    {
        std::vector<FInbetweenerGridPoint>& gridPointbuffer = iInbetweenerTag.GetGridPointBuffer();

        for( FInbetweenerGridPoint& point : gridPointbuffer )
        {
            double sourceX = point.GetSourcePosition().x;
            double sourceY = point.GetSourcePosition().y;
            double targetX = point.GetTargetPosition().x;
            double targetY = point.GetTargetPosition().y;

            Ar << sourceX;
            Ar << sourceY;
            Ar << targetX;
            Ar << targetY;
        }
    } );
}

void
FOdysseyVectorExportV2::WriteTagInbetweenerFFDGridSize( FOdysseyVectorTagInbetweener& iInbetweenerTag, FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_TAGINBETWEENER_FFDGRID_SIZE
                            , Ar
                            , [&iInbetweenerTag](FArchive &Ar) -> void
    {
        uint32 numQuadX = iInbetweenerTag.GetGridNumQuadX();
        uint32 numQuadY = iInbetweenerTag.GetGridNumQuadY();

        Ar << numQuadX;
        Ar << numQuadY;
    } );
}

void
FOdysseyVectorExportV2::WriteTagInbetweenerFFDGrid( FOdysseyVectorTagInbetweener& iInbetweenerTag
                                                  , FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_TAGINBETWEENER_FFDGRID
                            , Ar
                            , [&iInbetweenerTag](FArchive &Ar) -> void
    {
        WriteTagInbetweenerFFDGridSize( iInbetweenerTag, Ar );
        WriteTagInbetweenerFFDGridGeometry( iInbetweenerTag, Ar );
    } );
}

void
FOdysseyVectorExportV2::WriteTagInbetweenerTransformScaling( FOdysseyVectorTagInbetweener& iInbetweenerTag
                                                           , FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_TAGINBETWEENER_TRANSFORM_SCALING
                            , Ar
                            , [&iInbetweenerTag](FArchive &Ar) -> void
    {
        double scalingX = iInbetweenerTag.GetTargetScalingX();
        double scalingY = iInbetweenerTag.GetTargetScalingY();

        Ar << scalingX;
        Ar << scalingY;
    } );
}

void
FOdysseyVectorExportV2::WriteTagInbetweenerTransformRotation( FOdysseyVectorTagInbetweener& iInbetweenerTag
                                                            , FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_TAGINBETWEENER_TRANSFORM_ROTATION
                            , Ar
                            , [&iInbetweenerTag](FArchive &Ar) -> void
    {
        double rotation = iInbetweenerTag.GetTargetRotation();

        Ar << rotation;
    } );
}

void
FOdysseyVectorExportV2::WriteTagInbetweenerTransformTranslation( FOdysseyVectorTagInbetweener& iInbetweenerTag
                                                               , FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_TAGINBETWEENER_TRANSFORM_TRANSLATION
                            , Ar
                            , [&iInbetweenerTag](FArchive &Ar) -> void
    {
        double translationX = iInbetweenerTag.GetTargetTranslationX();
        double translationY = iInbetweenerTag.GetTargetTranslationY();

        Ar << translationX;
        Ar << translationY;
    } );
}

void
FOdysseyVectorExportV2::WriteTagInbetweenerTransform( FOdysseyVectorTagInbetweener& iInbetweenerTag
                                                    , FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_TAGINBETWEENER_TRANSFORM
                            , Ar
                            , [&iInbetweenerTag](FArchive &Ar) -> void
    {
        WriteTagInbetweenerTransformTranslation( iInbetweenerTag, Ar );
        WriteTagInbetweenerTransformRotation( iInbetweenerTag, Ar );
        WriteTagInbetweenerTransformScaling( iInbetweenerTag, Ar );
    } );
}

void
FOdysseyVectorExportV2::WriteTagInbetweenerChart( FOdysseyVectorTagInbetweener& iInbetweenerTag
                                                , FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_TAGINBETWEENER_CHART
                            , Ar
                            , [&iInbetweenerTag](FArchive &Ar) -> void
    {
        for( FInbetweenerInbetween& inbetween : iInbetweenerTag.GetChart().inbetweenBuffer )
        {
            float spacing = inbetween.spacing;

            Ar << spacing;
        }
    } );
}

void
FOdysseyVectorExportV2::WriteTagInbetweenerInbetweenCount( FOdysseyVectorTagInbetweener& iInbetweenerTag
                                                         , FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_TAGINBETWEENER_INBETWEENCOUNT
                            , Ar
                            , [&iInbetweenerTag](FArchive &Ar) -> void
    {
        uint32 inbetweenCount = iInbetweenerTag.GetInbetweenCount();

        Ar << inbetweenCount;
    } );
}

void
FOdysseyVectorExportV2::WriteTagInbetweener( FOdysseyVectorTagInbetweener& iInbetweenerTag, FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_TAGINBETWEENER
                            , Ar
                            , [&iInbetweenerTag](FArchive &Ar) -> void
    {
        WriteTagInbetweenerInbetweenCount( iInbetweenerTag, Ar );
        WriteTagInbetweenerChart( iInbetweenerTag, Ar );
        WriteTagInbetweenerTransform( iInbetweenerTag, Ar );

        if( iInbetweenerTag.GetGridType() == eInbetweenerGridType::FFD )
        {
            WriteTagInbetweenerFFDGrid( iInbetweenerTag, Ar );
        }
    } );
}
