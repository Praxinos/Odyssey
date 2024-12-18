// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "InbetweenerTag/InbetweenerGridARAP.h"
#include "InbetweenerTag/InbetweenerBreakdown.h"
#include "InbetweenerTag/InterpolatedPath.h"
#include "InbetweenerTag/InterpolatedSegment.h"
#include "InbetweenerTag/InterpolatedPoint.h"
#include "OdysseyVectorTagInbetweener.h"
#include "OdysseyVectorPoint.h"
#include "OdysseyVectorPath.h"
#include "OdysseyVector.h"
#include "OdysseyVectorHandleSegment.h"
#include "OdysseyVectorSegmentCubic.h"

FInbetweenerGridARAP::FInbetweenerGridARAP( FInbetweenerBreakdown* iBreakdown )
    : FInbetweenerGrid( iBreakdown )
{
}

#define EPSILON 0.001f

// See Sykora et al. ARAP Image Registration for Hand-drawn Cartoon Animation (sec. 3.3)
void
FInbetweenerGridARAP::RegularizeQuad( FInbetweenerQuad* iQuad
                                    , eInbetweenerPointPositionType iPositionType )
{
    FInbetweenerPoint** quadPoint = iQuad->GetPoints();
    double a = 0;
    double b = 0;
    // double mu_part = 0;
    double weight = 0;

    // Compute the optimal rigid transform R, t from DEFORM_POS to dstPos
    for ( uint32 i = 0; i < 4; i++ )
    {
        FInbetweenerPoint* point = quadPoint[i];

        ::ULIS::FVec2D p_minus_pc = point->GetPosition( eInbetweenerPointPositionType::InterpPosition )
                                  - iQuad->BiasedCentroid( eInbetweenerPointPositionType::InterpPosition ); // source pose
        ::ULIS::FVec2D q_minus_qc = point->GetPosition( iPositionType )
                                  - iQuad->BiasedCentroid(iPositionType); // target pose

        a += q_minus_qc.DotProduct( p_minus_pc );
        b += q_minus_qc.DotProduct( ::ULIS::FVec2D( -p_minus_pc.y, p_minus_pc.x ) );
    }

    // If the quad is pinned we add the contribution of the pin to the minimization problem
    if ( iQuad->IsPinned() )
    {
        ::ULIS::FVec2D p_minus_pc = iQuad->GetPoint( eInbetweenerPointPositionType::InterpPosition
                                                   , iQuad->GetPinU()
                                                   , iQuad->GetPinV() )
                                  - iQuad->BiasedCentroid( eInbetweenerPointPositionType::InterpPosition );
        ::ULIS::FVec2D q_minus_qc = iQuad->GetPinPosition() - iQuad->BiasedCentroid( iPositionType );

        a += 10000.0 * q_minus_qc.DotProduct( p_minus_pc );
        b += 10000.0 * q_minus_qc.DotProduct( ::ULIS::FVec2D( -p_minus_pc.y, p_minus_pc.x ) );
    }

    double mu = sqrt( ( a * a ) + ( b * b ) );
    if ( mu < EPSILON ) mu = EPSILON;
    double r1 = a / mu;
    double r2 = -b / mu;
    Eigen::Matrix2d R;
    ::ULIS::FVec2D centroidInterpPosition = iQuad->BiasedCentroid( eInbetweenerPointPositionType::InterpPosition );
    ::ULIS::FVec2D centroidTypePosition   = iQuad->BiasedCentroid( iPositionType );
    FInbetweenerPoint::VectorType eigenCentroidInterpPosition = FInbetweenerPoint::VectorType( centroidInterpPosition.x
                                                                                             , centroidInterpPosition.y );
    FInbetweenerPoint::VectorType eigenCentroidTypePosition   = FInbetweenerPoint::VectorType( centroidTypePosition.x
                                                                                             , centroidTypePosition.y );

    R << r1, r2, -r2, r1;

    FInbetweenerPoint::VectorType t = eigenCentroidTypePosition - ( R * eigenCentroidInterpPosition );

    // Transform corners and average
    for ( uint32 i = 0; i < 4; i++ )
    {
        FInbetweenerPoint* point = quadPoint[i];
        ::ULIS::FVec2D deformPosition = point->GetPosition( eInbetweenerPointPositionType::DeformPosition );
        ::ULIS::FVec2D interpPosition = point->GetPosition( eInbetweenerPointPositionType::InterpPosition );
        FInbetweenerPoint::VectorType eigenDeformPosition = FInbetweenerPoint::VectorType( deformPosition.x
                                                                                         , deformPosition.y );
        FInbetweenerPoint::VectorType eigenInterpPosition   = FInbetweenerPoint::VectorType( interpPosition.x
                                                                                           , interpPosition.y );

        eigenDeformPosition += ( R * eigenInterpPosition + t ) / double( point->GetQuadCount() );

        point->SetDeformPosition( eigenDeformPosition.x(), eigenDeformPosition.y() );
    }

    // Update centroids position
    iQuad->ComputeCentroid( iPositionType );
}

/**
 * Apply regularization on all quads, store the resulting position in dstPos.
 * Returns the maximum corner displacement (squared L2 norm)
 */
double
FInbetweenerGridARAP::RegularizeQuads( eInbetweenerPointPositionType iPositionType )
{
    // Compute ARAP deformation and average
    for ( FInbetweenerQuad& quad : mQuadBuffer )
    {
        if( quad.IsLinked() )
        {
            RegularizeQuad( &quad, iPositionType );
        }
    }

    // Update positions and keep track of max displacement
    double maxDisp = 0;

    for ( FInbetweenerPoint& point : mPointBuffer )
    {
        if( point.GetQuadCount() )
        {
            if ( point.IsDeformable() )
            {
                ::ULIS::FVec2D tgt( point.GetPosition( iPositionType ).x
                                  , point.GetPosition( iPositionType ).y );
                ::ULIS::FVec2D nw( point.GetPosition( eInbetweenerPointPositionType::DeformPosition ).x
                                 , point.GetPosition( eInbetweenerPointPositionType::DeformPosition ).y );
                double disp = ( tgt - nw ).Distance();
                ::ULIS::FVec2D deformPosition = point.GetPosition( eInbetweenerPointPositionType::DeformPosition );


                point.SetPosition( iPositionType, deformPosition.x, deformPosition.y, true );

                if ( disp > maxDisp )
                {
                    maxDisp = disp;
                }
            }
        }

        point.SetDeformPosition( 0.0f, 0.0f );
    }

    return maxDisp;
}

/**
 * Iteratively regularize all quads in the lattice.
 *
 * @param lattice
 * @param sourcePos         The configuration of the lattice that the regularization converges to (up to a translation factor)
 * @param dstPos            Where the result configuration is stored
 * @param maxIterations     Maximum nb of iterations
 * @param allGrid           Regularize all the quads (override the deformable flag)
 * @param convergenceStop   Stops the iterative process when the maximum corner displacement falls under an hardcoded threshold. Otherwise run #maxIterations
 * @return                  Number of regularization iterations done
 */
uint32
FInbetweenerGridARAP::Regularize( eInbetweenerPointPositionType iSourcePositionType
                                , eInbetweenerPointPositionType iDestPositionType
                                , int maxIterations
                                , bool allGrid
                                , bool convergenceStop )
{
    if ( maxIterations <= 0 )
    {
        return 0;
    }

    // Initialization of interpolated position & source pos
    for ( FInbetweenerPoint& point : mPointBuffer )
    {
        if( point.GetQuadCount() )
        {
            ::ULIS::FVec2D position = point.GetPosition( iSourcePositionType );

            point.SetInterpPosition( position.x, position.y );
            point.SetDeformPosition( 0.0f      , 0.0f       );

        }

        /*if ( allGrid )
        {
            point.SetDeformable( true );
        }*/
    }
    // Compute all quad centroids
    for ( FInbetweenerQuad& quad : mQuadBuffer )
    {
        if( quad.IsLinked() )
        {
            quad.ComputeCentroids();
        }
    }

    // Apply regularization until convergence or a max number of iteration
    double maxDisp = 0;
    int i = 0;
    do
    {
        maxDisp = RegularizeQuads( iDestPositionType );
        i++;
    } while ( convergenceStop ? ( i < maxIterations && sqrt( maxDisp ) > 1e-3 )
                              : ( i < maxIterations ) );

    // Save configuration for plastic deformation
    for ( FInbetweenerPoint& point : mPointBuffer )
    {
        if( point.GetQuadCount() )
        {
            ::ULIS::FVec2D interpPosition = point.GetPosition( eInbetweenerPointPositionType::InterpPosition );

            point.SetPosition( eInbetweenerPointPositionType::DeformPosition
                             , interpPosition.x
                             , interpPosition.y
                             , true );
        }
    }

    return i;
}

// polymorphic
void
FInbetweenerGridARAP::Regularize( uint32 iRigidity )
{
    Regularize( eInbetweenerPointPositionType::SourcePosition
              , eInbetweenerPointPositionType::TargetPosition
              , iRigidity
              , true
              , true );
}

uint32
FInbetweenerGridARAP::Expand( uint32 iUsedQuadCount )
{
    std::vector<FInbetweenerQuad*> linkedQuadArray;
    uint32 extraUsedQuad = 0;

    linkedQuadArray.reserve( mQuadBuffer.size() );

    for( FInbetweenerQuad& quad : mQuadBuffer )
    {
        if( quad.IsLinked() )
        {
            linkedQuadArray.push_back( &quad );
        }
    }

    for( FInbetweenerQuad* linkedQuad : linkedQuadArray )
    {
        FInbetweenerQuad* neighbourQuad[4] = { 0 };
        uint32 neighbourCount = linkedQuad->GetNeighbours( neighbourQuad );

        for( uint32 i = 0; i < neighbourCount; i++ )
        {
            if( ( neighbourQuad[i]->IsLinked() == false ) )
            {
                neighbourQuad[i]->Link();

                extraUsedQuad++;
            }
        }
    }

    return iUsedQuadCount + extraUsedQuad;
}

bool
FInbetweenerGridARAP::IsContiguous( uint32 iUsedQuadCount )
{
    std::vector<FInbetweenerQuad*> quadStackArray;
    FInbetweenerQuad* rootQuad = nullptr;

    quadStackArray.reserve( mQuadBuffer.size() );

    for( FInbetweenerQuad& quad : mQuadBuffer )
    {
        quad.SetVisited( false );

        if( quad.IsLinked() && ( quadStackArray.size() == 0 ) )
        {
            quadStackArray.push_back( &quad );
            quad.SetVisited( true );
        }
    }

    if( quadStackArray.size() )
    {
        FInbetweenerQuad** currentQuad = &quadStackArray[0];

        while( currentQuad != &quadStackArray[quadStackArray.size()] )
        {
            FInbetweenerQuad* neighbourQuad[4] = { 0 };

            (*currentQuad)->GetNeighbours( neighbourQuad );

            for( uint32 i = 0; i < 4; i++ )
            {
                if( ( neighbourQuad[i] )
                 && ( neighbourQuad[i]->IsLinked() )
                 && ( neighbourQuad[i]->IsVisited() == false ) )
                {
                    quadStackArray.push_back( neighbourQuad[i] );
                    neighbourQuad[i]->SetVisited( true );
                }
            }

            currentQuad++;
        }
    }

    return ( quadStackArray.size() == iUsedQuadCount ) ? true : false;
}

void
FInbetweenerGridARAP::MapInterpolatedPaths( std::vector<FInterpolatedPath>& iPathBuffer )
{
    BLMatrix2D& ownerInverseWorldMatrix = mBreakdown->GetInbetweenerTag()->GetOwner()->GetInverseWorldMatrix();
    BLMatrix2D conversionMatrix;
    uint32 pointID = 0;
    uint32 segmentID = 0;

    uint32 usedQuadCount = 0;
    uint32 usedPointCount = 0;

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
                interpolatedPoint.SetUV( quadIndex, u, v );
            }
        }
    }

    DiscardEmptyQuads( mBreakdown->GetInbetweenerTag()->GetInterpolatedPathBuffer() );

    for( FInbetweenerQuad& quad : mQuadBuffer )
    {
        if( quad.IsLinked() )
        {
            usedQuadCount++;
        }
    }

    // ARAP interpolation will not work if the grid is not contiguous
    while( IsContiguous( usedQuadCount ) == false )
    {
        usedQuadCount = Expand( usedQuadCount );
    }

    for( FInbetweenerPoint& point : mPointBuffer )
    {
        if( point.GetQuadCount() )
        {
            point.SetID( usedPointCount++ );
        }
        else
        {
            point.SetID( 0xFFFFFFFFUL );
        }
    }

    mBreakdown->GetInbetweenerTag()->SetUsedQuadCount( usedQuadCount );
    mBreakdown->GetInbetweenerTag()->SetUsedPointCount( usedPointCount );
}

void
FInbetweenerGridARAP::IntersectNeededQuads( const ::ULIS::FRectD& iSourceBBox
                                          , double iXMin
                                          , double iYMin
                                          , double iXMax
                                          , double iYMax )
{
    double umin = std::clamp<double>( ( iXMin - iSourceBBox.x ) / iSourceBBox.w, 0.0f, 0.9999f );
    double vmin = std::clamp<double>( ( iYMin - iSourceBBox.y ) / iSourceBBox.h, 0.0f, 0.9999f );
    double umax = std::clamp<double>( ( iXMax - iSourceBBox.x ) / iSourceBBox.w, 0.0f, 0.9999f );
    double vmax = std::clamp<double>( ( iYMax - iSourceBBox.y ) / iSourceBBox.h, 0.0f, 0.9999f );
    uint32 uminIdx = umin * mBreakdown->GetInbetweenerTag()->GetGridNumQuadX();
    uint32 vminIdx = vmin * mBreakdown->GetInbetweenerTag()->GetGridNumQuadY();
    uint32 umaxIdx = umax * mBreakdown->GetInbetweenerTag()->GetGridNumQuadX();
    uint32 vmaxIdx = vmax * mBreakdown->GetInbetweenerTag()->GetGridNumQuadY();

    for( uint32 i = vminIdx; i <= vmaxIdx; i++ )
    {
        for( uint32 j = uminIdx; j <= umaxIdx; j++ )
        {
            uint32 offset = ( i * mBreakdown->GetInbetweenerTag()->GetGridNumQuadX() ) + j;

            mQuadBuffer[offset].GetPoints()[0]->SetNeeded( true );
            mQuadBuffer[offset].GetPoints()[1]->SetNeeded( true );
            mQuadBuffer[offset].GetPoints()[2]->SetNeeded( true );
            mQuadBuffer[offset].GetPoints()[3]->SetNeeded( true );
        }
    }
}

void
FInbetweenerGridARAP::DiscardEmptyQuads( std::vector<FInterpolatedPath>& iPathBuffer )
{
    ::ULIS::FRectD tagBBox = mGridBBox;

    for( FInterpolatedPath& interpolatedPath : iPathBuffer )
    {
        FOdysseyVectorPath* path = interpolatedPath.GetOriginalPath();
        BLMatrix2D& tagOwnerInverseWorldMatrix = mBreakdown->GetInbetweenerTag()->GetOwner()->GetInverseWorldMatrix();
        BLMatrix2D& pathWorldMatrix = path->GetWorldMatrix();
        BLMatrix2D conversionMatrix = pathWorldMatrix;

        FOdysseyVector::MatrixMultiply( tagOwnerInverseWorldMatrix
                                      , pathWorldMatrix
                                      , conversionMatrix );

        for( FOdysseyVectorSegment* segment : path->GetSegmentList() )
        {
            std::vector<FOdysseyVectorFraction>& fractionCache = segment->GetFractionCache();

            for( FOdysseyVectorFraction& fraction : fractionCache )
            {
                ::ULIS::FVec2D& p0Local = fraction.point[0]->GetCoords();
                ::ULIS::FVec2D& p1Local = fraction.point[1]->GetCoords();
                BLPoint pt0 = conversionMatrix.mapPoint( p0Local.x, p0Local.y );
                BLPoint pt1 = conversionMatrix.mapPoint( p1Local.x, p1Local.y );
                double xmin = ::ULIS::FMath::Min( pt0.x, pt1.x );
                double xmax = ::ULIS::FMath::Max( pt0.x, pt1.x );
                double ymin = ::ULIS::FMath::Min( pt0.y, pt1.y );
                double ymax = ::ULIS::FMath::Max( pt0.y, pt1.y );

                IntersectNeededQuads( tagBBox, xmin, ymin, xmax, ymax );
            }

            if( mBreakdown->GetInbetweenerTag()->GetMapAsPolyline() == false )
            {
                if( segment->GetClass() == FOdysseyVectorSegmentCubic::StaticClass() )
                {
                    FOdysseyVectorVertex* vertex0 = segment->GetVertex(0);
                    FOdysseyVectorVertex* vertex1 = segment->GetVertex(1);
                    FOdysseyVectorHandleSegment* handle0 = segment->GetHandle(0);
                    FOdysseyVectorHandleSegment* handle1 = segment->GetHandle(1);
                    ::ULIS::FVec2D& p0Local = vertex0->GetCoords();
                    ::ULIS::FVec2D& p1Local = handle0->GetCoords();
                    ::ULIS::FVec2D& p2Local = handle1->GetCoords();
                    ::ULIS::FVec2D& p3Local = vertex1->GetCoords();
                    BLPoint pt0 = conversionMatrix.mapPoint( p0Local.x, p0Local.y );
                    BLPoint pt1 = conversionMatrix.mapPoint( p1Local.x, p1Local.y );
                    BLPoint pt2 = conversionMatrix.mapPoint( p2Local.x, p2Local.y );
                    BLPoint pt3 = conversionMatrix.mapPoint( p3Local.x, p3Local.y );

                    IntersectNeededQuads( tagBBox
                                        , ::ULIS::FMath::Min( pt0.x, pt1.x )
                                        , ::ULIS::FMath::Min( pt0.y, pt1.y )
                                        , ::ULIS::FMath::Max( pt0.x, pt1.x )
                                        , ::ULIS::FMath::Max( pt0.y, pt1.y ) );

                    IntersectNeededQuads( tagBBox
                                        , ::ULIS::FMath::Min( pt3.x, pt2.x )
                                        , ::ULIS::FMath::Min( pt3.y, pt2.y )
                                        , ::ULIS::FMath::Max( pt3.x, pt2.x )
                                        , ::ULIS::FMath::Max( pt3.y, pt2.y ) );
                }
            }
        }
    }

    for( FInbetweenerQuad& quad : mQuadBuffer )
    {
        if( ( quad.GetPoints()[0]->IsNeeded() == false )
         || ( quad.GetPoints()[1]->IsNeeded() == false )
         || ( quad.GetPoints()[2]->IsNeeded() == false )
         || ( quad.GetPoints()[3]->IsNeeded() == false ) )
        {
             quad.Unlink();
        }
    }
}
