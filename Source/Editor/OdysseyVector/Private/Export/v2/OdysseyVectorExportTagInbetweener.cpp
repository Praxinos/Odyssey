#include "Export/v2/OdysseyVectorExport.h"
#include "Palette/OdysseyPaletteEntry.h"
// from module OdysseyFile
#include "OdysseyFile.h"
#include "InbetweenerTag/InbetweenerPoint.h"
#include "InbetweenerTag/InbetweenerGrid.h"
#include "OdysseyVectorTagInbetweener.h"

void
FOdysseyVectorExportV2::WriteTagInbetweenerGridGeometryMk2( FOdysseyVectorTagInbetweener& iInbetweenerTag
                                                          , FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_TAGINBETWEENER_GRID_GEOMETRY_MK2
                            , Ar
                            , [&iInbetweenerTag](FArchive &Ar) -> void
    {
        std::vector<FInbetweenerPoint>& gridPointbuffer = iInbetweenerTag.GetGridPointBuffer();
        uint32 numQuadX = iInbetweenerTag.GetGridNumQuadX();
        uint32 numQuadY = iInbetweenerTag.GetGridNumQuadY();

        Ar << numQuadX;
        Ar << numQuadY;

        for( FInbetweenerPoint& point : gridPointbuffer )
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
FOdysseyVectorExportV2::WriteTagInbetweenerGridGeometry( FOdysseyVectorTagInbetweener& iInbetweenerTag
                                                       , FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_TAGINBETWEENER_GRID_GEOMETRY
                            , Ar
                            , [&iInbetweenerTag](FArchive &Ar) -> void
    {
        std::vector<FInbetweenerPoint>& gridPointbuffer = iInbetweenerTag.GetGridPointBuffer();

        for( FInbetweenerPoint& point : gridPointbuffer )
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
FOdysseyVectorExportV2::WriteTagInbetweenerGridInterpolation( FOdysseyVectorTagInbetweener& iInbetweenerTag, FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_TAGINBETWEENER_GRID_INTERPOLATION
                            , Ar
                            , [&iInbetweenerTag](FArchive &Ar) -> void
    {
        uint32 interpolationType = static_cast<uint32>(iInbetweenerTag.GetInterpolationType());

        Ar << interpolationType;
    } );
}

void
FOdysseyVectorExportV2::WriteTagInbetweenerGridArapRigidity( FInbetweenerGridARAP& iArapGrid
                                                           , FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_TAGINBETWEENER_GRID_ARAP_RIGIDITY
                            , Ar
                            , [&iArapGrid](FArchive &Ar) -> void
    {
        uint32 rigidty = static_cast<uint32>(iArapGrid.GetRigidity());

        Ar << rigidty;
    } );
}

void
FOdysseyVectorExportV2::WriteTagInbetweenerGridType( FOdysseyVectorTagInbetweener& iInbetweenerTag
                                                   , FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_TAGINBETWEENER_GRID_TYPE
                            , Ar
                            , [&iInbetweenerTag](FArchive &Ar) -> void
    {
        uint32 gridType = static_cast<uint32>(iInbetweenerTag.GetGridType());

        Ar << gridType;
    } );
}

void
FOdysseyVectorExportV2::WriteTagInbetweenerGridSize( FOdysseyVectorTagInbetweener& iInbetweenerTag
                                                   , FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_TAGINBETWEENER_GRID_SIZE
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
FOdysseyVectorExportV2::WriteTagInbetweenerGrid( FOdysseyVectorTagInbetweener& iInbetweenerTag
                                               , FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_TAGINBETWEENER_GRID
                            , Ar
                            , [&iInbetweenerTag](FArchive &Ar) -> void
    {
        WriteTagInbetweenerGridType( iInbetweenerTag, Ar );
        WriteTagInbetweenerGridInterpolation( iInbetweenerTag, Ar );
        //WriteTagInbetweenerGridSize( iInbetweenerTag, Ar );
        //WriteTagInbetweenerGridGeometry( iInbetweenerTag, Ar );
        WriteTagInbetweenerGridGeometryMk2( iInbetweenerTag, Ar );

        if( iInbetweenerTag.GetGridType() == eInbetweenerGridType::ARAP )
        {
            FInbetweenerGridARAP* arapGrid = static_cast<FInbetweenerGridARAP*>(iInbetweenerTag.GetGrid());

            WriteTagInbetweenerGridArapRigidity( *arapGrid, Ar );
        }
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
        WriteTagInbetweenerGrid( iInbetweenerTag, Ar );
    } );
}
