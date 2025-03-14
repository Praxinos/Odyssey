// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "Export/v2/OdysseyVectorExport.h"
#include "Palette/OdysseyPaletteEntry.h"
// from module OdysseyFile
#include "OdysseyFile.h"
#include "InbetweenerTag/InbetweenerPoint.h"
#include "InbetweenerTag/InbetweenerBreakdown.h"
#include "InbetweenerTag/InbetweenerTrajectory.h"
#include "InbetweenerTag/InbetweenerHandleTrajectory.h"
#include "InbetweenerTag/InbetweenerGrid.h"
#include "OdysseyVectorTagInbetweener.h"

void
FOdysseyVectorExportV2::WriteTagInbetweenerRoutes( FOdysseyVectorTagInbetweener& iInbetweenerTag
                                                 , FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_TAGINBETWEENER_ROUTES
                            , Ar
                            , [&iInbetweenerTag](FArchive &Ar) -> void
    {
        for( FInbetweenerRoute* route : iInbetweenerTag.GetRouteList() )
        {
            WriteRoute( *route, Ar );
        }
    } );
}

void
FOdysseyVectorExportV2::WriteTagInbetweenerBreakdownsLayout( FOdysseyVectorTagInbetweener& iInbetweenerTag
                                                             , FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_TAGINBETWEENER_BREAKDOWNS_LAYOUT
                            , Ar
                            , [&iInbetweenerTag](FArchive &Ar) -> void
    {
        uint32 breakdownCount = iInbetweenerTag.GetBreakdownCount();

        Ar << breakdownCount;

        for( FInbetweenerBreakdown* breakdown : iInbetweenerTag.GetBreakdownList() )
        {
            uint32 ignored = 0;
            uint32 sourceDrawingIndex = breakdown->GetSourceDrawingIndex();
            uint32 targetDrawingIndex = breakdown->GetTargetDrawingIndex();

            Ar << ignored;
            Ar << sourceDrawingIndex;
            Ar << targetDrawingIndex;
        }
    } );
}

void
FOdysseyVectorExportV2::WriteTagInbetweenerBreakdownsGeometry( FOdysseyVectorTagInbetweener& iInbetweenerTag
                                                             , FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_TAGINBETWEENER_BREAKDOWNS_GRIDGEOMETRY
                            , Ar
                            , [&iInbetweenerTag](FArchive &Ar) -> void
    {
        for( FInbetweenerBreakdown* breakdown : iInbetweenerTag.GetBreakdownList() )
        {
            std::vector<FInbetweenerPoint>& gridPointbuffer = breakdown->GetGrid()->GetPointBuffer();

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
        }
    } );
}

void
FOdysseyVectorExportV2::WriteTagInbetweenerBreakdowns( FOdysseyVectorTagInbetweener& iInbetweenerTag
                                                     , FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_TAGINBETWEENER_BREAKDOWNS
                            , Ar
                            , [&iInbetweenerTag](FArchive &Ar) -> void
    {
        WriteTagInbetweenerBreakdownsLayout( iInbetweenerTag, Ar );
        // deprecated
        // WriteTagInbetweenerBreakdownsGeometry( iInbetweenerTag, Ar );

        // then write the rest
        for( FInbetweenerBreakdown* breakdown : iInbetweenerTag.GetBreakdownList() )
        {
            WriteBreakdown( *breakdown, Ar );
        }
    } );
}

void
FOdysseyVectorExportV2::WriteTagInbetweenerDimension( FOdysseyVectorTagInbetweener& iInbetweenerTag
                                                    , FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_TAGINBETWEENER_GRID_SPECS
                            , Ar
                            , [&iInbetweenerTag](FArchive &Ar) -> void
    {
        uint32 gridType = static_cast<uint32>(iInbetweenerTag.GetGridType());
        uint32 numQuadX = iInbetweenerTag.GetGridNumQuadX();
        uint32 numQuadY = iInbetweenerTag.GetGridNumQuadY();
        uint32 square = iInbetweenerTag.IsSquare() ? 1 : 0;

        Ar << gridType;
        Ar << numQuadX;
        Ar << numQuadY;
        Ar << square;
    } );
}

void
FOdysseyVectorExportV2::WriteTagInbetweenerDirection( FOdysseyVectorTagInbetweener& iInbetweenerTag, FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_TAGINBETWEENER_DIRECTION
                            , Ar
                            , [&iInbetweenerTag](FArchive &Ar) -> void
    {
        uint32 directionType = static_cast<uint32>(iInbetweenerTag.GetInterpolationDirection());

        Ar << directionType;
    } );
}

void
FOdysseyVectorExportV2::WriteTagInbetweenerInterpolation( FOdysseyVectorTagInbetweener& iInbetweenerTag, FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_TAGINBETWEENER_INTERPOLATION
                            , Ar
                            , [&iInbetweenerTag](FArchive &Ar) -> void
    {
        uint32 interpolationType = static_cast<uint32>(iInbetweenerTag.GetInterpolationType());

        Ar << interpolationType;
    } );
}

void
FOdysseyVectorExportV2::WriteTagInbetweenerDeformation( FOdysseyVectorTagInbetweener& iInbetweenerTag
                                                      , FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_TAGINBETWEENER_DEFORMATION
                            , Ar
                            , [&iInbetweenerTag](FArchive &Ar) -> void
    {
        uint32 gridType = static_cast<uint32>(iInbetweenerTag.GetGridType());

        Ar << gridType;
    } );
}

void
FOdysseyVectorExportV2::WriteTagInbetweenerDrawingCount( FOdysseyVectorTagInbetweener& iInbetweenerTag
                                                       , FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_TAGINBETWEENER_LENGTH
                            , Ar
                            , [&iInbetweenerTag](FArchive &Ar) -> void
    {
        uint32 inbetweenCount = iInbetweenerTag.GetLength();

        Ar << inbetweenCount;
    } );
}

void
FOdysseyVectorExportV2::WriteTagInbetweenerColor( FOdysseyVectorTagInbetweener& iInbetweenerTag
                                                , FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_TAGINBETWEENER_INBETWEENCOLOR
                            , Ar
                            , [&iInbetweenerTag](FArchive &Ar) -> void
    {
        FColor color = iInbetweenerTag.GetInbetweenColor();
        uint8 r8 = color.R;
        uint8 g8 = color.G;
        uint8 b8 = color.B;
        uint8 a8 = color.A;

        Ar << r8;
        Ar << g8;
        Ar << b8;
        Ar << a8;
    } );
}

void
FOdysseyVectorExportV2::WriteTagInbetweenerChartColor( FOdysseyVectorTagInbetweener& iInbetweenerTag
                                                     , FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_TAGINBETWEENER_CHARTCOLOR
                            , Ar
                            , [&iInbetweenerTag](FArchive &Ar) -> void
    {
        FColor color = iInbetweenerTag.GetChartColor();
        uint8 r8 = color.R;
        uint8 g8 = color.G;
        uint8 b8 = color.B;
        uint8 a8 = color.A;

        Ar << r8;
        Ar << g8;
        Ar << b8;
        Ar << a8;
    } );
}

void
FOdysseyVectorExportV2::WriteTagInbetweenerGridColor( FOdysseyVectorTagInbetweener& iInbetweenerTag
                                                     , FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_TAGINBETWEENER_GRIDCOLOR
                            , Ar
                            , [&iInbetweenerTag](FArchive &Ar) -> void
    {
        FColor color = iInbetweenerTag.GetGridColor();
        uint8 r8 = color.R;
        uint8 g8 = color.G;
        uint8 b8 = color.B;
        uint8 a8 = color.A;

        Ar << r8;
        Ar << g8;
        Ar << b8;
        Ar << a8;
    } );
}

void
FOdysseyVectorExportV2::WriteTagInbetweenerTrajectoryColor( FOdysseyVectorTagInbetweener& iInbetweenerTag
                                                          , FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_TAGINBETWEENER_TRAJECTORYCOLOR
                            , Ar
                            , [&iInbetweenerTag](FArchive &Ar) -> void
    {
        FColor color = iInbetweenerTag.GetTrajectoryColor();
        uint8 r8 = color.R;
        uint8 g8 = color.G;
        uint8 b8 = color.B;
        uint8 a8 = color.A;

        Ar << r8;
        Ar << g8;
        Ar << b8;
        Ar << a8;
    } );
}

void
FOdysseyVectorExportV2::WriteTagInbetweenerMapAsPolyline( FOdysseyVectorTagInbetweener& iInbetweenerTag
                                                        , FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_TAGINBETWEENER_MAPASPOLYLINE
                            , Ar
                            , [&iInbetweenerTag](FArchive &Ar) -> void
    {
        uint32 mapAsPolyline = iInbetweenerTag.GetMapAsPolyline() ? 1 : 0;

        Ar << mapAsPolyline;
    } );
}

void
FOdysseyVectorExportV2::WriteTagInbetweenerWithThickness( FOdysseyVectorTagInbetweener& iInbetweenerTag
                                                        , FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_TAGINBETWEENER_WITHTHICKNESS
                            , Ar
                            , [&iInbetweenerTag](FArchive &Ar) -> void
    {
        uint32 withThickness = iInbetweenerTag.GetWithThickness() ? 1 : 0;

        Ar << withThickness;
    } );
}

void
FOdysseyVectorExportV2::WriteTagInbetweenerConstantWidth( FOdysseyVectorTagInbetweener& iInbetweenerTag
                                                        , FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_TAGINBETWEENER_CONSTANTWIDTH
                            , Ar
                            , [&iInbetweenerTag](FArchive &Ar) -> void
    {
        uint32 constantWidth = iInbetweenerTag.HasConstantWidth() ? 1 : 0;

        Ar << constantWidth;
    } );
}

void
FOdysseyVectorExportV2::WriteTagInbetweener( FOdysseyVectorTagInbetweener& iInbetweenerTag, FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_TAGINBETWEENER
                            , Ar
                            , [&iInbetweenerTag](FArchive &Ar) -> void
    {
        WriteTagInbetweenerColor( iInbetweenerTag, Ar );
        WriteTagInbetweenerChartColor( iInbetweenerTag, Ar );
        WriteTagInbetweenerGridColor( iInbetweenerTag, Ar );
        WriteTagInbetweenerDrawingCount( iInbetweenerTag, Ar );
        WriteTagInbetweenerMapAsPolyline( iInbetweenerTag, Ar );
        WriteTagInbetweenerWithThickness( iInbetweenerTag, Ar );
        WriteTagInbetweenerConstantWidth( iInbetweenerTag, Ar );
        //WriteTagInbetweenerTransform( iInbetweenerTag, Ar );
        WriteTagInbetweenerDeformation( iInbetweenerTag, Ar );
        WriteTagInbetweenerInterpolation( iInbetweenerTag, Ar );
        WriteTagInbetweenerDirection( iInbetweenerTag, Ar );
        WriteTagInbetweenerDimension( iInbetweenerTag, Ar );
        WriteTagInbetweenerBreakdowns( iInbetweenerTag, Ar );
        WriteTagInbetweenerRoutes( iInbetweenerTag, Ar );
    } );
}
