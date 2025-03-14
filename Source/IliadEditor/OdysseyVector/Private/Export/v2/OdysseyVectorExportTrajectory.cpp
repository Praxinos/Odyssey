// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "Export/v2/OdysseyVectorExport.h"
#include "Palette/OdysseyPaletteEntry.h"
// from module OdysseyFile
#include "OdysseyFile.h"
#include "InbetweenerTag/InbetweenerPoint.h"
#include "InbetweenerTag/InbetweenerTrajectory.h"
#include "InbetweenerTag/InbetweenerHandleTrajectory.h"
#include "InbetweenerTag/InbetweenerGrid.h"
#include "OdysseyVectorTagInbetweener.h"

#ifdef unused
void
FOdysseyVectorExportV2::WriteTrajectoryGeometry( FInbetweenerTrajectory& iTrajectory
                                               , FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_TRAJECTORY_GEOMETRY
                            , Ar
                            , [&iTrajectory](FArchive &Ar) -> void
    {
        FInbetweenerHandleTrajectory* handle0 = iTrajectory.GetHandle(0);
        FInbetweenerHandleTrajectory* handle1 = iTrajectory.GetHandle(1);
        double handle0DirX = handle0->GetDirection().x;
        double handle0DirY = handle0->GetDirection().y;
        double handle0LengthRatio = handle0->GetLengthRatio();
        double handle1DirX = handle1->GetDirection().x;
        double handle1DirY = handle1->GetDirection().y;
        double handle1LengthRatio = handle1->GetLengthRatio();

        Ar << handle0DirX;
        Ar << handle0DirY;
        Ar << handle0LengthRatio;
        Ar << handle1DirX;
        Ar << handle1DirY;
        Ar << handle1LengthRatio;
    } );
}

void
FOdysseyVectorExportV2::WriteTrajectoryCoords( FInbetweenerTrajectory& iTrajectory
                                             , FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_TRAJECTORY_COORDS
                            , Ar
                            , [&iTrajectory](FArchive &Ar) -> void
    {
/*--------------
        std::vector<FInbetweenerQuad>& quadBuffer = iTrajectory.GetInbetweenerTag()->GetGrid()->GetQuadBuffer();
        uint32 quadIndex = iTrajectory.GetQuadIndex();
        double quadU = iTrajectory.GetQuadU();
        double quadV = iTrajectory.GetQuadV();

        Ar << quadIndex;
        Ar << quadU;
        Ar << quadV;
*/
    } );
}

void
FOdysseyVectorExportV2::WriteTrajectoryWaypointsRatio( FInbetweenerTrajectory& iTrajectory
                                                     , FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_TRAJECTORY_WAYPOINTS_RATIO
                            , Ar
                            , [&iTrajectory](FArchive &Ar) -> void
    {
/*-----------
        uint32 inbetweenCount = iTrajectory.GetInbetweenerTag()->GetInbetweenCount();
        std::vector<FInbetweenerWaypoint>& waypointBuffer = iTrajectory.GetWaypointBuffer();

        for( uint32 i = 0; i < inbetweenCount; i++ )
        {
            float ratio = waypointBuffer[i].GetRatio();

            Ar << ratio;
        }
*/
    } );
}

void
FOdysseyVectorExportV2::WriteTrajectoryWaypoints( FInbetweenerTrajectory& iTrajectory
                                                , FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_TRAJECTORY_WAYPOINTS
                            , Ar
                            , [&iTrajectory](FArchive &Ar) -> void
    {
        WriteTrajectoryWaypointsRatio( iTrajectory, Ar );
    } );
}

void
FOdysseyVectorExportV2::WriteTrajectory( FInbetweenerTrajectory& iTrajectory
                                       , FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_TRAJECTORY
                            , Ar
                            , [&iTrajectory](FArchive &Ar) -> void
    {
        WriteTrajectoryCoords( iTrajectory, Ar );
        WriteTrajectoryGeometry( iTrajectory, Ar );
        WriteTrajectoryWaypoints( iTrajectory, Ar );
    } );
}
#endif
