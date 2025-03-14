// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "Import/v2/OdysseyVectorImport.h"
#include "Palette/OdysseyPalette.h"
#include "Engine/ObjectLibrary.h"
#include "AssetRegistry/AssetRegistryModule.h"

// from module OdysseyFile
#include "OdysseyFile.h"
#include "InbetweenerTag/InbetweenerPoint.h"
#include "InbetweenerTag/InbetweenerGrid.h"
#include "InbetweenerTag/InbetweenerBreakdown.h"
#include "OdysseyVectorTagInbetweener.h"

void
FOdysseyVectorImportV2::ReadBreakdown( FInbetweenerBreakdown& iBreakdown
                                     , uint64 iChunkEnd
                                     , FArchive &Ar )
{
    FOdysseyFile::ReadChunks( iChunkEnd
                            , Ar
                            , [ this
                              , &iBreakdown ]( uint32 iChunkID
                                             , uint64 iChunkLen
                                             , FArchive &Ar ) -> void
        {
            switch( iChunkID )
            {
                case FOdysseyFile::VectorV2::CHUNK_BREAKDOWN_TARGETVISIBILITY:
                    uint32 visibility;

                    Ar << visibility;

                    iBreakdown.SetTargetVisibility( visibility ? true : false );
                break;

                case FOdysseyFile::VectorV2::CHUNK_BREAKDOWN_TRANSFORM:  // container
                break;

                case FOdysseyFile::VectorV2::CHUNK_BREAKDOWN_TRANSFORM_TRANSLATION:
                    double translationX;
                    double translationY;

                    Ar << translationX;
                    Ar << translationY;

                    iBreakdown.Translate( translationX, translationY );
                    //iBreakdown.UpdateMatrix();
                break;

                case FOdysseyFile::VectorV2::CHUNK_BREAKDOWN_TRANSFORM_ROTATION:
                    double rotation;

                    Ar << rotation;

                    iBreakdown.Rotate( rotation );
                    //iBreakdown.UpdateMatrix();
                break;

                case FOdysseyFile::VectorV2::CHUNK_BREAKDOWN_TRANSFORM_SCALING:
                    double scalingX;
                    double scalingY;

                    Ar << scalingX;
                    Ar << scalingY;

                    iBreakdown.Scale( scalingX, scalingY );
                    //iBreakdown.UpdateMatrix();
                break;

                case FOdysseyFile::VectorV2::CHUNK_BREAKDOWN_GRIDGEOMETRY:
                {
                    uint32 numQuadX = iBreakdown.GetInbetweenerTag()->GetGridNumQuadX();
                    uint32 numQuadY = iBreakdown.GetInbetweenerTag()->GetGridNumQuadY();

                    std::vector<FInbetweenerPoint>& gridPointbuffer = iBreakdown.GetGrid()->GetPointBuffer();
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

                    iBreakdown.GetGrid()->SetGeometry( sourcePosition, eInbetweenerPointPositionType::SourcePosition, true );
                    iBreakdown.GetGrid()->SetGeometry( targetPosition, eInbetweenerPointPositionType::TargetPosition, true );
                }
                break;

                case FOdysseyFile::VectorV2::CHUNK_BREAKDOWN_CHART:
                break;

                case FOdysseyFile::VectorV2::CHUNK_BREAKDOWN_CHART_HUDBEZIER:
                {
                    FInbetweenerChart::HUDBezier* quadraticBezier = iBreakdown.GetChart()->GetHUDBezier();
                    double x, y;

                    Ar << x;
                    Ar << y;
                    quadraticBezier->GetPoints()[0].SetPosition( x, y );

                    Ar << x;
                    Ar << y;
                    quadraticBezier->GetPoints()[1].SetPosition( x, y );

                    Ar << x;
                    Ar << y;
                    quadraticBezier->GetPoints()[2].SetPosition( x, y );
                }
                break;

                case FOdysseyFile::VectorV2::CHUNK_BREAKDOWN_CHART_SPACING:
                {
                    for( FInbetweenerChart::Inbetween& division : iBreakdown.GetChart()->GetInbetweenBuffer() )
                    {
                        float spacing;

                        Ar << spacing;

                        division.SetSpacing( spacing );
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
