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
                case FOdysseyFile::VectorV2::CHUNK_TAGINBETWEENER_INBETWEENCOLOR:
                {
                    uint8 r, g, b, a;

                    Ar << r;
                    Ar << g;
                    Ar << b;
                    Ar << a;

                    iInbetweenerTag.SetInbetweenColor( r, g, b, a );
                }
                break;

                case FOdysseyFile::VectorV2::CHUNK_TAGINBETWEENER_CHARTCOLOR:
                {
                    uint8 r, g, b, a;

                    Ar << r;
                    Ar << g;
                    Ar << b;
                    Ar << a;

                    iInbetweenerTag.SetChartColor( FColor( r, g, b, a ) );
                }
                break;

                case FOdysseyFile::VectorV2::CHUNK_TAGINBETWEENER_GRIDCOLOR:
                {
                    uint8 r, g, b, a;

                    Ar << r;
                    Ar << g;
                    Ar << b;
                    Ar << a;

                    iInbetweenerTag.SetGridColor( FColor( r, g, b, a ) );
                }
                break;

                case FOdysseyFile::VectorV2::CHUNK_TAGINBETWEENER_TRAJECTORYCOLOR:
                {
                    uint8 r, g, b, a;

                    Ar << r;
                    Ar << g;
                    Ar << b;
                    Ar << a;

                    iInbetweenerTag.SetTrajectoryColor( FColor( r, g, b, a ) );
                }
                break;

                case FOdysseyFile::VectorV2::CHUNK_TAGINBETWEENER_LENGTH:
                {
                    uint32 drawingCount;

                    Ar << drawingCount;

                    iInbetweenerTag.GetMasterBreakdown()->SetTargetDrawingIndex( drawingCount - 1 );
                }
                break;

                case FOdysseyFile::VectorV2::CHUNK_TAGINBETWEENER_MAPASPOLYLINE:
                {
                    uint32 mapAsPolyline;

                    Ar << mapAsPolyline;

                    iInbetweenerTag.SetMapAsPolyline( mapAsPolyline ? true : false );
                }
                break;

                case FOdysseyFile::VectorV2::CHUNK_TAGINBETWEENER_TRANSFORM:  // container
                break;

                case FOdysseyFile::VectorV2::CHUNK_TAGINBETWEENER_TRANSFORM_TRANSLATION:
                    double translationX;
                    double translationY;

                    Ar << translationX;
                    Ar << translationY;

                    iInbetweenerTag.GetMasterBreakdown()->Translate( translationX, translationY );
                    //iInbetweenerTag.UpdateMatrix();
                break;

                case FOdysseyFile::VectorV2::CHUNK_TAGINBETWEENER_TRANSFORM_ROTATION:
                    double rotation;

                    Ar << rotation;

                    iInbetweenerTag.GetMasterBreakdown()->Rotate( rotation );
                    //iInbetweenerTag.UpdateMatrix();
                break;

                case FOdysseyFile::VectorV2::CHUNK_TAGINBETWEENER_TRANSFORM_SCALING:
                    double scalingX;
                    double scalingY;

                    Ar << scalingX;
                    Ar << scalingY;

                    iInbetweenerTag.GetMasterBreakdown()->Scale( scalingX, scalingY );
                    //iInbetweenerTag.UpdateMatrix();
                break;

                // legacy
                case FOdysseyFile::VectorV2::CHUNK_TAGINBETWEENER_DEFORMATION:
                {
                    uint32 gridType;

                    Ar << gridType;

                    iInbetweenerTag.SetGrid( static_cast<eInbetweenerGridType>(gridType)
                                           , iInbetweenerTag.GetGridNumQuadX()
                                           , iInbetweenerTag.GetGridNumQuadY()
                                           , iInbetweenerTag.IsSquare() );
                }
                break;

                case FOdysseyFile::VectorV2::CHUNK_TAGINBETWEENER_INTERPOLATION:
                {
                    uint32 interpolationType;

                    Ar << interpolationType;

                    iInbetweenerTag.SetInterpolationType( static_cast<eInbetweenerInterpolationType>(interpolationType) );
                }
                break;

                case FOdysseyFile::VectorV2::CHUNK_TAGINBETWEENER_DIRECTION:
                {
                    uint32 directionType;

                    Ar << directionType;

                    iInbetweenerTag.SetInterpolationDirection( static_cast<eInbetweenerInterpolationDirection>(directionType) );
                }
                break;

                // legacy
                case FOdysseyFile::VectorV2::CHUNK_TAGINBETWEENER_DIMENSION:
                {
                    uint32 numQuadX;
                    uint32 numQuadY;

                    Ar << numQuadX;
                    Ar << numQuadY;

                    iInbetweenerTag.SetGridNumQuad( numQuadX, numQuadY, false );
                }
                break;

                case FOdysseyFile::VectorV2::CHUNK_TAGINBETWEENER_GRID_SPECS:
                {
                    uint32 gridType;
                    uint32 numQuadX;
                    uint32 numQuadY;
                    uint32 square;

                    Ar << gridType;
                    Ar << numQuadX;
                    Ar << numQuadY;
                    Ar << square;

                    iInbetweenerTag.SetGrid( static_cast<eInbetweenerGridType>(gridType)
                                           , numQuadX
                                           , numQuadY
                                           , square ? true : false );
                }
                break;

                case FOdysseyFile::VectorV2::CHUNK_TAGINBETWEENER_BREAKDOWNS: // container
                break;

                case FOdysseyFile::VectorV2::CHUNK_BREAKDOWN:
                {
                    FInbetweenerBreakdown* breakdown;
                    uint32 master;
                    uint32 targetIndex;

                    Ar << master;
                    Ar << targetIndex;

                    // We don't create a new breakdown if it's the master one, a.k.a the default one.
                    breakdown = iInbetweenerTag.GetBreakdownByTargetIndex( targetIndex );

                    // fix for misdesign of breakdown layout chunk. Should be removed on the long term.
                    if( breakdown == nullptr )
                    {
                        breakdown = new FInbetweenerBreakdown( &iInbetweenerTag );

                        iInbetweenerTag.AddBreakdown( breakdown, targetIndex, false );
                    }

                    ReadBreakdown( *breakdown
                                 , Ar.Tell() + iChunkLen - 0x08 // Note: we have already read 8 bytes.
                                 , Ar );
                }
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
                                               : new FInbetweenerBreakdown( &iInbetweenerTag );

                        if( master == 0 )
                        {
                            iInbetweenerTag.AddBreakdown( breakdown, targetDrawingIndex, false );
                        }
                        else
                        {
                            breakdown->SetTargetDrawingIndex( targetDrawingIndex );
                        }
                    }
                }
                break;

                // deprecated. Kept for compatibility
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

                        breakdown->GetGrid()->SetGeometry( sourcePosition, eInbetweenerPointPositionType::SourcePosition, true );
                        breakdown->GetGrid()->SetGeometry( targetPosition, eInbetweenerPointPositionType::TargetPosition, true );
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

                default:
                // Mandatory
                    Ar.Seek( Ar.Tell() + iChunkLen );
                break;
            }
        } );
}
