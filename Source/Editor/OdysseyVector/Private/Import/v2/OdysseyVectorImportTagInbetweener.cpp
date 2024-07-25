#include "Import/v2/OdysseyVectorImport.h"
#include "Palette/OdysseyPalette.h"
#include "Engine/ObjectLibrary.h"
#include "AssetRegistry/AssetRegistryModule.h"

// from module OdysseyFile
#include "OdysseyFile.h"
#include "OdysseyVectorTagInbetweener.h"

void
FOdysseyVectorImportV2::ReadTagInbetweener( FOdysseyVectorTagInbetweener& iInbetweenerTag
                                          , uint64 iChunkEnd
                                          , FArchive &Ar )
{
    FOdysseyFile::ReadChunks( iChunkEnd
                            , Ar
                            , [ this
                              , &iInbetweenerTag ]( uint32 iChunkID
                                                  , uint64 iChunkLen
                                                  , FArchive &Ar ) -> void
        {
            switch( iChunkID )
            {
                case FOdysseyFile::VectorV2::CHUNK_TAGINBETWEENER_COLOR:
                {
                    uint8 r, g, b, a;

                    Ar << r;
                    Ar << g;
                    Ar << b;
                    Ar << a;

                    iInbetweenerTag.SetColor( r, g, b, a );
                }
                break;

                case FOdysseyFile::VectorV2::CHUNK_TAGINBETWEENER_INBETWEENCOUNT:
                {
                    uint32 inbetweenCount;

                    Ar << inbetweenCount;

                    iInbetweenerTag.SetInbetweenCount( inbetweenCount );
                    // allocating chart will alow us to read timing data
                    iInbetweenerTag.ResetChart();
                }
                break;

                case FOdysseyFile::VectorV2::CHUNK_TAGINBETWEENER_MAPASPOLYLINE:
                {
                    uint32 mapAsPolyline;

                    Ar << mapAsPolyline;

                    iInbetweenerTag.SetMapAsPolyline( mapAsPolyline ? true : false );
                }
                break;

                case FOdysseyFile::VectorV2::CHUNK_TAGINBETWEENER_CHART:
                {
                    for( uint32 i = 0; i < iInbetweenerTag.GetInbetweenCount(); i++ )
                    {
                        float spacing;

                        Ar << spacing;

                        iInbetweenerTag.GetChart().inbetweenBuffer[i].spacing = spacing;
                    }
                }
                break;

                case FOdysseyFile::VectorV2::CHUNK_TAGINBETWEENER_TRANSFORM:  // container
                break;

                case FOdysseyFile::VectorV2::CHUNK_TAGINBETWEENER_TRANSFORM_TRANSLATION:
                    double translationX;
                    double translationY;

                    Ar << translationX;
                    Ar << translationY;

                    iInbetweenerTag.Translate( translationX, translationY );
                    //iInbetweenerTag.UpdateMatrix();
                break;

                case FOdysseyFile::VectorV2::CHUNK_TAGINBETWEENER_TRANSFORM_ROTATION:
                    double rotation;

                    Ar << rotation;

                    iInbetweenerTag.Rotate( rotation );
                    //iInbetweenerTag.UpdateMatrix();
                break;

                case FOdysseyFile::VectorV2::CHUNK_TAGINBETWEENER_TRANSFORM_SCALING:
                    double scalingX;
                    double scalingY;

                    Ar << scalingX;
                    Ar << scalingY;

                    iInbetweenerTag.Scale( scalingX, scalingY );
                    //iInbetweenerTag.UpdateMatrix();
                break;

                case FOdysseyFile::VectorV2::CHUNK_TAGINBETWEENER_GRID:  // container
                break;

                case FOdysseyFile::VectorV2::CHUNK_TAGINBETWEENER_GRID_TYPE:
                    uint32 gridType;

                    Ar << gridType;

                    iInbetweenerTag.SetGrid( static_cast<eInbetweenerGridType>(gridType)
                                           , iInbetweenerTag.GetGridNumQuadX()
                                           , iInbetweenerTag.GetGridNumQuadY() );
                break;

                case FOdysseyFile::VectorV2::CHUNK_TAGINBETWEENER_GRID_INTERPOLATION:
                    uint32 interpolationType;

                    Ar << interpolationType;

                    iInbetweenerTag.SetInterpolationType( static_cast<eInbetweenerInterpolationType>(interpolationType) );
                break;

                // deprecated chunk
                case FOdysseyFile::VectorV2::CHUNK_TAGINBETWEENER_GRID_SIZE:
                {
                    uint32 numQuadX;
                    uint32 numQuadY;

                    Ar << numQuadX;
                    Ar << numQuadY;

                    iInbetweenerTag.SetGridNumQuad( numQuadX, numQuadY );
                }
                break;

                // deprecated chunk
/*------------------
                case FOdysseyFile::VectorV2::CHUNK_TAGINBETWEENER_GRID_GEOMETRY:
                {
                    std::vector<FInbetweenerPoint>& gridPointbuffer = iInbetweenerTag.GetGridPointBuffer();

                    for( FInbetweenerPoint& point : gridPointbuffer )
                    {
                        double sourceX;
                        double sourceY;
                        double targetX;
                        double targetY;

                        Ar << sourceX;
                        Ar << sourceY;
                        Ar << targetX;
                        Ar << targetY;

                        point.SetSourcePosition( sourceX, sourceY );
                        point.SetTargetPosition( targetX, targetY );
                    }
                }
                break;
*/
/*--------------------
                case FOdysseyFile::VectorV2::CHUNK_TAGINBETWEENER_GRID_GEOMETRY_MK2:
                {
                    std::vector<FInbetweenerPoint>& gridPointbuffer = iInbetweenerTag.GetGridPointBuffer();
                    std::vector<::ULIS::FVec2D> sourcePosition;
                    std::vector<::ULIS::FVec2D> targetPosition;
                    uint32 numQuadX;
                    uint32 numQuadY;

                    Ar << numQuadX;
                    Ar << numQuadY;

                    if( numQuadX && numQuadY )
                    {
                        uint32 pointCount = ( numQuadX + 1 ) * ( numQuadY + 1 );

                        sourcePosition.reserve( pointCount );
                        targetPosition.reserve( pointCount );

                        for( uint32 i = 0; i < pointCount; i++ )
                        {
                            double sourceX;
                            double sourceY;
                            double targetX;
                            double targetY;

                            Ar << sourceX;
                            Ar << sourceY;
                            Ar << targetX;
                            Ar << targetY;

                            sourcePosition.emplace_back( sourceX, sourceY );
                            targetPosition.emplace_back( targetX, targetY );
                        }
                    }

                    iInbetweenerTag.SetGridNumQuad( numQuadX
                                                  , numQuadY
                                                  , sourcePosition
                                                  , targetPosition );
                }
                break;

                case FOdysseyFile::VectorV2::CHUNK_TAGINBETWEENER_GRID_ARAP_RIGIDITY:
                {
                    FInbetweenerGridARAP* arapGrid = static_cast<FInbetweenerGridARAP*>(iInbetweenerTag.GetGrid());
                    uint32 rigidity;

                    Ar << rigidity;

                    arapGrid->SetRigidity( rigidity );
                }
                break;
*/
                case FOdysseyFile::VectorV2::CHUNK_TAGINBETWEENER_GRID_TRAJECTORIES: // container
                break;

/*-------------
                case FOdysseyFile::VectorV2::CHUNK_TRAJECTORY:
                {
                    FInbetweenerTrajectory* trajectory = new FInbetweenerTrajectory( &iInbetweenerTag
                                                                                   , 0
                                                                                   , 0.0f
                                                                                   , 0.0f );

                    iInbetweenerTag.AddTrajectory( trajectory );

                    ReadTrajectory( *trajectory
                                  , Ar.Tell() + iChunkLen
                                  , Ar );
                }
                break;
*/

                default:
                // Mandatory
                    Ar.Seek( Ar.Tell() + iChunkLen );
                break;
            }    
        } );
}
