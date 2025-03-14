// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "Import/v2/OdysseyVectorImport.h"
#include "Palette/OdysseyPalette.h"
#include "Engine/ObjectLibrary.h"
#include "AssetRegistry/AssetRegistryModule.h"

// from module OdysseyFile
#include "OdysseyFile.h"
#include "InbetweenerTag/InbetweenerTrajectory.h"
#include "InbetweenerTag/InbetweenerRoute.h"
#include "OdysseyVectorTagInbetweener.h"

void
FOdysseyVectorImportV2::ReadRoute( FInbetweenerRoute& iRoute
                                 , uint64 iChunkEnd
                                 , FArchive &Ar )
{
    FOdysseyFile::ReadChunks( iChunkEnd
                            , Ar
                            , [&iRoute](uint32 iChunkID, uint64 iChunkLen, FArchive &Ar) -> void
        {
            switch( iChunkID )
            {
                case FOdysseyFile::VectorV2::CHUNK_ROUTE_COORDS:
                {
                    uint32 quadID;
                    double quadU;
                    double quadV;

                    Ar << quadID;
                    Ar << quadU;
                    Ar << quadV;

                    iRoute.Init( quadID, quadU, quadV );
                }
                break;

                case FOdysseyFile::VectorV2::CHUNK_ROUTE_TRAJECTORIES:
                {
                    // Note: there are as many trajectories as breakdowns
                    for( FInbetweenerTrajectory& trajectory : iRoute.GetTrajectoryBuffer() )
                    {
                        double handle0DirX;
                        double handle0DirY;
                        double handle0LengthRatio;
                        double handle1DirX;
                        double handle1DirY;
                        double handle1LengthRatio;

                        Ar << handle0DirX;
                        Ar << handle0DirY;
                        Ar << handle0LengthRatio;
                        Ar << handle1DirX;
                        Ar << handle1DirY;
                        Ar << handle1LengthRatio;

                        trajectory.GetHandle(0)->Set( ::ULIS::FVec2D( handle0DirX, handle0DirY ), handle0LengthRatio );
                        trajectory.GetHandle(1)->Set( ::ULIS::FVec2D( handle1DirX, handle1DirY ), handle1LengthRatio );
                    }
                }
                break;

                case FOdysseyFile::VectorV2::CHUNK_ROUTE_TRAJECTORIES_WAYPOINTS:
                {
                    // Note: there are as many trajectories as breakdowns
                    for( FInbetweenerTrajectory& trajectory : iRoute.GetTrajectoryBuffer() )
                    {
                        std::vector<FInbetweenerWaypoint>& waypointBuffer = trajectory.GetWaypointBuffer();

                        for( uint32 i = 0; i < waypointBuffer.size(); i++ )
                        {
                            float ratio;

                            Ar << ratio;

                            waypointBuffer[i].SetRatio( ratio );
                        }
                    }
                }
                break;

                default:
                // Mandatory
                    Ar.Seek( Ar.Tell() + iChunkLen );
                break;
            }
        } );
}
