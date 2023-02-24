#include "Import/OdysseyVectorImport.h"

static void
ReadPathCubicGeometrySegments( UOdysseyVectorPathCubic& iCubicPath
                             , std::vector<UOdysseyVectorVertexCubic*>& vertexArray
                             , FArchive &Ar )
{
    uint32 segmentCount;

    Ar << segmentCount;

    for( uint32 i = 0; i < segmentCount; i++ )
    {
        UOdysseyVectorSegmentCubic* cubicSegment;
        uint32 p0ID;
        uint32 p1ID;
        double ctrlPoint0X;
        double ctrlPoint0Y;
        double ctrlPoint1X;
        double ctrlPoint1Y;

        Ar << p0ID;
        Ar << p1ID;
        Ar << ctrlPoint0X;
        Ar << ctrlPoint0Y;
        Ar << ctrlPoint1X;
        Ar << ctrlPoint1Y;

        cubicSegment = UOdysseyVectorSegmentCubic::New( &iCubicPath
                                                      , vertexArray[p0ID]
                                                      , ctrlPoint0X
                                                      , ctrlPoint0Y
                                                      , ctrlPoint1X
                                                      , ctrlPoint1Y
                                                      , vertexArray[p1ID] );

        iCubicPath.AddSegment( cubicSegment );

        // update bounding box
        /*cubicSegment->Update();*/
    }
}

static void
ReadPathCubicGeometryVertices( UOdysseyVectorPathCubic& iCubicPath
                             , std::vector<UOdysseyVectorVertexCubic*>& vertexArray
                             , FArchive &Ar )
{
    uint32 vertexCount;

    Ar << vertexCount;

    for( uint32 i = 0; i < vertexCount; i++ )
    {
        UOdysseyVectorVertexCubic* cubicVertex;
        double x;
        double y;
        double radius;

        Ar << x;
        Ar << y;
        Ar << radius;

        cubicVertex = UOdysseyVectorVertexCubic::New( x, y, radius );

        iCubicPath.AddVertex( cubicVertex );

        // indexation required for creating segments
        vertexArray.push_back( cubicVertex );
    }
}

void
FOdysseyVectorImport::ReadObjectPathCubic( UOdysseyVectorPathCubic& iCubicPath, uint64 iChunkEnd, FArchive &Ar )
{
    std::vector<UOdysseyVectorVertexCubic*> vertexArray;

    FOdysseyVectorImport::ReadChunks( iChunkEnd
                                    , Ar
                                    , [&iCubicPath, &vertexArray](uint32 iChunkID, uint64 iChunkLen, FArchive &Ar) -> void
        {
            switch( iChunkID )
            {
                case FOdysseyVectorExport::CHUNK_PATHCUBIC_GEOMETRY:
                break;

                case FOdysseyVectorExport::CHUNK_PATHCUBIC_GEOMETRY_VERTICES:
                    ReadPathCubicGeometryVertices( iCubicPath, vertexArray, Ar );
                break;

                case FOdysseyVectorExport::CHUNK_PATHCUBIC_GEOMETRY_SEGMENTS:
                    ReadPathCubicGeometrySegments( iCubicPath, vertexArray, Ar );
                break;

                default:
                // Mandatory
                    Ar.Seek( Ar.Tell() + iChunkLen );
                break;
            }    
        } );
}
