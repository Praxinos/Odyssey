#include "Import/v2/OdysseyVectorImport.h"
// from module OdysseyFile
#include "OdysseyFile.h"

void
FOdysseyVectorImportV2::ReadPathGeometryCubicSegments( FOdysseyVectorPath& iPath
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
FOdysseyVectorImportV2::ReadPathGeometryVertexLock( FOdysseyVectorVertex& iVertex
                                                  , FArchive &Ar )
{
    uint32 lock;

    Ar << lock;

    iVertex.SetLocked( lock ? true : false );
}

void
FOdysseyVectorImportV2::ReadPathGeometryVertexHandleAlignment( FOdysseyVectorVertex& iVertex
                                                             , FArchive &Ar )
{
    uint32 handleAlignment;

    Ar << handleAlignment;

    iVertex.SetHandleAligned( handleAlignment ? true : false );
}

void
FOdysseyVectorImportV2::ReadPathGeometryVertexPosition( FOdysseyVectorVertex& iVertex
                                                      , FArchive &Ar )
{
    double x;
    double y;
    double radius;

    Ar << x;
    Ar << y;
    Ar << radius;

    iVertex.Set( x, y );
    iVertex.SetRadius( radius );
}

void
FOdysseyVectorImportV2::ReadPathGeometryVertices( FOdysseyVectorPath& iPath
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
FOdysseyVectorImportV2::ReadPath( FOdysseyVectorPath& iPath, uint64 iChunkEnd, FArchive &Ar )
{
    std::vector<FOdysseyVectorVertex*> vertexArray;
    FOdysseyVectorVertex* currentVertex;
    FOdysseyVectorBrush* currentBrush;

    FOdysseyFile::ReadChunks( iChunkEnd
                            , Ar
                            , [ this
                              , &iPath
                              , &vertexArray
                              , &currentVertex
                              , &currentBrush ](uint32 iChunkID, uint64 iChunkLen, FArchive &Ar) -> void
        {
            switch( iChunkID )
            {
                case FOdysseyFile::VectorV2::CHUNK_PATH_JOINT:
                {
                    uint32 jointType;

                    Ar << jointType;

                    iPath.SetJointType(static_cast<eJointType>(jointType), true);
                }
                break;

                case FOdysseyFile::VectorV2::CHUNK_PATH_MITERLIMIT:
                {
                    double miterLimit;

                    Ar << miterLimit;

                    iPath.SetMiterLimit( miterLimit, true );
                }
                break;

                case FOdysseyFile::VectorV2::CHUNK_PATH_BRUSH:
                    currentBrush = &iPath.GetBrush();
                break;

                case FOdysseyFile::VectorV2::CHUNK_BRUSH:
                    ReadBrush( *currentBrush, Ar.Tell() + iChunkLen, Ar );
                break;

                case FOdysseyFile::VectorV2::CHUNK_PATH_GEOMETRY:
                break;

                case FOdysseyFile::VectorV2::CHUNK_PATH_GEOMETRY_VERTEX:
                {
                    currentVertex = new FOdysseyVectorVertex( 0.0f, 0.0f, 0.0f );

                    iPath.AddVertex( currentVertex );
                    // indexation required for creating segments
                    vertexArray.push_back( currentVertex );
                }
                break;

                case FOdysseyFile::VectorV2::CHUNK_PATH_GEOMETRY_VERTEX_POSITION:
                    ReadPathGeometryVertexPosition( *currentVertex, Ar );
                break;

                case FOdysseyFile::VectorV2::CHUNK_PATH_GEOMETRY_VERTEX_HANDLEALIGNMENT:
                    ReadPathGeometryVertexHandleAlignment( *currentVertex, Ar );
                break;

                case FOdysseyFile::VectorV2::CHUNK_PATH_GEOMETRY_VERTEX_LOCK:
                    ReadPathGeometryVertexLock( *currentVertex, Ar );
                break;

                case FOdysseyFile::VectorV2::CHUNK_PATH_GEOMETRY_VERTICES:
                    ReadPathGeometryVertices( iPath, vertexArray, Ar );
                break;

                case FOdysseyFile::VectorV2::CHUNK_PATH_GEOMETRY_CUBICSEGMENTS:
                    ReadPathGeometryCubicSegments( iPath, vertexArray, Ar );
                break;

                default:
                    FOdysseyVectorImportV2::ParseObjectChunks( iPath, iChunkID, iChunkLen, Ar );
                break;
            }
        } );
}
