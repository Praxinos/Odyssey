#include "Export/v1/OdysseyVectorExport.h"
#include "OdysseyVectorSegmentCubic.h"
// from module OdysseyFile
#include "OdysseyFile.h"

void
FOdysseyVectorExportV1::WritePathCubicGeometrySegments( FOdysseyVectorPath& iPath, FArchive &Ar )
{
    if( iPath.GetSegmentList().size() )
    {
        FOdysseyFile::WriteChunk( FOdysseyFile::VectorV1::CHUNK_PATH_GEOMETRY_CUBICSEGMENTS
                                , Ar
                                , [&iPath](FArchive &Ar) -> void
        {
            uint32 segmentCount = iPath.GetSegmentList().size();

            Ar << segmentCount;

            for( std::list<FOdysseyVectorSegment*>::iterator it = iPath.GetSegmentList().begin(); it != iPath.GetSegmentList().end(); ++it )
            {
                FOdysseyVectorSegmentCubic* cubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(*it);
                uint32 p0ID = cubicSegment->GetPoint(0)->GetID();
                uint32 p1ID = cubicSegment->GetPoint(1)->GetID();
                ::ULIS::FVec2D& ctrlPoint0 = cubicSegment->GetHandle(0)->GetCoords();
                ::ULIS::FVec2D& ctrlPoint1 = cubicSegment->GetHandle(1)->GetCoords();
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

void
FOdysseyVectorExportV1::WritePathCubicGeometryVertices( FOdysseyVectorPath& iPath, FArchive &Ar )
{
    if ( iPath.GetVertexList().size() )
    {
        FOdysseyFile::WriteChunk( FOdysseyFile::VectorV1::CHUNK_PATH_GEOMETRY_VERTICES
                                , Ar
                                , [&iPath](FArchive &Ar) -> void
        {
            uint32 vertexID = 0;
            uint32 vertexCount = iPath.GetVertexList().size();

            Ar << vertexCount;

            for( std::list<FOdysseyVectorVertex*>::iterator it = iPath.GetVertexList().begin(); it != iPath.GetVertexList().end(); ++it )
            {
                FOdysseyVectorVertex* vertex = static_cast<FOdysseyVectorVertex*>(*it);
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

void
FOdysseyVectorExportV1::WritePathCubicGeometry( FOdysseyVectorPath& iPath, FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV1::CHUNK_PATH_GEOMETRY
                            , Ar
                            , [&iPath](FArchive &Ar) -> void
    {
        WritePathCubicGeometryVertices( iPath, Ar );
        WritePathCubicGeometrySegments( iPath, Ar );
    } );
}

void
FOdysseyVectorExportV1::WritePathJoint( FOdysseyVectorPath& iPath, FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV1::CHUNK_PATH_JOINT
                            , Ar
                            , [&iPath](FArchive &Ar) -> void
    {
        uint32 jointType = static_cast<uint32>(iPath.GetJointType());

        Ar << jointType;
    } );
}

void
FOdysseyVectorExportV1::WritePath( FOdysseyVectorPath& iPath, FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV1::CHUNK_OBJECT_PATH
                            , Ar
                            , [&iPath](FArchive &Ar) -> void
    {
        WritePathJoint( iPath, Ar );
        WritePathCubicGeometry( iPath, Ar );
    } );
}
