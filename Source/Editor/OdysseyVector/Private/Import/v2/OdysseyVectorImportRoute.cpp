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
/*-----------------
                case FOdysseyFile::VectorV2::CHUNK_TRAJECTORY_WAYPOINTS:
                // container
                break;
*/
/*----------------
                case FOdysseyFile::VectorV2::CHUNK_TRAJECTORY_WAYPOINTS_RATIO:
                {
                    uint32 inbetweenCount = iTrajectory.GetInbetweenerTag()->GetInbetweenCount();
                    std::vector<FInbetweenerWaypoint>& waypointBuffer = iTrajectory.GetWaypointBuffer();

                    for( uint32 i = 0; i < inbetweenCount; i++ )
                    {
                        float ratio;

                        Ar << ratio;

                        waypointBuffer[i].SetRatio( ratio );
                    }
                }
                break;
*/

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
                    for( FInbetweenerBreakdown* breakdown : iRoute.GetInbetweenerTag()->GetBreakdownList() )
                    {
                        uint32 breakdownIndex = breakdown->GetIndex();
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

                        iRoute.GetTrajectoryBuffer()[breakdownIndex].GetHandle(0)->Set( ::ULIS::FVec2D( handle0DirX, handle0DirY ), handle0LengthRatio );
                        iRoute.GetTrajectoryBuffer()[breakdownIndex].GetHandle(1)->Set( ::ULIS::FVec2D( handle1DirX, handle1DirY ), handle1LengthRatio );
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
