// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Export/v2/OdysseyVectorExport.h"
#include "OdysseyVectorSegmentCubic.h"
// from module OdysseyFile
#include "OdysseyFile.h"

#include "OdysseyVectorVertex.h"
#include "OdysseyVectorSegment.h"
#include "OdysseyVectorObject.h"
#include "OdysseyVectorPath.h"

//static
void
FOdysseyVectorExportV2::WritePathGeometrySegments( FOdysseyVectorPath& iPath, FArchive &Ar )
{
    if( iPath.GetSegmentList().size() )
    {
        FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_PATH_GEOMETRY_CUBICSEGMENTS
                                , Ar
                                , [&iPath](FArchive &Ar) -> void
        {
            uint32 cubicSegmentCount = 0;

            // filter cubic segments
            for( FOdysseyVectorSegment* segment : iPath.GetSegmentList() )
            {
                if( segment->HasBaseClass( FOdysseyVectorSegmentCubic::StaticClass() ) )
                {
                    cubicSegmentCount++;
                }
            }

            // write count
            Ar << cubicSegmentCount;

            for( FOdysseyVectorSegment* segment : iPath.GetSegmentList() )
            {
                if( segment->HasBaseClass( FOdysseyVectorSegmentCubic::StaticClass() ) )
                {
                    FOdysseyVectorSegmentCubic* cubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(segment);

                    uint32 p0ID = cubicSegment->GetVertex(0)->GetID();
                    uint32 p1ID = cubicSegment->GetVertex(1)->GetID();
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
            }
        } );
    }
}

//static
void
FOdysseyVectorExportV2::WritePathGeometryVertexLock( FOdysseyVectorVertex& iVertex, FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_PATH_GEOMETRY_VERTEX_LOCK
                            , Ar
                            , [&iVertex](FArchive &Ar) -> void
    {
        uint32 isLocked = static_cast<uint32>(iVertex.IsLocked());

        Ar << isLocked;
    } );
}

//static
void
FOdysseyVectorExportV2::WritePathGeometryVertexHandleAlignment( FOdysseyVectorVertex& iVertex, FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_PATH_GEOMETRY_VERTEX_HANDLEALIGNMENT
                            , Ar
                            , [&iVertex](FArchive &Ar) -> void
    {
        uint32 isHandleAligned = static_cast<uint32>(iVertex.IsHandleAligned());

        Ar << isHandleAligned;
    } );
}

//static
void
FOdysseyVectorExportV2::WritePathGeometryVertexPosition( FOdysseyVectorVertex& iVertex, FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_PATH_GEOMETRY_VERTEX_POSITION
                            , Ar
                            , [&iVertex](FArchive &Ar) -> void
    {
        double x = iVertex.GetX();
        double y = iVertex.GetY();
        double radius = iVertex.GetRadius();

        Ar << x;
        Ar << y;
        Ar << radius;
    } );
}

//static
void
FOdysseyVectorExportV2::WritePathGeometryVertex( FOdysseyVectorVertex& iVertex, FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_PATH_GEOMETRY_VERTEX
                            , Ar
                            , [&iVertex](FArchive &Ar) -> void
    {
        WritePathGeometryVertexPosition( iVertex, Ar );
        WritePathGeometryVertexHandleAlignment( iVertex, Ar );
        WritePathGeometryVertexLock( iVertex, Ar );
    } );
}

//static
void
FOdysseyVectorExportV2::WritePathGeometryVertices( FOdysseyVectorPath& iPath, FArchive &Ar )
{
    if ( iPath.GetVertexList().size() )
    {
        FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_PATH_GEOMETRY_VERTICES
                                , Ar
                                , [&iPath](FArchive &Ar) -> void
        {
            uint32 vertexCount = iPath.GetVertexList().size();

            Ar << vertexCount;

            for( FOdysseyVectorVertex* vertex : iPath.GetVertexList() )
            {
                double x = vertex->GetX();
                double y = vertex->GetY();
                double radius = vertex->GetRadius();

                Ar << x;
                Ar << y;
                Ar << radius;
            }
        } );
    }
}

//static
void
FOdysseyVectorExportV2::WritePathGeometry( FOdysseyVectorPath& iPath, FArchive &Ar )
{
    uint32 vertexID = 0;

    // renumber vertices, for use as a reference when writing segments e.g
    for( FOdysseyVectorVertex* vertex : iPath.GetVertexList() )
    {
        vertex->SetID( vertexID++ );
    }

    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_PATH_GEOMETRY
                            , Ar
                            , [&iPath](FArchive &Ar) -> void
    {
        //WritePathGeometryVertices_Packed( iPath, Ar );
        for( FOdysseyVectorVertex* vertex : iPath.GetVertexList() )
        {
            WritePathGeometryVertex( *vertex, Ar );
        }

        WritePathGeometrySegments( iPath, Ar );
    } );
}

//static
void
FOdysseyVectorExportV2::WritePathJoint( FOdysseyVectorPath& iPath, FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_PATH_JOINT
                            , Ar
                            , [&iPath](FArchive &Ar) -> void
    {
        uint32 jointType = static_cast<uint32>(iPath.GetJointType());

        Ar << jointType;
    } );
}

//static
void
FOdysseyVectorExportV2::WritePathMiterLimit( FOdysseyVectorPath& iPath, FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_PATH_MITERLIMIT
                            , Ar
                            , [&iPath](FArchive &Ar) -> void
    {
        double miterLimit = iPath.GetMiterLimit();

        Ar << miterLimit;
    } );
}

//static
void
FOdysseyVectorExportV2::WritePathBrush( FOdysseyVectorPath& iPath, FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_PATH_BRUSH
                            , Ar
                            , [&iPath](FArchive &Ar) -> void
    {
        FOdysseyVectorExportV2::WriteBrush( iPath.GetBrush(), Ar );
    } );
}

//static
void
FOdysseyVectorExportV2::WritePath( FOdysseyVectorPath& iPath, FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_PATH
                            , Ar
                            , [&iPath](FArchive &Ar) -> void
    {
        // inherited chunks
        WriteObjectChunks( iPath, Ar );

        // own chunks
        WritePathJoint( iPath, Ar );
        WritePathMiterLimit( iPath, Ar );
        WritePathGeometry( iPath, Ar );
        WritePathBrush( iPath, Ar );
    } );
}
