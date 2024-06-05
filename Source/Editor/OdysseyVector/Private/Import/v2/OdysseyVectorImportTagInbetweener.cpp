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
                            , [&iInbetweenerTag](uint32 iChunkID, uint64 iChunkLen, FArchive &Ar) -> void
        {
            switch( iChunkID )
            {
                case FOdysseyFile::VectorV2::CHUNK_TAGINBETWEENER_INBETWEENCOUNT:
                {
                    uint32 inbetweenCount;

                    Ar << inbetweenCount;

                    iInbetweenerTag.SetInbetweenCount( inbetweenCount );
                    // allocating chart will alow us to read timing data
                    iInbetweenerTag.ResetChart();
                }
                break;

                case FOdysseyFile::VectorV2::CHUNK_TAGINBETWEENER_CHART:
                {
                    for( FInbetweenerInbetween& inbetween : iInbetweenerTag.GetChart().inbetweenBuffer )
                    {
                        float spacing;

                        Ar << spacing;

                        inbetween.spacing = spacing;
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
                break;

                case FOdysseyFile::VectorV2::CHUNK_TAGINBETWEENER_TRANSFORM_ROTATION:
                    double rotation;

                    Ar << rotation;

                    iInbetweenerTag.Rotate( rotation );
                break;

                case FOdysseyFile::VectorV2::CHUNK_TAGINBETWEENER_TRANSFORM_SCALING:
                    double scalingX;
                    double scalingY;

                    Ar << scalingX;
                    Ar << scalingY;

                    iInbetweenerTag.Scale( scalingX, scalingY );
                break;

                case FOdysseyFile::VectorV2::CHUNK_TAGINBETWEENER_FFDGRID:  // container
                    iInbetweenerTag.SetGridType( eInbetweenerGridType::FFD );
                break;

                case FOdysseyFile::VectorV2::CHUNK_TAGINBETWEENER_FFDGRID_SIZE:
                {
                    uint32 numQuadX;
                    uint32 numQuadY;

                    Ar << numQuadX;
                    Ar << numQuadY;

                    iInbetweenerTag.SetGridNumQuad( numQuadX, numQuadY );
                }
                break;

                case FOdysseyFile::VectorV2::CHUNK_TAGINBETWEENER_FFDGRID_GEOMETRY:
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

                default:
                // Mandatory
                    Ar.Seek( Ar.Tell() + iChunkLen );
                break;
            }    
        } );
}
