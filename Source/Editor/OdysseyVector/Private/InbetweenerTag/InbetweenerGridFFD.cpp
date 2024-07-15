#include "InbetweenerTag/InbetweenerGridFFD.h"
#include "InbetweenerTag/InterpolatedPath.h"
#include "InbetweenerTag/InterpolatedSegment.h"
#include "InbetweenerTag/InterpolatedPoint.h"
#include "OdysseyVectorTagInbetweener.h"
#include "OdysseyVectorPoint.h"
#include "OdysseyVectorPath.h"
#include "OdysseyVector.h"

FInbetweenerGridFFD::FInbetweenerGridFFD( FOdysseyVectorTagInbetweener* iInbetweenerTag )
    : FInbetweenerGrid( iInbetweenerTag )
{
}

::ULIS::FVec2D
FInbetweenerGridFFD::DeformPoint( FInterpolatedPoint* iInterpolatedPoint
                                , const ::ULIS::FRectD& iSourceBBox )
{
    if( iInterpolatedPoint->GetMappedQuad() )
    {
        return iInterpolatedPoint->GetMappedQuad()->GetPoint( eInbetweenerPointPositionType::InterpPosition
                                                            , iInterpolatedPoint->GetU()
                                                            , iInterpolatedPoint->GetV() );
    }

    return ::ULIS::FVec2D( 0.0f, 0.0f );
}

void
FInbetweenerGridFFD::MapInterpolatedPaths( std::vector<FInterpolatedPath>& iPathBuffer )
{
    BLMatrix2D& ownerInverseWorldMatrix = mInbetweenerTag->GetOwner()->GetInverseWorldMatrix();
    ::ULIS::FRectD spaceBBox = mInbetweenerTag->GetSourceBBox( false );
    BLMatrix2D conversionMatrix;
    uint32 pointID = 0;
    uint32 segmentID = 0;

    mUsedQuadCount = 0;
    mUsedPointCount = 0;

    // reset point status
    for( FInbetweenerPoint& point : mPointBuffer )
    {
        point.SetNeeded( false );
    }

    // relink unlinked quads before discarding unused ones at the end of the function
    for( FInbetweenerQuad& quad : mQuadBuffer )
    {
        if( quad.IsLinked() == false )
        {
            quad.Link();
        }
    }

    for( FInterpolatedPath& interpolatedPath : iPathBuffer )
    {
        std::vector<FInterpolatedPoint>& interpolatedPointBuffer = interpolatedPath.GetInterpolatedPointBuffer();

        FOdysseyVector::MatrixMultiply( ownerInverseWorldMatrix
                                      , interpolatedPath.GetOriginalPath()->GetWorldMatrix()
                                      , conversionMatrix );

        for( FInterpolatedPoint& interpolatedPoint : interpolatedPointBuffer )
        {
            FOdysseyVectorPoint* originalPoint = interpolatedPoint.GetOriginalPoint();
            BLPoint pt = conversionMatrix.mapPoint( originalPoint->GetX()
                                                  , originalPoint->GetY() );

            int quadIndex = GetQuadIndex( ::ULIS::FVec2D( pt.x, pt.y ) );

            if( quadIndex >= 0 )
            {
                FInbetweenerQuad* matchedQuad = &mQuadBuffer[quadIndex];
                ::ULIS::FRectD quadBBox = matchedQuad->GetBBox( eInbetweenerPointPositionType::SourcePosition );
                double quadX = pt.x - quadBBox.x;
                double quadY = pt.y - quadBBox.y;
                //double u = std::clamp<double>( spaceX / iSpaceBBox.w, 0.0f, 1.0f );
                //double v = std::clamp<double>( spaceY / iSpaceBBox.h, 0.0f, 1.0f );
                double u = std::clamp<double>( quadX / quadBBox.w, 0.0f, 1.0f );
                double v = std::clamp<double>( quadY / quadBBox.h, 0.0f, 1.0f );

                // Note: we add +1 for the target position
                interpolatedPoint.SetUV( matchedQuad, u, v );
            }
        }
    }

    //DiscardEmptyQuads( mInbetweenerTag->GetInterpolatedPathBuffer() );

    mQuadArray.clear();
    mQuadArray.reserve( mQuadBuffer.size() );

    // TODO: do this in base class
    for( FInbetweenerQuad& quad : mQuadBuffer )
    {
        if( quad.IsLinked() )
        {
            mUsedQuadCount++;
            mQuadArray.push_back( &quad );
        }
    }

    for( FInbetweenerPoint& point : mPointBuffer )
    {
        if( point.GetQuadCount() )
        {
            point.SetID( mUsedPointCount++ );
        }
    }
    //---------------
}
