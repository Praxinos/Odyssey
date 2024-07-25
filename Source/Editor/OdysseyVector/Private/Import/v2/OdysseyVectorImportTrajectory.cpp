#include "Import/v2/OdysseyVectorImport.h"
#include "Palette/OdysseyPalette.h"
#include "Engine/ObjectLibrary.h"
#include "AssetRegistry/AssetRegistryModule.h"

// from module OdysseyFile
#include "OdysseyFile.h"
#include "OdysseyVectorTagInbetweener.h"

void
FOdysseyVectorImportV2::ReadTrajectory( FInbetweenerTrajectory& iTrajectory
                                      , uint64 iChunkEnd
                                      , FArchive &Ar )
{
    FOdysseyFile::ReadChunks( iChunkEnd
                            , Ar
                            , [&iTrajectory](uint32 iChunkID, uint64 iChunkLen, FArchive &Ar) -> void
        {
            switch( iChunkID )
            {
                case FOdysseyFile::VectorV2::CHUNK_TRAJECTORY_WAYPOINTS:
                // container
                break;

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
/*------------------
                case FOdysseyFile::VectorV2::CHUNK_TRAJECTORY_COORDS:
                {
                    std::vector<FInbetweenerQuad>& quadBuffer =  iTrajectory.GetInbetweenerTag()->GetGrid()->GetQuadBuffer();
                    uint32 quadID;
                    double quadU;
                    double quadV;

                    Ar << quadID;
                    Ar << quadU;
                    Ar << quadV;

                    iTrajectory.Init( quadID, quadU, quadV );
                }
                break;
*/
                case FOdysseyFile::VectorV2::CHUNK_TRAJECTORY_GEOMETRY:
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

                    iTrajectory.GetHandle(0)->Set( ::ULIS::FVec2D( handle0DirX, handle0DirY ), handle0LengthRatio );
                    iTrajectory.GetHandle(1)->Set( ::ULIS::FVec2D( handle1DirX, handle1DirY ), handle1LengthRatio );
                }
                break;

                default:
                // Mandatory
                    Ar.Seek( Ar.Tell() + iChunkLen );
                break;
            }    
        } );
}
