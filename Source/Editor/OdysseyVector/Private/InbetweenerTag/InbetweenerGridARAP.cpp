#include "InbetweenerTag/InbetweenerGridARAP.h"
#include "InbetweenerTag/InterpolatedPath.h"
#include "InbetweenerTag/InterpolatedSegment.h"
#include "InbetweenerTag/InterpolatedPoint.h"
#include "OdysseyVectorTagInbetweener.h"
#include "OdysseyVectorPoint.h"
#include "OdysseyVectorPath.h"
#include "OdysseyVector.h"
#include "OdysseyVectorHandleSegment.h"
#include "OdysseyVectorSegmentCubic.h"

FInbetweenerGridARAP::FInbetweenerGridARAP( FOdysseyVectorTagInbetweener* iInbetweenerTag )
    : FInbetweenerGrid( iInbetweenerTag )
    , mRigidity( 10 )
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


                point.SetPosition( iPositionType, deformPosition.x, deformPosition.y );

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

        if ( allGrid )
        {
            point.SetDeformable( true );
        }
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
                             , interpPosition.y );
        }
    }

    return i;
}

// polymorphic
void
FInbetweenerGridARAP::Regularize()
{
    Regularize( eInbetweenerPointPositionType::SourcePosition
              , eInbetweenerPointPositionType::TargetPosition
              , mRigidity
              , true
              , true );
}

uint32
FInbetweenerGridARAP::GetRigidity()
{
    return mRigidity;
}

void
FInbetweenerGridARAP::SetRigidity( uint32 iRigidity )
{
    mRigidity = iRigidity;
}



void
FInbetweenerGridARAP::Update( uint32 iUpdateFlags
                            , uint64 iTagInvalidationFlags )
{
    FInbetweenerGrid::Update( iUpdateFlags, iTagInvalidationFlags );
}

void
FInbetweenerGridARAP::MapInterpolatedPaths( std::vector<FInterpolatedPath>& iPathBuffer )
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

    DiscardEmptyQuads( mInbetweenerTag->GetInterpolatedPathBuffer() );

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
    uint32 uminIdx = umin * mInbetweenerTag->GetGridNumQuadX();
    uint32 vminIdx = vmin * mInbetweenerTag->GetGridNumQuadY();
    uint32 umaxIdx = umax * mInbetweenerTag->GetGridNumQuadX();
    uint32 vmaxIdx = vmax * mInbetweenerTag->GetGridNumQuadY();

    for( uint32 i = vminIdx; i <= vmaxIdx; i++ )
    {
        for( uint32 j = uminIdx; j <= umaxIdx; j++ )
        {
            uint32 offset = ( i * mInbetweenerTag->GetGridNumQuadX() ) + j;

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
    ::ULIS::FRectD tagBBox = mInbetweenerTag->GetSourceBBox( false );

    for( FInterpolatedPath& interpolatedPath : iPathBuffer )
    {
        FOdysseyVectorPath* path = interpolatedPath.GetOriginalPath();
        BLMatrix2D& tagOwnerInverseWorldMatrix = mInbetweenerTag->GetOwner()->GetInverseWorldMatrix();
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

            if( mInbetweenerTag->GetMapAsPolyline() == false )
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
