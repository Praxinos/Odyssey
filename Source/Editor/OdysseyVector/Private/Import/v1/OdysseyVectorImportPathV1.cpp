#include "Import/v1/OdysseyVectorImport.h"
// from module OdysseyFile
#include "OdysseyFile.h"
#include "OdysseyVectorVertex.h"
#include "OdysseyVectorSegment.h"
#include "OdysseyVectorSegmentCubic.h"
#include "OdysseyVectorPath.h"

void
FOdysseyVectorImportV1::ReadPathGeometryCubicSegments( FOdysseyVectorPath& iPath
                                                     , std::vector<FOdysseyVectorVertex*>& vertexArray
                                                     , FArchive &Ar )
{
    uint32 segmentCount;

    Ar << segmentCount;

    for( uint32 i = 0; i < segmentCount; i++ )
    {
        FOdysseyVectorSegmentCubic* cubicSegment;
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

        cubicSegment = new FOdysseyVectorSegmentCubic ( &iPath
                                                       , vertexArray[p0ID]
                                                       , ctrlPoint0X
                                                       , ctrlPoint0Y
                                                       , ctrlPoint1X
                                                       , ctrlPoint1Y
                                                       , vertexArray[p1ID]
                                                       , true );

        iPath.AddSegment( cubicSegment );

        // update bounding box
        /*cubicSegment->Update();*/
    }
}

void
FOdysseyVectorImportV1::ReadPathGeometryVertices( FOdysseyVectorPath& iPath
                                                , std::vector<FOdysseyVectorVertex*>& vertexArray
                                                , FArchive &Ar )
{
    uint32 vertexCount;

    Ar << vertexCount;

    for( uint32 i = 0; i < vertexCount; i++ )
    {
        FOdysseyVectorVertex* cubicVertex;
        double x;
        double y;
        double radius;

        Ar << x;
        Ar << y;
        Ar << radius;

        cubicVertex = new FOdysseyVectorVertex( x, y, radius );

        iPath.AddVertex( cubicVertex );

        // indexation required for creating segments
        vertexArray.push_back( cubicVertex );
    }
}

void
FOdysseyVectorImportV1::ReadPath( FOdysseyVectorPath& iPath, uint64 iChunkEnd, FArchive &Ar )
{
    std::vector<FOdysseyVectorVertex*> vertexArray;

    FOdysseyFile::ReadChunks( iChunkEnd
                            , Ar
                            , [&iPath, &vertexArray](uint32 iChunkID, uint64 iChunkLen, FArchive &Ar) -> void
        {
            switch( iChunkID )
            {
                case FOdysseyFile::VectorV1::CHUNK_PATH_JOINT:
                {
                    uint32 jointType;

                    Ar << jointType;

                    iPath.SetJointType(static_cast<eJointType>(jointType), true );
                }
                break;

                case FOdysseyFile::VectorV1::CHUNK_PATH_GEOMETRY:
                break;

                case FOdysseyFile::VectorV1::CHUNK_PATH_GEOMETRY_VERTICES:
                    ReadPathGeometryVertices( iPath, vertexArray, Ar );
                break;

                case FOdysseyFile::VectorV1::CHUNK_PATH_GEOMETRY_CUBICSEGMENTS:
                    ReadPathGeometryCubicSegments( iPath, vertexArray, Ar );
                break;

                default:
                // Mandatory
                    Ar.Seek( Ar.Tell() + iChunkLen );
                break;
            }
        } );
}
