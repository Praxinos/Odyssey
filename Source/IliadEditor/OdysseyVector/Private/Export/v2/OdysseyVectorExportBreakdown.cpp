// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Export/v2/OdysseyVectorExport.h"
// from module OdysseyFile
#include "OdysseyFile.h"
#include "InbetweenerTag/InbetweenerPoint.h"
#include "InbetweenerTag/InbetweenerBreakdown.h"
#include "InbetweenerTag/InbetweenerGrid.h"
#include "OdysseyVectorTagInbetweener.h"

void
FOdysseyVectorExportV2::WriteBreakdownChartHUBBezier( FInbetweenerBreakdown& iBreakdown
                                                    , FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_BREAKDOWN_CHART_HUDBEZIER
                            , Ar
                            , [&iBreakdown](FArchive &Ar) -> void
    {
        FInbetweenerChart::HUDBezier* quadraticBezier = iBreakdown.GetChart()->GetHUDBezier();
        ::ULIS::FVec2D pt[3] = { quadraticBezier->GetPoints()[0].GetPosition()
                               , quadraticBezier->GetPoints()[1].GetPosition()
                               , quadraticBezier->GetPoints()[2].GetPosition() };

        Ar << pt[0].x;
        Ar << pt[0].y;
        Ar << pt[1].x;
        Ar << pt[1].y;
        Ar << pt[2].x;
        Ar << pt[2].y;
    } );
}

void
FOdysseyVectorExportV2::WriteBreakdownChartSpacing( FInbetweenerBreakdown& iBreakdown
                                                  , FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_BREAKDOWN_CHART_SPACING
                            , Ar
                            , [&iBreakdown](FArchive &Ar) -> void
    {
        for( FInbetweenerChart::Inbetween& division : iBreakdown.GetChart()->GetInbetweenBuffer() )
        {
            float spacing = division.GetSpacing();

            Ar << spacing;
        }
    } );
}

void
FOdysseyVectorExportV2::WriteBreakdownChart( FInbetweenerBreakdown& iBreakdown
                                           , FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_BREAKDOWN_CHART
                            , Ar
                            , [&iBreakdown](FArchive &Ar) -> void
    {
        WriteBreakdownChartHUBBezier( iBreakdown, Ar );
        WriteBreakdownChartSpacing( iBreakdown, Ar );
    } );
}

void
FOdysseyVectorExportV2::WriteBreakdownGridGeometry( FInbetweenerBreakdown& iBreakdown
                                                  , FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_BREAKDOWN_GRIDGEOMETRY
                            , Ar
                            , [&iBreakdown](FArchive &Ar) -> void
    {
        std::vector<FInbetweenerPoint>& gridPointbuffer = iBreakdown.GetGrid()->GetPointBuffer();

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
FOdysseyVectorExportV2::WriteBreakdownTransformScaling( FInbetweenerBreakdown& iBreakdown
                                                      , FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_BREAKDOWN_TRANSFORM_SCALING
                            , Ar
                            , [&iBreakdown](FArchive &Ar) -> void
    {
        double scalingX = iBreakdown.GetTargetScalingX();
        double scalingY = iBreakdown.GetTargetScalingY();

        Ar << scalingX;
        Ar << scalingY;
    } );
}

void
FOdysseyVectorExportV2::WriteBreakdownTransformRotation( FInbetweenerBreakdown& iBreakdown
                                                       , FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_BREAKDOWN_TRANSFORM_ROTATION
                            , Ar
                            , [&iBreakdown](FArchive &Ar) -> void
    {
        double rotation = iBreakdown.GetTargetRotation();

        Ar << rotation;
    } );
}

void
FOdysseyVectorExportV2::WriteBreakdownTransformTranslation( FInbetweenerBreakdown& iBreakdown
                                                          , FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_BREAKDOWN_TRANSFORM_TRANSLATION
                            , Ar
                            , [&iBreakdown](FArchive &Ar) -> void
    {
        double translationX = iBreakdown.GetTargetTranslationX();
        double translationY = iBreakdown.GetTargetTranslationY();

        Ar << translationX;
        Ar << translationY;
    } );
}

void
FOdysseyVectorExportV2::WriteBreakdownTransform( FInbetweenerBreakdown& iBreakdown
                                               , FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_BREAKDOWN_TRANSFORM
                            , Ar
                            , [&iBreakdown](FArchive &Ar) -> void
    {
        WriteBreakdownTransformTranslation( iBreakdown, Ar );
        WriteBreakdownTransformRotation( iBreakdown, Ar );
        WriteBreakdownTransformScaling( iBreakdown, Ar );
    } );
}

void
FOdysseyVectorExportV2::WriteBreakdownTargetVisibility( FInbetweenerBreakdown& iBreakdown
                                                      , FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_BREAKDOWN_TARGETVISIBILITY
                            , Ar
                            , [&iBreakdown](FArchive &Ar) -> void
    {
        uint32 visibility = static_cast<uint32>(iBreakdown.IsTargetVisible());

        Ar << visibility;
    } );
}

void
FOdysseyVectorExportV2::WriteBreakdown( FInbetweenerBreakdown& iBreakdown
                                      , FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_BREAKDOWN
                            , Ar
                            , [&iBreakdown](FArchive &Ar) -> void
    {
        uint32 targetIndex = iBreakdown.GetTargetDrawingIndex();
        uint32 ignored = 0;

        Ar << ignored;
        Ar << targetIndex;

        WriteBreakdownTargetVisibility( iBreakdown, Ar );
        WriteBreakdownTransform( iBreakdown, Ar );
        WriteBreakdownGridGeometry( iBreakdown, Ar );
        WriteBreakdownChart( iBreakdown, Ar );
    } );
}
