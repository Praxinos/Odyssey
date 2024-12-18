// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "Export/v2/OdysseyVectorExport.h"
#include "Palette/OdysseyPaletteEntry.h"
// from module OdysseyFile
#include "OdysseyFile.h"
#include "InbetweenerTag/InbetweenerPoint.h"
#include "InbetweenerTag/InbetweenerTrajectory.h"
#include "InbetweenerTag/InbetweenerRoute.h"
#include "InbetweenerTag/InbetweenerHandleTrajectory.h"
#include "InbetweenerTag/InbetweenerGrid.h"
#include "OdysseyVectorTagInbetweener.h"

void
FOdysseyVectorExportV2::WriteRouteTrajectoriesWaypoints( FInbetweenerRoute& iRoute
                                                       , FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_ROUTE_TRAJECTORIES_WAYPOINTS
                            , Ar
                            , [&iRoute](FArchive &Ar) -> void
    {
        // Note: there are as many trajectories as breakdowns
        for( FInbetweenerTrajectory& trajectory : iRoute.GetTrajectoryBuffer() )
        {
            std::vector<FInbetweenerWaypoint>& waypointBuffer = trajectory.GetWaypointBuffer();

            for( uint32 i = 0; i < waypointBuffer.size(); i++ )
            {
                float ratio = waypointBuffer[i].GetRatio();

                Ar << ratio;
            }
        }
    } );
}

void
FOdysseyVectorExportV2::WriteRouteTrajectories( FInbetweenerRoute& iRoute
                                              , FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_ROUTE_TRAJECTORIES
                            , Ar
                            , [&iRoute](FArchive &Ar) -> void
    {
        // Note: there are as many trajectories as breakdowns
        for( FInbetweenerTrajectory& trajectory : iRoute.GetTrajectoryBuffer() )
        {
            FInbetweenerHandleTrajectory* handle0 = trajectory.GetHandle(0);
            FInbetweenerHandleTrajectory* handle1 = trajectory.GetHandle(1);
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
        }
    } );
}

void
FOdysseyVectorExportV2::WriteRouteCoords( FInbetweenerRoute& iRoute
                                        , FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_ROUTE_COORDS
                            , Ar
                            , [&iRoute](FArchive &Ar) -> void
    {
        uint32 quadIndex = iRoute.GetQuadIndex();
        double quadU = iRoute.GetQuadU();
        double quadV = iRoute.GetQuadV();

        Ar << quadIndex;
        Ar << quadU;
        Ar << quadV;
    } );
}

void
FOdysseyVectorExportV2::WriteRoute( FInbetweenerRoute& iRoute
                                  , FArchive &Ar )
{
    FOdysseyFile::WriteChunk( FOdysseyFile::VectorV2::CHUNK_ROUTE
                            , Ar
                            , [&iRoute](FArchive &Ar) -> void
    {
        WriteRouteCoords( iRoute, Ar );
        WriteRouteTrajectories( iRoute, Ar );
        WriteRouteTrajectoriesWaypoints( iRoute, Ar );
    } );
}
