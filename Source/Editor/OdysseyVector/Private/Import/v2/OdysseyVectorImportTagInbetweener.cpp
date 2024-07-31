#include "Import/v2/OdysseyVectorImport.h"
#include "Palette/OdysseyPalette.h"
#include "Engine/ObjectLibrary.h"
#include "AssetRegistry/AssetRegistryModule.h"

// from module OdysseyFile
#include "OdysseyFile.h"
#include "InbetweenerTag/InbetweenerRoute.h"
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

                    iInbetweenerTag.SetDrawingCount( inbetweenCount );
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
                    for( uint32 i = 0; i < iInbetweenerTag.GetDrawingCount(); i++ )
                    {
                        float spacing;

                        Ar << spacing;

                        iInbetweenerTag.GetChart().drawingBuffer[i].spacing = spacing;
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

                case FOdysseyFile::VectorV2::CHUNK_TAGINBETWEENER_DEFORMATION:
                {
                    uint32 gridType;

                    Ar << gridType;

                    iInbetweenerTag.SetGrid( static_cast<eInbetweenerGridType>(gridType)
                                           , iInbetweenerTag.GetGridNumQuadX()
                                           , iInbetweenerTag.GetGridNumQuadY() );
                }
                break;

                case FOdysseyFile::VectorV2::CHUNK_TAGINBETWEENER_INTERPOLATION:
                {
                    uint32 interpolationType;

                    Ar << interpolationType;

                    iInbetweenerTag.SetInterpolationType( static_cast<eInbetweenerInterpolationType>(interpolationType) );
                }
                break;

                case FOdysseyFile::VectorV2::CHUNK_TAGINBETWEENER_DIMENSION:
                {
                    uint32 numQuadX;
                    uint32 numQuadY;

                    Ar << numQuadX;
                    Ar << numQuadY;

                    iInbetweenerTag.SetGridNumQuad( numQuadX, numQuadY );
                }
                break;

                case FOdysseyFile::VectorV2::CHUNK_TAGINBETWEENER_ARAPRIGIDITY:
                {
                    uint32 arapRigidity;

                    Ar << arapRigidity;

                    iInbetweenerTag.SetARAPRigidity( arapRigidity );
                }
                break;

                case FOdysseyFile::VectorV2::CHUNK_TAGINBETWEENER_BREAKDOWNS: // container
                break;

                case FOdysseyFile::VectorV2::CHUNK_TAGINBETWEENER_BREAKDOWNS_LAYOUT: // container
                {
                    FInbetweenerBreakdown* masterBreakdown = iInbetweenerTag.GetMasterBreakdown();
                    uint32 breakdownCount;

                    Ar << breakdownCount;

                    for( uint32 i = 0; i < breakdownCount; i++ )
                    {
                        FInbetweenerBreakdown* breakdown;
                        uint32 master;
                        uint32 sourceDrawingIndex;
                        uint32 targetDrawingIndex;

                        Ar << master;
                        Ar << sourceDrawingIndex; // actually unneeded
                        Ar << targetDrawingIndex;

                        breakdown = ( master ) ? masterBreakdown
                                               : new FInbetweenerBreakdown( &iInbetweenerTag
                                                                          , masterBreakdown
                                                                          , sourceDrawingIndex
                                                                          , targetDrawingIndex );

                        if( master == 0 ) 
                        {
                            iInbetweenerTag.AddBreakdown( breakdown, targetDrawingIndex, false );
                        }
                        else
                        {
                            breakdown->SetSourceDrawingIndex( sourceDrawingIndex );
                            breakdown->SetTargetDrawingIndex( targetDrawingIndex );
                        }
                    }
                }
                break;

                case FOdysseyFile::VectorV2::CHUNK_TAGINBETWEENER_BREAKDOWNS_GRIDGEOMETRY:
                {
                    uint32 numQuadX = iInbetweenerTag.GetGridNumQuadX();
                    uint32 numQuadY = iInbetweenerTag.GetGridNumQuadY();

                    for( FInbetweenerBreakdown* breakdown : iInbetweenerTag.GetBreakdownList() )
                    {
                        std::vector<FInbetweenerPoint>& gridPointbuffer = breakdown->GetGrid()->GetPointBuffer();
                        std::vector<::ULIS::FVec2D> sourcePosition;
                        std::vector<::ULIS::FVec2D> targetPosition;

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

                        breakdown->GetGrid()->SetGeometry( sourcePosition, eInbetweenerPointPositionType::SourcePosition );
                        breakdown->GetGrid()->SetGeometry( targetPosition, eInbetweenerPointPositionType::TargetPosition );
                    }
                }
                break;

                case FOdysseyFile::VectorV2::CHUNK_TAGINBETWEENER_ROUTES:
                break;

                case FOdysseyFile::VectorV2::CHUNK_ROUTE:
                {
                    FInbetweenerRoute* route = new FInbetweenerRoute( &iInbetweenerTag
                                                                     , 0
                                                                     , 0.0f
                                                                     , 0.0f );

                    iInbetweenerTag.AddRoute( route );

                    ReadRoute( *route
                             , Ar.Tell() + iChunkLen
                             , Ar );
                }
                break;

                // deprecated chunk
/*------------------
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
*/
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
/*-------------
                case FOdysseyFile::VectorV2::CHUNK_TAGINBETWEENER_GRID_TRAJECTORIES: // container
                break;
*/
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
