// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "InbetweenerTag/InbetweenerGrid.h"
#include "InbetweenerTag/InbetweenerRoute.h"
#include "InbetweenerTag/InbetweenerTrajectory.h"
#include "InbetweenerTag/InbetweenerBreakdown.h"
#include "InbetweenerTag/InterpolatedPath.h"
#include "OdysseyVectorTagInbetweener.h"
#include "OdysseyVector.h"
#include "OdysseyVectorPath.h"
#include "OdysseyVectorObject.h"

// for pow()
#include <unsupported/Eigen/MatrixFunctions>

FInbetweenerGrid::~FInbetweenerGrid()
{
}

FInbetweenerGrid::FInbetweenerGrid( FInbetweenerBreakdown* iBreakdown )
    : mBreakdown( iBreakdown )
    , mInvalidationFlags ( 0 )
{
    //Make();
}

FInbetweenerBreakdown*
FInbetweenerGrid::GetBreakdown()
{
    return mBreakdown;
}

::ULIS::FRectD
FInbetweenerGrid::GetBBox( eInbetweenerPointPositionType iPositionType )
{
    ::ULIS::FVec2D center = ::ULIS::FVec2D( 0.0f, 0.0f );
    uint32 pointCount = 0;
    double xmin = DBL_MAX, ymin = DBL_MAX, xmax = -DBL_MAX, ymax = -DBL_MAX;

    for ( FInbetweenerPoint& point : mPointBuffer )
    {
        ::ULIS::FVec2D pointPosition = point.GetPosition( iPositionType );

        if( point.GetQuadCount() )
        {
            if( pointPosition.x < xmin ) xmin = pointPosition.x;
            if( pointPosition.y < ymin ) ymin = pointPosition.y;
            if( pointPosition.x > xmax ) xmax = pointPosition.x;
            if( pointPosition.y > ymax ) ymax = pointPosition.y;
        }

    }

    return ( ( xmin < xmax ) && ( ymin < ymax ) ) ? ::ULIS::FRectD::FromMinMax( xmin, ymin, xmax, ymax )
                                                  : ::ULIS::FRectD::FromXYWH( 0, 0, 0, 0 );
}

::ULIS::FVec2D
FInbetweenerGrid::GetCenterOfMass( eInbetweenerPointPositionType iPositionType )
{
    ::ULIS::FVec2D center = ::ULIS::FVec2D( 0.0f, 0.0f );
    uint32 pointCount = 0;

    for ( FInbetweenerPoint& point : mPointBuffer )
    {
        ::ULIS::FVec2D pointPosition = point.GetPosition( iPositionType );

        if( point.GetQuadCount() )
        {
            center += point.GetPosition( iPositionType );

            pointCount++;
        }
    }

    return pointCount ? ( center / pointCount ) : ::ULIS::FVec2D( 0.0f, 0.0f );
}

void
FInbetweenerGrid::Invalidate( uint32 iInvalidationFlags )
{
    mInvalidationFlags |= iInvalidationFlags;
}

void
FInbetweenerGrid::UpdateBBox( uint32 iUpdateFlags
                            , uint64 iTagInvalidationFlags )
{
    if( ( mInvalidationFlags    & INVALIDATE_SOURCEBBOX                             )
     || ( iTagInvalidationFlags & FOdysseyVectorTagInbetweener::INVALIDATE_GRIDTYPE ) )
    {
        FInbetweenerBreakdown* prevBreakdown = mBreakdown->GetPrevBreakdown();

        // note: we could also use prevBreakdown->GetGrid()->mTargetBBox for faster but less safe if not updated
        mSourceBBox = prevBreakdown ? prevBreakdown->GetGrid()->GetBBox( eInbetweenerPointPositionType::TargetPosition )
                                    : GetBBox( eInbetweenerPointPositionType::SourcePosition );

        mInvalidationFlags &= ~(INVALIDATE_SOURCEBBOX);
    }

    if( ( mInvalidationFlags    & INVALIDATE_TARGETBBOX                             )
     || ( iTagInvalidationFlags & FOdysseyVectorTagInbetweener::INVALIDATE_GRIDTYPE ) )
    {
        mTargetBBox = GetBBox( eInbetweenerPointPositionType::TargetPosition );

        mInvalidationFlags &= ~(INVALIDATE_TARGETBBOX);
    }
}

void
FInbetweenerGrid::UpdateCenterOfMass( uint32 iUpdateFlags
                                    , uint64 iTagInvalidationFlags )
{
    if( ( mInvalidationFlags    & INVALIDATE_SOURCECENTEROFMASS                     )
     || ( iTagInvalidationFlags & FOdysseyVectorTagInbetweener::INVALIDATE_GRIDTYPE ) )
    {
        FInbetweenerBreakdown* prevBreakdown = mBreakdown->GetPrevBreakdown();

        // note: we could also use prevBreakdown->GetGrid()->mTargetCenterOfMass for faster but less safe if not updated
        mSourceCenterOfMass = prevBreakdown ? prevBreakdown->GetGrid()->GetCenterOfMass( eInbetweenerPointPositionType::TargetPosition )
                                            : GetCenterOfMass( eInbetweenerPointPositionType::SourcePosition );

        mInvalidationFlags &= ~(INVALIDATE_SOURCECENTEROFMASS);
    }

    if( ( mInvalidationFlags    & INVALIDATE_TARGETCENTEROFMASS                     )
     || ( iTagInvalidationFlags & FOdysseyVectorTagInbetweener::INVALIDATE_GRIDTYPE ) )
    {
        mTargetCenterOfMass = GetCenterOfMass( eInbetweenerPointPositionType::TargetPosition );

        mInvalidationFlags &= ~(INVALIDATE_TARGETCENTEROFMASS);
    }
}

void FInbetweenerGrid::ResetDeformation( bool iInvalidate )
{
    for( uint32 i = 0; i < mPointBuffer.size(); i++ )
    {
        ::ULIS::FVec2D sourcePosition = mPointBuffer[i].GetSourcePosition();

        mPointBuffer[i].SetTargetPosition( sourcePosition.x
                                         , sourcePosition.y
                                         , iInvalidate  );
    }
}

void FInbetweenerGrid::Make( bool iInvalidate )
{
    std::vector<::ULIS::FVec2D> targetPositionBuffer; // stays empty

    Make( targetPositionBuffer, iInvalidate );
}

//static
void
SquareGridBBox( ::ULIS::FRectD& iBBox )
{
    ::ULIS::FRectD tmpBox = iBBox;
    double difw, difh;

    if ( iBBox.h > iBBox.w )
    {
        iBBox.w = iBBox.h;
    }
    else
    {
        iBBox.h = iBBox.w;
    }

    difw = iBBox.w - tmpBox.w;
    difh = iBBox.h - tmpBox.h;

    iBBox.x -= ( difw * 0.5f );
    iBBox.y -= ( difh * 0.5f );
}

void
FInbetweenerGrid::Make( const std::vector<::ULIS::FVec2D>& iTargetPositionBuffer
                      , bool iInvalidate )
{
    uint32 numQuadX = mBreakdown->GetInbetweenerTag()->GetGridNumQuadX();
    uint32 numQuadY = mBreakdown->GetInbetweenerTag()->GetGridNumQuadY();
    bool square = mBreakdown->GetInbetweenerTag()->IsSquare();

    mGridBBox = mBreakdown->GetInbetweenerTag()->GetOwner()->GetBBox( true, false );


    //---- make the grid it square ---//
    if( square )
    {
        SquareGridBBox( mGridBBox );
    }

    // add some margin to prevent point that would be outside the box due to precision errors
    mGridBBox.x -= 0.01f;
    mGridBBox.y -= 0.01f;
    mGridBBox.w += 0.02f;
    mGridBBox.h += 0.02f;

    //mTargetBBox = mSourceBBox;

    mQuadArea = 0.0f;

    mPointBuffer.clear();
    mQuadBuffer.clear();

    if( numQuadX && numQuadY )
    {
        double x = mGridBBox.x;
        double y = mGridBBox.y;
        double stepx = mGridBBox.w / numQuadX;
        double stepy = mGridBBox.h / numQuadY;
        uint32 numVertexX = numQuadX + 1;
        uint32 numVertexY = numQuadY + 1;
        uint32 pointID = 0;

        mPointBuffer.reserve( numVertexX * numVertexY );
        mQuadBuffer.resize( numQuadX * numQuadY );

        // position vertices
        for( uint32 i = 0; i < numVertexY; i++ )
        {
            for( uint32 j = 0; j < numVertexX; j++ )
            {
                uint32 offset = ( i * numVertexX ) + j;
                double u = std::clamp<double>( ( x - mGridBBox.x ) / mGridBBox.w, 0.0f, 1.0f );
                double v = std::clamp<double>( ( y - mGridBBox.y ) / mGridBBox.h, 0.0f, 1.0f );

                FInbetweenerPoint& point = mPointBuffer.emplace_back( this, x, y, u, v );

                point.SetTargetPosition( x, y, iInvalidate );

                x += stepx;
            }

            y += stepy;
            x = mGridBBox.x;
        }

        if( iTargetPositionBuffer.size() /*== mPointBuffer.size()*/ )
        {
            for( uint32 i = 0; i < mPointBuffer.size(); i++ )
            {
                mPointBuffer[i].SetTargetPosition( iTargetPositionBuffer[i].x
                                                 , iTargetPositionBuffer[i].y
                                                 , iInvalidate  );
            }
        }

        // design cells
        for( uint32 i = 0; i < numQuadY; i++ )
        {
            for( uint32 j = 0; j < numQuadX; j++ )
            {
                uint32 vertexOffset = ( i * numVertexX ) + j;
                uint32 quadOffset   = ( i * numQuadX  ) + j;
                FInbetweenerQuad* quad = &mQuadBuffer[quadOffset];
                FInbetweenerPoint** gridPoint = quad->GetPoints();

                gridPoint[0] = &mPointBuffer[vertexOffset];
                gridPoint[1] = &mPointBuffer[vertexOffset+1];
                gridPoint[2] = &mPointBuffer[vertexOffset+1+numVertexX];
                gridPoint[3] = &mPointBuffer[vertexOffset+numVertexX];

                quad->Init( this );
                quad->Link();
            }
        }

        mQuadArea = ( mGridBBox.w * mGridBBox.h ) / mQuadBuffer.size();

/*
        if( iInvalidate )
        {
            mBreakdown->GetInbetweenerTag()->Invalidate( FOdysseyVectorTagInbetweener::INVALIDATE_SOURCEGRID
                                                       | FOdysseyVectorTagInbetweener::INVALIDATE_TARGETGRID );
        }
*/
    }
}

const ::ULIS::FRectD&
FInbetweenerGrid::GetTargetBBox()
{
    return mTargetBBox;
}

const ::ULIS::FRectD&
FInbetweenerGrid::GetSourceBBox()
{
    return mSourceBBox;
}

::ULIS::FVec2D
FInbetweenerGrid::DeformPoint( FInterpolatedPoint* iInterpolatedPoint, eInbetweenerPointPositionType iPositionType )
{
    FInbetweenerQuad* mappedQuad = &mQuadBuffer[iInterpolatedPoint->GetMappedQuadIndex()];

    if( mappedQuad )
    {
        return mappedQuad->GetPoint( iPositionType
                                   , iInterpolatedPoint->GetU()
                                   , iInterpolatedPoint->GetV() );
    }

    return ::ULIS::FVec2D( 0.0f, 0.0f );
}

void
FInbetweenerGrid::DeformPoints( FInterpolatedObject* iInterpolatedObject
                              , FInbetweenerChart::Inbetween *iInbetween
                              , eInbetweenerPointPositionType iPositionType )
{
    std::vector<FInterpolatedPath::PointGeometry>& interpolatedPointGeometryBuffer = iInterpolatedObject->GetInterpolatedPointGeometryBuffer();
    FOdysseyVectorObject* owner = mBreakdown->GetInbetweenerTag()->GetOwner();
    uint32 pointCount = iInterpolatedObject->GetInterpolatedPointBuffer().size();
    uint32 inbetweenAbsoluteIndex = iInbetween->GetIndexInInbetweener();
    uint32 skippedOffset = ( inbetweenAbsoluteIndex * pointCount );
    BLMatrix2D conversionMatrix = iInterpolatedObject->GetOriginalObject()->GetInverseWorldMatrix();

    conversionMatrix.transform( owner->GetWorldMatrix() );

    for( uint32 i = 0; i < pointCount; i++ )
    {
        FInterpolatedPoint* interpolatedPoint = &iInterpolatedObject->GetInterpolatedPointBuffer()[i];
        ::ULIS::FVec2D deformedPoint = DeformPoint( interpolatedPoint, iPositionType );

        // Point will be in owner coords. convert it in path coords
        // note: owner and path could be the same, in which case coords remain the same
        interpolatedPointGeometryBuffer[skippedOffset + i].position = FOdysseyVector::MapPoint( conversionMatrix
                                                                                              , deformedPoint );
    }
}

void
FInbetweenerGrid::DeformObjects( FInbetweenerChart::Inbetween *iInbetween
                               , eInbetweenerPointPositionType iPositionType )
{
    std::vector<FInterpolatedObject*>& interpolatedObjectArray = mBreakdown->GetInbetweenerTag()->GetInterpolatedObjectArray();

    for( FInterpolatedObject* interpolatedObject : interpolatedObjectArray )
    {
        DeformPoints( interpolatedObject, iInbetween, iPositionType );
    }
}

int
FInbetweenerGrid::GetQuadIndex( const ::ULIS::FVec2D& iLocalCoords )
{
    ::ULIS::FRectD bbox = mGridBBox;
    double difX = iLocalCoords.x - bbox.x;
    double difY = iLocalCoords.y - bbox.y;
    double u = difX / bbox.w;
    double v = difY / bbox.h;
    FInbetweenerQuad* quad = nullptr;
    uint32 numQuadX = mBreakdown->GetInbetweenerTag()->GetGridNumQuadX();
    uint32 numQuadY = mBreakdown->GetInbetweenerTag()->GetGridNumQuadY();

    if( ( u >= 0.0f ) && ( u < 1.0f )
     && ( v >= 0.0f ) && ( v < 1.0f ) )
    {
        uint32 coordU = ( u * numQuadX );
        uint32 coordV = ( v * numQuadY );
        uint32 offset = ( coordV * numQuadX ) + coordU;

        if( mQuadBuffer[offset].IsLinked() == true )
        {
            return offset;
        }
    }

    return -1;
}

void
FInbetweenerGrid::SetGeometry( const std::vector<::ULIS::FVec2D>& iGeometry
                             , eInbetweenerPointPositionType iPositionType
                             , bool iInvalidate )
{
    for( uint32 i = 0; i < mPointBuffer.size(); i++ )
    {
        mPointBuffer[i].SetPosition( iPositionType, iGeometry[i].x, iGeometry[i].y, iInvalidate );
    }
}

void
FInbetweenerGrid::GetGeometry( std::vector<::ULIS::FVec2D>& oGeometry
                             , eInbetweenerPointPositionType iPositionType )
{
    oGeometry.clear( );
    oGeometry.reserve( mPointBuffer.size() );

    for( FInbetweenerPoint& point : mPointBuffer )
    {
        uint32 pointIndex = point.GetIndex();

        oGeometry.emplace_back( point.GetPosition( iPositionType ) );
    }
}

std::vector<FInbetweenerQuad>&
FInbetweenerGrid::GetQuadBuffer()
{
    return mQuadBuffer;
}

std::vector<FInbetweenerPoint>&
FInbetweenerGrid::GetPointBuffer()
{
    return mPointBuffer;
}

/**
 * Compute P* for the two triangles of the given quad and add them to the sparse matrix P (via the triplet list)
 * See Baxter et al. 2008
 */
void
FInbetweenerGrid::ComputePStar( FInbetweenerPoint* iTriangle[3]
                              , int triRow
                              , eInbetweenerPointPositionType iPositionType
                              , std::vector<TripletD>& PTriplets )
{
    Eigen::MatrixXd P( 3, 2 ), D( 2, 3 ), PStar( 2, 3 );
    D << 1, 0, -1, 0, 1, -1;
    int i, j, k;

    P( 0, 0 ) = iTriangle[0]->GetPosition( iPositionType ).x;
    P( 0, 1 ) = iTriangle[0]->GetPosition( iPositionType ).y;
    P( 1, 0 ) = iTriangle[1]->GetPosition( iPositionType ).x;
    P( 1, 1 ) = iTriangle[1]->GetPosition( iPositionType ).y;
    P( 2, 0 ) = iTriangle[2]->GetPosition( iPositionType ).x;
    P( 2, 1 ) = iTriangle[2]->GetPosition( iPositionType ).y;

    PStar = ( D * P ).inverse() * D;

    i = iTriangle[0]->GetID();
    j = iTriangle[1]->GetID();
    k = iTriangle[2]->GetID();

    PTriplets.push_back( TripletD( 2 * triRow    , i, PStar( 0, 0 ) ) );
    PTriplets.push_back( TripletD( 2 * triRow    , j, PStar( 0, 1 ) ) );
    PTriplets.push_back( TripletD( 2 * triRow    , k, PStar( 0, 2 ) ) );
    PTriplets.push_back( TripletD( 2 * triRow + 1, i, PStar( 1, 0 ) ) );
    PTriplets.push_back( TripletD( 2 * triRow + 1, j, PStar( 1, 1 ) ) );
    PTriplets.push_back( TripletD( 2 * triRow + 1, k, PStar( 1, 2 ) ) );
}

void
FInbetweenerGrid::GetValidRouteArray( std::vector<FInbetweenerRoute*>& oValidRouteArray )
{
    std::list<FInbetweenerRoute*>& routeList = mBreakdown->GetInbetweenerTag()->GetRouteList();

    oValidRouteArray.clear();
    oValidRouteArray.reserve( routeList.size() );

    for( FInbetweenerRoute* route : routeList )
    {
        if ( route->IsEnabled() )
        {
            if( mQuadBuffer[route->GetQuadIndex()].IsLinked() )
            {
                oValidRouteArray.emplace_back( route );
            }
        }
    }
}

/**
 * Precompute the sparse matrices P^T and prefactor P^T*P for later computations
 * See Baxter et al. 2008
 */
bool
FInbetweenerGrid::PrecomputeARAPInterpolation()
{
    TRACE_CPUPROFILER_EVENT_SCOPE(FInbetweenerGrid::PrecomputeARAPInterpolation);

    std::vector<FInbetweenerRoute*> validRouteArray;
    uint32 usedQuadCount = mBreakdown->GetInbetweenerTag()->GetUsedQuadCount();
    uint32 usedPointCount = mBreakdown->GetInbetweenerTag()->GetUsedPointCount();
    uint32 P_rows = 8 * usedQuadCount; // P_rows
    double triArea = mQuadArea * 0.5f;
    std::vector<TripletD> P_triplets;
    int triRow = 0;

    GetValidRouteArray( validRouteArray );

    // Compute P (sparse) and store its transpose to construct the RHS of the equation later
    // TODO refactorize concatenation
    //for( FInbetweenerQuad& quad : mQuadBuffer )
    // we use an index that is stored in the inbetweener tag because it is computed only once for the front grid
    for( uint32 quadIndex : mBreakdown->GetInbetweenerTag()->GetUsedQuadIndexBuffer() )
    {
        FInbetweenerQuad& quad = mQuadBuffer[quadIndex];

        //if( quad.IsLinked() )
        {
            FInbetweenerPoint** points = quad.GetPoints();
            FInbetweenerPoint* triangleA[3] = { points[0], points[1], points[2] };
            FInbetweenerPoint* triangleB[3] = { points[2], points[3], points[0] };

            ComputePStar( triangleA, triRow, eInbetweenerPointPositionType::SourcePosition, P_triplets );
            triRow++;
            ComputePStar( triangleB, triRow, eInbetweenerPointPositionType::SourcePosition, P_triplets );
            triRow++;
        }
    }

    //for( FInbetweenerQuad& quad : mQuadBuffer )
    // we use an index that is stored in the inbetweener tag because it is computed only once for the front grid
    for( uint32 quadIndex : mBreakdown->GetInbetweenerTag()->GetUsedQuadIndexBuffer() )
    {
        FInbetweenerQuad& quad = mQuadBuffer[quadIndex];

        //if( quad.IsLinked() )
        {
            FInbetweenerPoint** points = quad.GetPoints();
            FInbetweenerPoint* triangleA[3] = { points[0], points[1], points[2] };
            FInbetweenerPoint* triangleB[3] = { points[2], points[3], points[0] };

            ComputePStar( triangleA, triRow, eInbetweenerPointPositionType::TargetPosition, P_triplets );
            triRow++;
            ComputePStar( triangleB, triRow, eInbetweenerPointPositionType::TargetPosition, P_triplets );
            triRow++;
        }
    }

    Eigen::SparseMatrix<double, Eigen::ColMajor> P( P_rows, usedPointCount );
    P.setFromTriplets( P_triplets.begin(), P_triplets.end() );
    mPt = P.transpose();

    // Assembling diagonal W matrix
    mW = Eigen::VectorXd( P_rows );

    for ( uint32 i = 0; i < P_rows; ++i )
    {
        mW[i] = triArea;
    }

    // Assembling LHS (with constraint)
    uint32 constraintCount = validRouteArray.size() > 0 ? validRouteArray.size() : 1;
    uint32 idx = usedPointCount;
    Eigen::SparseMatrix<double, Eigen::ColMajor> PTP = mPt * mW.asDiagonal() * P;
    // Left Hand Side is a square matrix
    Eigen::SparseMatrix<double, Eigen::ColMajor> LHS( usedPointCount + constraintCount
                                                    , usedPointCount + constraintCount );

    // main constraint (linear interp of center of mass)
    // TODO: is there a more efficient way to do this than using the intermediate var PTP?
    LHS.innerVectors( 0, usedPointCount ) = PTP.innerVectors( 0, usedPointCount );

    if ( validRouteArray.size() == 0 )
    {
        float constraintMean = usedPointCount ? 1.0f / usedPointCount : 0.0f;

        for ( uint32 i = 0; i < usedPointCount; ++i )
        {
            LHS.insert( idx, i ) = constraintMean;
            LHS.insert( i, idx ) = constraintMean;
        }
        ++idx;
    }

    // user defined hard constraints
    for ( FInbetweenerRoute* validRoute : validRouteArray )
    {
        FInbetweenerQuad* quad = validRoute->GetTrajectoryBuffer()[mBreakdown->GetIndex()].GetQuad();
        FInbetweenerPoint** quadPoints = quad->GetPoints();
        double u = validRoute->GetQuadU();
        double v = validRoute->GetQuadV();

        // the constraint coeff vector and its transpose are set at the same time
        LHS.insert( idx, quadPoints[0]->GetID()) = LHS.insert( quadPoints[0]->GetID(), idx ) = ( 1.0f - u ) * ( 1.0f - v );
        LHS.insert( idx, quadPoints[1]->GetID()) = LHS.insert( quadPoints[1]->GetID(), idx ) = u * ( 1.0 - v );
        LHS.insert( idx, quadPoints[2]->GetID()) = LHS.insert( quadPoints[2]->GetID(), idx ) = u * v;
        LHS.insert( idx, quadPoints[3]->GetID()) = LHS.insert( quadPoints[3]->GetID(), idx ) = ( 1.0f - u ) * v;
        ++idx;
    }

    // Factorization of LHS
    LHS.makeCompressed();
    mLU.compute( LHS );

    if ( mLU.info() != Eigen::Success )
    {
        UE_LOG(LogTemp, Warning, TEXT("ERROR DURING FACTORIZATION"));

        return false;
    }

    return true;
}


/**
 * Computes "A" the transpose of the jacobian of the affine map between two triangles (ref pose vs target pose of a lattice cell) i.e. A is the linear part of the affine map between the two triangles.
 * A=(1/P)*Q   Eq. 2, Rigid Shape Interpolation Using Normal Equations, Baxter et al. 2008. i and j are corner indices used to determine which triangle of the quad we are using
 *
 * @param q                     quad
 * @param i                     corner of the quad (!= BOTTOM_LEFT)
 * @param j                     corner of the quad (!= BOTTOM_LEFT)
 * @param inverseOrientation    if true the output linear transform goes from target to source (Q->P)
 * @param A                     output linear transform
 */
void FInbetweenerGrid::ComputeJAM( FInbetweenerPoint* iTriangle[3]
                                 , bool iInverseOrientation
                                 , Eigen::Matrix2d& oA )
{
    // target pose
    const ::ULIS::FVec2D &qi = iTriangle[0]->GetTargetPosition()
                       , &qj = iTriangle[1]->GetTargetPosition()
                       , &qk = iTriangle[2]->GetTargetPosition();
    // reference pose
    const ::ULIS::FVec2D &pti = iTriangle[0]->GetSourcePosition()
                       , &ptj = iTriangle[1]->GetSourcePosition()
                       , &ptk = iTriangle[2]->GetSourcePosition();
    Eigen::Matrix2d P, Q;

    Q << qi.x - qk.x, qi.y - qk.y, qj.x - qk.x, qj.y - qk.y;
    P << pti.x - ptk.x, pti.y - ptk.y, ptj.x - ptk.x, ptj.y - ptk.y;

    if ( iInverseOrientation )
    {
        oA = Q.inverse() * P;
    } else {
        oA = P.inverse() * Q;
    }
}

/**
 * Computing the polar decomposition of A
 *
 * @param A input linear transform matrix
 * @param S output shear matrix
 * @return rotation angle in rad
 */
double
FInbetweenerGrid::PolarDecomp( Eigen::Matrix2d& iA, Eigen::Matrix2d& oS )
{
    Eigen::Matrix2d B = iA.transpose();
    Eigen::Matrix2d Rt;
    double angle = std::atan2( B( 1, 0 ), B( 0, 0 ) );

    Rt << cos( angle ), sin( angle ), -sin( angle ), cos( angle );

    oS = Rt * B;

    return angle;
}

/**
 * Compute interpolated target linear maps A(t) (concatenated for the two triangles of the given quad)
 * See Baxter et al. 2008
 */

void
FInbetweenerGrid::ComputeQuadA( FInbetweenerQuad* iQuad
                              , Eigen::MatrixXd& iAt
                              , int &i
                              , float iT
                              , bool iInverseOrientation )
{
    FInbetweenerPoint** points = iQuad->GetPoints();
    FInbetweenerPoint* triangle0[3] = { points[0], points[1], points[2] };
    FInbetweenerPoint* triangle1[3] = { points[2], points[3], points[0] };
    Eigen::Matrix2d A_interp, I;
    Eigen::Matrix2d A, Rt, S;
    double angle;

    I = Eigen::Matrix2d::Identity();

    if ( iInverseOrientation ) iT = 1.0f - iT;

    // Compute and concatenate the interpolated linear transformation of the triangle
    // formed by CornerA, CornerB and the bottom left corner of the quad
    auto computeTriangleA = [&]( FInbetweenerPoint* iTriangle[3] )
    {
        ComputeJAM( iTriangle, iInverseOrientation, A );

        angle = PolarDecomp( A, S );

        // Interpolated rotation matrix
        Rt << cos( angle * iT ), -sin( angle * iT ), sin( angle * iT ), cos( angle * iT );

        // Interpolated linear transformation of the triangle (rotation and shearing interpolated independently)
        // A_interp = Rt * ((1 - t) * I + t * S);
        A_interp = Rt * S.pow( iT );

        // Concatenate transposed result to the input matrix A(t)
        iAt( 0, i ) = A_interp( 0, 0 );
        iAt( 1, i ) = A_interp( 1, 0 );
        i++;
        iAt( 0, i ) = A_interp( 0, 1 );
        iAt( 1, i ) = A_interp( 1, 1 );
        i++;
    };

    computeTriangleA( triangle0 );
    computeTriangleA( triangle1 );
}

/**
 * Compute the interpolation of the lattice between its REF_POS and TARGET_POS.
 * Stores the results in INTERP_POS.
 * The resulting interpolated lattice can be additionally transformed by a given rigid transformation.
 *
 * @param alphaLinear Linear interpolating factor between the two adjacent keyframes (from the timeline: (curFrame - prevKeyFrame) / (nextKeyFrame - prevKeyFrame))
 * @param alpha Remapping of the linear interpolating factor by the group's spacing function. This is what controls the interpolation.
 * @param globalRigidTransform Global rigid transformation applied after the interpolation.
 * @param useRigidTransform If true the global rigid transformation is applied.
 */
bool
FInbetweenerGrid::ComputeARAPInterpolation( FInbetweenerChart::Inbetween* iInbetween
                                          , bool useRigidTransform )
{
    TRACE_CPUPROFILER_EVENT_SCOPE(FInbetweenerGrid::ComputeARAPInterpolation);

    uint32 usedQuadCount = mBreakdown->GetInbetweenerTag()->GetUsedQuadCount();
    uint32 usedPointCount = mBreakdown->GetInbetweenerTag()->GetUsedPointCount();
    std::vector<FInbetweenerRoute*> validRouteArray;

    GetValidRouteArray( validRouteArray );

/*
    auto startTotal = std::chrono::high_resolution_clock::now();
*/
    Eigen::MatrixXd A( 2, 8 * usedQuadCount  );
    double t = iInbetween->GetSpacing();
    // Compute A(t)
    int i = 0;

    //for ( FInbetweenerQuad& quad : mQuadBuffer )
    // we use an index that is stored in the inbetweener tag because it is computed only once for the front grid
    for( uint32 quadIndex : mBreakdown->GetInbetweenerTag()->GetUsedQuadIndexBuffer() )
    {
        FInbetweenerQuad& quad = mQuadBuffer[quadIndex];

        //if( quad.IsLinked() )
        {
            ComputeQuadA( &quad, A, i, t, false );
        }
    }

    //for ( FInbetweenerQuad& quad : mQuadBuffer )
    // we use an index that is stored in the inbetweener tag because it is computed only once for the front grid
    for( uint32 quadIndex : mBreakdown->GetInbetweenerTag()->GetUsedQuadIndexBuffer() )
    {
        FInbetweenerQuad& quad = mQuadBuffer[quadIndex];

        //if( quad.IsLinked() )
        {
            ComputeQuadA( &quad, A, i, t, true );
        }
    }

    // Assembling final RHS matrix and concatenating constraints values
    unsigned int constraintCount = validRouteArray.size() > 0 ? validRouteArray.size() : 1;
    unsigned int idx = usedPointCount;
    Eigen::MatrixXd PTAD( usedPointCount + constraintCount, 2 );

    PTAD.block( 0, 0, usedPointCount, 2 ) = mPt * mW.asDiagonal() * A.transpose();

    // Main constraint (linear interp of center of mass)
    if ( validRouteArray.size() == 0 )
    {
        // get grid center of mass in their respective boundary space
        BLPoint sourceCenterOfMass = mBreakdown->GetSourceLocalMatrix().mapPoint( mSourceCenterOfMass.x
                                                                                , mSourceCenterOfMass.y );
        BLPoint targetCenterOfMass = mBreakdown->GetTargetLocalMatrix().mapPoint( mTargetCenterOfMass.x
                                                                                , mTargetCenterOfMass.y );
        // convert to inbetween space
        BLPoint inbetweenSourceCenterOfMass = iInbetween->GetDrawing()->inverseMatrix.mapPoint( sourceCenterOfMass.x, sourceCenterOfMass.y );
        BLPoint inbetweenTargetCenterOfMass = iInbetween->GetDrawing()->inverseMatrix.mapPoint( targetCenterOfMass.x, targetCenterOfMass.y );

        PTAD( idx, 0 ) = inbetweenSourceCenterOfMass.x + ( ( inbetweenTargetCenterOfMass.x - inbetweenSourceCenterOfMass.x ) * t );
        PTAD( idx, 1 ) = inbetweenSourceCenterOfMass.y + ( ( inbetweenTargetCenterOfMass.y - inbetweenSourceCenterOfMass.y ) * t );
        ++idx;
    }

    // User defined constraints values
    for ( FInbetweenerRoute* route : validRouteArray )
    {
        FInbetweenerTrajectory* trajectory = &route->GetTrajectoryBuffer()[mBreakdown->GetIndex()];
        ::ULIS::FVec2D* cubicBezier = trajectory->GetCubicBezier();
        uint32 waypointIndex = iInbetween->GetIndex() ;
        double waypointT = trajectory->GetWaypointBuffer()[waypointIndex].GetT();
        ::ULIS::FVec2D coords = trajectory->GetPointFromLinearT( waypointT );

        BLPoint inbetweenCoords = iInbetween->GetDrawing()->inverseMatrix.mapPoint( coords.x, coords.y );

        PTAD( idx, 0 ) = inbetweenCoords.x;
        PTAD( idx, 1 ) = inbetweenCoords.y;
        ++idx;
    }

    Eigen::MatrixXd V = mLU.solve( PTAD ).eval();

    if ( mLU.info() != Eigen::Success )
    {
        UE_LOG(LogTemp, Warning, TEXT("ERROR DURING SOLVE"));

        return false;
    }

    // Setting new interpolated vertices in corners INTERP_POS coordinates
    //for ( FInbetweenerPoint& point : mPointBuffer )
    // we use an index that is stored in the inbetweener tag because it is computed only once for the front grid
    for( uint32 pointIndex : mBreakdown->GetInbetweenerTag()->GetUsedPointIndexBuffer() )
    {
        FInbetweenerPoint& point = mPointBuffer[pointIndex];

        //if( point.GetQuadCount() )
        {
            uint32 pointID = point.GetID();
            FInbetweenerPoint::VectorType coords = V.row( pointID );

            point.SetInterpPosition( coords.x(), coords.y() );
        }
    }

/*
    auto stopTotal = std::chrono::high_resolution_clock::now();
    auto durationTotal = std::chrono::duration_cast<std::chrono::microseconds>(stopTotal - startTotal);
    UE_LOG(LogTemp, Warning, TEXT("ComputeARAPInterpolation Exec time %llu"), durationTotal.count() );
*/

    return true;
}

void
FInbetweenerGrid::MapInterpolatedObjects()
{
}
