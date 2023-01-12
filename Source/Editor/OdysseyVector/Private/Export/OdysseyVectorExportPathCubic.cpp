#include "Export/OdysseyVectorExport.h"
#include "OdysseyVectorSegmentCubic.h"

static void
WritePathCubicGeometrySegments( UOdysseyVectorPathCubic& iCubicPath, FArchive &Ar )
{
    if( iCubicPath.GetSegmentList().size() )
    {
        FOdysseyVectorExport::WriteChunk( FOdysseyVectorExport::CHUNK_PATHCUBIC_GEOMETRY_SEGMENTS
                                        , Ar
                                        , [&iCubicPath](FArchive &Ar) -> void
        {
            uint32 segmentCount = iCubicPath.GetSegmentList().size();

            Ar << segmentCount;

            for( std::list<UOdysseyVectorSegment*>::iterator it = iCubicPath.GetSegmentList().begin(); it != iCubicPath.GetSegmentList().end(); ++it )
            {
                UOdysseyVectorSegmentCubic* cubicSegment = static_cast<UOdysseyVectorSegmentCubic*>(*it);
                uint32 p0ID = cubicSegment->GetPoint(0)->GetID();
                uint32 p1ID = cubicSegment->GetPoint(1)->GetID();
                ::ULIS::FVec2D& ctrlPoint0 = cubicSegment->GetControlPoint(0)->GetCoords();
                ::ULIS::FVec2D& ctrlPoint1 = cubicSegment->GetControlPoint(1)->GetCoords();
                double ctrlPoint0X = ctrlPoint0.x;
                double ctrlPoint0Y = ctrlPoint0.y;
                double ctrlPoint1X = ctrlPoint1.x;
                double ctrlPoint1Y = ctrlPoint1.y;

                Ar << p0ID;
                Ar << p1ID;

                Ar << ctrlPoint0X;
                Ar << ctrlPoint0Y;
                Ar << ctrlPoint1X;
                Ar << ctrlPoint1Y;
            }
        } );
    }
}

static void
WritePathCubicGeometryVertices( UOdysseyVectorPathCubic& iCubicPath, FArchive &Ar )
{
    if ( iCubicPath.GetVertexList().size() )
    {
        FOdysseyVectorExport::WriteChunk( FOdysseyVectorExport::CHUNK_PATHCUBIC_GEOMETRY_VERTICES
                                        , Ar
                                        , [&iCubicPath](FArchive &Ar) -> void
        {
            uint32 vertexID = 0;
            uint32 vertexCount = iCubicPath.GetVertexList().size();

            Ar << vertexCount;

            for( std::list<UOdysseyVectorVertex*>::iterator it = iCubicPath.GetVertexList().begin(); it != iCubicPath.GetVertexList().end(); ++it )
            {
                UOdysseyVectorVertexCubic* vertex = static_cast<UOdysseyVectorVertexCubic*>(*it);
                double x = vertex->GetX();
                double y = vertex->GetY();
                double radius = vertex->GetRadius();

                // used as a reference when writing segments
                vertex->SetID( vertexID++ );

                Ar << x;
                Ar << y;
                Ar << radius;
            }
        } );
    }
}

static void
WritePathCubicGeometry( UOdysseyVectorPathCubic& iCubicPath, FArchive &Ar )
{
    FOdysseyVectorExport::WriteChunk( FOdysseyVectorExport::CHUNK_PATHCUBIC_GEOMETRY
                                    , Ar
                                    , [&iCubicPath](FArchive &Ar) -> void
    {
        WritePathCubicGeometryVertices( iCubicPath, Ar );
        WritePathCubicGeometrySegments( iCubicPath, Ar );
    } );
}

void
FOdysseyVectorExport::WriteObjectPathCubic( UOdysseyVectorPathCubic& iCubicPath, FArchive &Ar )
{
    FOdysseyVectorExport::WriteChunk( FOdysseyVectorExport::CHUNK_OBJECT_PATHCUBIC
                                    , Ar
                                    , [&iCubicPath](FArchive &Ar) -> void
    {
        WritePathCubicGeometry( iCubicPath, Ar );
    } );
}
