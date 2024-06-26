#include "InbetweenerTag/InbetweenerGrid.h"
#include "InbetweenerTag/InterpolatedPath.h"
#include "OdysseyVectorTagInbetweener.h"
#include "OdysseyVector.h"
#include "OdysseyVectorObject.h"

// for pow()
#include <unsupported/Eigen/MatrixFunctions>

FInbetweenerGrid::~FInbetweenerGrid()
{
    mTrajectoryList.remove_if( []( FInbetweenerTrajectory* trajectory )
                               {
                                   delete trajectory;

                                   return true;
                               } );
}

FInbetweenerGrid::FInbetweenerGrid( FOdysseyVectorTagInbetweener* iInbetweenerTag
                                  , uint32 iNumQuadX
                                  , uint32 iNumQuadY )
    : mNumQuadX( iNumQuadX )
    , mNumQuadY( iNumQuadY )
    , mInbetweenerTag( iInbetweenerTag )
    , mUsedQuadCount( 0 )
    , mUsedPointCount( 0 )
{
}

FOdysseyVectorTagInbetweener*
FInbetweenerGrid::GetInbetweenerTag()
{
    return mInbetweenerTag;
}

uint32
FInbetweenerGrid::GetNumQuadX()
{
    return mNumQuadX;
}

uint32
FInbetweenerGrid::GetNumQuadY()
{
    return mNumQuadY;
}

::ULIS::FVec2D
FInbetweenerGrid::GetCenterOfMass( eInbetweenerPointPositionType iPositionType )
{
    ::ULIS::FVec2D center = ::ULIS::FVec2D( 0.0f, 0.0f );
    uint32 pointCount = 0;

    for ( FInbetweenerPoint& point : mPointBuffer )
    {
        if( point.GetQuadCount() )
        {
            center += point.GetPosition( iPositionType );

            pointCount++;
        }
    }

    return pointCount ? ( center / pointCount ) : ::ULIS::FVec2D( 0.0f, 0.0f );
}

void FInbetweenerGrid::Make( uint32 iNumQuadX
                           , uint32 iNumQuadY
                           , const ::ULIS::FRectD& iBBox )
{
    std::vector<::ULIS::FVec2D> sourcePositionBuffer;
    std::vector<::ULIS::FVec2D> targetPositionBuffer;

    Make( iNumQuadX, iNumQuadY, iBBox, sourcePositionBuffer, targetPositionBuffer );
}

void
FInbetweenerGrid::Make( uint32 iNumQuadX
                      , uint32 iNumQuadY
                      , const ::ULIS::FRectD& iBBox
                      , const std::vector<::ULIS::FVec2D>& iSourcePositionBuffer
                      , const std::vector<::ULIS::FVec2D>& iTargetPositionBuffer )
{
    mNumQuadX = iNumQuadX;
    mNumQuadY = iNumQuadY;
    mQuadArea = 0.0f;

    RemoveAllTrajectories();
    mPointBuffer.clear();
    mQuadBuffer.clear();

    if( mNumQuadX && mNumQuadY )
    {
        double x = iBBox.x;
        double y = iBBox.y;
        double stepx = iBBox.w / iNumQuadX;
        double stepy = iBBox.h / iNumQuadY;
        uint32 numVertexX = iNumQuadX + 1;
        uint32 numVertexY = iNumQuadY + 1;
        uint32 pointID = 0;

        mPointBuffer.resize( numVertexX * numVertexY );
        mQuadBuffer.resize( mNumQuadX * mNumQuadY );

        // position vertices
        for( uint32 i = 0; i < numVertexY; i++ )
        {
            for( uint32 j = 0; j < numVertexX; j++ )
            {
                uint32 offset = ( i * numVertexX ) + j;

                mPointBuffer[offset].Init( this );
                mPointBuffer[offset].SetSourcePosition( x, y );
                mPointBuffer[offset].SetTargetPosition( x, y );

                mPointBuffer[offset].SetU( std::clamp<double>( ( x - iBBox.x ) / iBBox.w, 0.0f, 1.0f ) );
                mPointBuffer[offset].SetV( std::clamp<double>( ( y - iBBox.y ) / iBBox.h, 0.0f, 1.0f ) );

                x += stepx;
            }

            y += stepy;
            x = iBBox.x;
        }

        if( iSourcePositionBuffer.size() )
        {
            for( uint32 i = 0; i < mPointBuffer.size(); i++ )
            {
                mPointBuffer[i].SetSourcePosition( iSourcePositionBuffer[i].x
                                                 , iSourcePositionBuffer[i].y  );
            }
        }

        if( iTargetPositionBuffer.size() )
        {
            for( uint32 i = 0; i < mPointBuffer.size(); i++ )
            {
                mPointBuffer[i].SetTargetPosition( iTargetPositionBuffer[i].x
                                                 , iTargetPositionBuffer[i].y  );
            }
        }

        // design cells
        for( uint32 i = 0; i < mNumQuadY; i++ )
        {
            for( uint32 j = 0; j < mNumQuadX; j++ )
            {
                uint32 vertexOffset = ( i * numVertexX ) + j;
                uint32 quadOffset   = ( i * mNumQuadX  ) + j;
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

        mQuadArea = mQuadBuffer[0].GetSourceArea();

        mInbetweenerTag->Invalidate( FOdysseyVectorTagInbetweener::INVALIDATE_SOURCEBBOX
                                   | FOdysseyVectorTagInbetweener::INVALIDATE_TARGETBBOX );
    }
}

void
FInbetweenerGrid::Update( uint32 iUpdateFlags
                        , uint64 iTagInvalidationFlags )
{
    if( iTagInvalidationFlags & FOdysseyVectorTagInbetweener::INVALIDATE_SOURCEBBOX )
    {
        mSourceCenterOfMass = GetCenterOfMass( eInbetweenerPointPositionType::SourcePosition );
    }

    if( iTagInvalidationFlags & FOdysseyVectorTagInbetweener::INVALIDATE_TARGETBBOX )
    {
        mTargetCenterOfMass = GetCenterOfMass( eInbetweenerPointPositionType::TargetPosition );
    }
}

::ULIS::FVec2D
FInbetweenerGrid::DeformPoint( FInterpolatedPoint* iInterpolatedPoint )
{
    FInbetweenerQuad* mappedQuad = iInterpolatedPoint->GetMappedQuad();

    if( mappedQuad )
    {
        return mappedQuad->GetPoint( eInbetweenerPointPositionType::InterpPosition
                                   , iInterpolatedPoint->GetU()
                                   , iInterpolatedPoint->GetV() );
    }

    return ::ULIS::FVec2D( 0.0f, 0.0f );
}

void
FInbetweenerGrid::DeformPaths( std::vector<FInterpolatedPath>& iInterpolatedPathBuffer
                             , uint32 iInbetweenIndex )
{

    for( FInterpolatedPath& interpolatedPath : iInterpolatedPathBuffer )
    {
        std::vector<::ULIS::FVec2D>& interpolatedPointPositionBuffer = interpolatedPath.GetInterpolatedPointPositionBuffer();
        uint32 pointCount = interpolatedPath.GetInterpolatedPointBuffer().size();
        uint32 skippedOffset = ( iInbetweenIndex * pointCount );

        for( uint32 i = 0; i < pointCount; i++ )
        {
            FInterpolatedPoint* interpolatedPoint = &interpolatedPath.GetInterpolatedPointBuffer()[i];

            interpolatedPointPositionBuffer[skippedOffset + i] = DeformPoint( interpolatedPoint );
        }
    }
}

int
FInbetweenerGrid::GetQuadIndex( const ::ULIS::FVec2D& iLocalCoords )
{
    ::ULIS::FRectD bbox = mInbetweenerTag->GetSourceBBox( false );
    double difX = iLocalCoords.x - bbox.x;
    double difY = iLocalCoords.y - bbox.y;
    double u = difX / bbox.w;
    double v = difY / bbox.h;
    FInbetweenerQuad* quad = nullptr;

    if( ( u >= 0.0f ) && ( u < 1.0f )
     && ( v >= 0.0f ) && ( v < 1.0f ) )
    {
        uint32 coordU = ( u * mNumQuadX );
        uint32 coordV = ( v * mNumQuadY );
        uint32 offset = ( coordV * mNumQuadX ) + coordU;

        if( mQuadBuffer[offset].IsLinked() == true )
        {
            return offset;
        }
    }

    return -1;
}

std::list<FInbetweenerTrajectory*>&
FInbetweenerGrid::GetTrajectoryList()
{
    return mTrajectoryList;
}

void
FInbetweenerGrid::AddTrajectory( FInbetweenerTrajectory* iTrajectory )
{
    mTrajectoryList.push_back( iTrajectory );

    mInbetweenerTag->Invalidate( FOdysseyVectorTagInbetweener::INVALIDATE_TRAJECTORIES
                               | FOdysseyVectorTagInbetweener::INVALIDATE_SPACING
                               | FOdysseyVectorTagInbetweener::INVALIDATE_CELLS );
}

void
FInbetweenerGrid::RemoveTrajectory( FInbetweenerTrajectory* iTrajectory )
{
    mTrajectoryList.remove( iTrajectory );

    mInbetweenerTag->Invalidate( FOdysseyVectorTagInbetweener::INVALIDATE_SPACING
                               | FOdysseyVectorTagInbetweener::INVALIDATE_CELLS );
}

void
FInbetweenerGrid::RemoveAllTrajectories()
{
    mTrajectoryList.clear();
}

FInbetweenerTrajectory*
FInbetweenerGrid::AddTrajectory( const ::ULIS::FVec2D& iLocalCoords )
{
    int quadIndex = GetQuadIndex( iLocalCoords );

    if( quadIndex >= 0 )
    {
        FInbetweenerQuad* quad = &mQuadBuffer[quadIndex];
        FInbetweenerPoint** quadPoint = quad->GetPoints();
        ::ULIS::FVec2D p0Coords = quadPoint[0]->GetSourcePosition();
        ::ULIS::FVec2D p1Coords = quadPoint[1]->GetSourcePosition();
        ::ULIS::FVec2D p2Coords = quadPoint[2]->GetSourcePosition();
        ::ULIS::FVec2D p3Coords = quadPoint[3]->GetSourcePosition();
        double difX = p1Coords.x - p0Coords.x;
        double difY = p2Coords.y - p1Coords.y;
        double quadU = difX ? ( iLocalCoords.x - p0Coords.x ) / difX : 0.0f;
        double quadV = difY ? ( iLocalCoords.y - p0Coords.y ) / difY : 0.0f;
        FInbetweenerTrajectory* trajectory = new FInbetweenerTrajectory( this
                                                                       , quadIndex
                                                                       , quadU
                                                                       , quadV );

        AddTrajectory( trajectory );

        return trajectory;
    }

    return nullptr;
}

void
FInbetweenerGrid::SetGeometry( const std::vector<::ULIS::FVec2D>& iGeometry
                             , eInbetweenerPointPositionType iPositionType )
{
    for( uint32 i = 0; i < mPointBuffer.size(); i++ )
    {
        mPointBuffer[i].SetPosition( iPositionType, iGeometry[i].x, iGeometry[i].y );
    }
}

void
FInbetweenerGrid::GetGeometry( std::vector<::ULIS::FVec2D>& oGeometry
                             , eInbetweenerPointPositionType iPositionType )
{
    oGeometry.reserve( mPointBuffer.size() );

    for( FInbetweenerPoint& point : mPointBuffer )
    {
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

/**
 * Precompute the sparse matrices P^T and prefactor P^T*P for later computations
 * See Baxter et al. 2008
 */
bool
FInbetweenerGrid::PrecomputeARAPInterpolation()
{
/*
    if (!m_singleConnectedComponent)
    {
        qWarning() << "Cannot precompute a lattice with multiple connected components! ";
        return;
    }
*/
    std::vector<TripletD> P_triplets;
    uint32 P_rows = 8 * mUsedQuadCount; // P_rows
    double triArea = mQuadArea * 0.5f;
    int triRow = 0;

    // Compute P (sparse) and store its transpose to construct the RHS of the equation later
    // TODO refactorize concatenation
    for( FInbetweenerQuad& quad : mQuadBuffer )
    {
        if( quad.IsLinked() )
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

    for( FInbetweenerQuad& quad : mQuadBuffer )
    {
        if( quad.IsLinked() )
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

    Eigen::SparseMatrix<double, Eigen::ColMajor> P( P_rows, mUsedPointCount );
    P.setFromTriplets( P_triplets.begin(), P_triplets.end() );
    mPt = P.transpose();

    // Assembling diagonal W matrix
    mW = Eigen::VectorXd( P_rows );

    for ( uint32 i = 0; i < P_rows; ++i )
    {
        mW[i] = triArea;
    }

    // Assembling LHS (with constraint)
    uint32 constraintCount = mTrajectoryList.size() > 0 ? mTrajectoryList.size() : 1;
    uint32 idx = mUsedPointCount;
    Eigen::SparseMatrix<double, Eigen::ColMajor> PTP = mPt * mW.asDiagonal() * P;
    // Left Hand Side is a square matrix
    Eigen::SparseMatrix<double, Eigen::ColMajor> LHS( mUsedPointCount + constraintCount
                                                    , mUsedPointCount + constraintCount );

    // main constraint (linear interp of center of mass)
    // TODO: is there a more efficient way to do this than using the intermediate var PTP?
    LHS.innerVectors( 0, mUsedPointCount ) = PTP.innerVectors( 0, mUsedPointCount );

    if ( mTrajectoryList.size() == 0 )
    {
        float constraintMean = mUsedPointCount ? 1.0f / mUsedPointCount : 0.0f;

        for ( uint32 i = 0; i < mUsedPointCount; ++i )
        {
            LHS.insert( idx, i ) = constraintMean;
            LHS.insert( i, idx ) = constraintMean;
        }
        ++idx;
    }

    // user defined hard constraints
    for ( FInbetweenerTrajectory* trajectory : mTrajectoryList )
    {
        FInbetweenerQuad* quad = trajectory->GetQuad();
        FInbetweenerPoint** quadPoints = quad->GetPoints();
        double u = trajectory->GetQuadU();
        double v = trajectory->GetQuadV();

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

    // Compute ref and target center of mass
    //mSourceCenterOfMass = GetCenterOfMass( eInbetweenerPointPositionType::SourcePosition );
    //mTargetCenterOfMass = GetCenterOfMass( eInbetweenerPointPositionType::TargetPosition );

    //m_precomputeDirty = false;
    //m_arapDirty = true;
    //sw.stop();

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
    ::ULIS::FVec2D qi, qj, qk, pi, pj, pk;
    Eigen::Matrix2d P, Q;

    // target pose
    qi = iTriangle[0]->GetTargetPosition();
    qj = iTriangle[1]->GetTargetPosition();
    qk = iTriangle[2]->GetTargetPosition();

    Q << qi.x - qk.x, qi.y - qk.y, qj.x - qk.x, qj.y - qk.y;

    // reference pose
    pi = iTriangle[0]->GetSourcePosition();
    pj = iTriangle[1]->GetSourcePosition();
    pk = iTriangle[2]->GetSourcePosition();

    P << pi.x - pk.x, pi.y - pk.y, pj.x - pk.x, pj.y - pk.y;

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
FInbetweenerGrid::ComputeARAPInterpolation( //float alphaLinear
                                          //, float alpha
                                          //, const FInbetweenerPoint::Affine& globalRigidTransform
                                            const FInbetweenerInbetween* iInbetween
                                          , bool useRigidTransform )
{
//    qDebug() << "** Interpolating lattice at t=" << alpha;
//    StopWatch sw("ARAP interpolation");

    //useRigidTransform = useRigidTransform && k_useGlobalRigidTransform;
//    m_currentPrecomputedTime = alpha;

    // Lattices with multiple connected components cannot be interpolated, return reference or target configuration
//    if (!m_singleConnectedComponent) {
//        if (alpha < 1.0) copyPositions(this, REF_POS, INTERP_POS);
//        else             copyPositions(this, TARGET_POS, INTERP_POS);
//        return;
//    }

    Eigen::MatrixXd A( 2, 8 * mUsedQuadCount  );
    //double t = alpha;
    double t = iInbetween->spacing;

    // Compute A(t)
    int i = 0;
    for ( FInbetweenerQuad& quad : mQuadBuffer )
    {
        if( quad.IsLinked() )
        {
            ComputeQuadA( &quad, A, i, t, false );
        }
    }

    for ( FInbetweenerQuad& quad : mQuadBuffer )
    {
        if( quad.IsLinked() )
        {
            ComputeQuadA( &quad, A, i, t, true );
        }
    }

    // Assembling final RHS matrix and concatenating constraints values
    unsigned int constraintCount = mTrajectoryList.size() > 0 ? mTrajectoryList.size() : 1;
    unsigned int idx = mUsedPointCount;
    Eigen::MatrixXd PTAD( mUsedPointCount + constraintCount, 2 );

    PTAD.block( 0, 0, mUsedPointCount, 2 ) = mPt * mW.asDiagonal() * A.transpose();

    // Main constraint (linear interp of center of mass)
    if ( mTrajectoryList.size() == 0 )
    {
        PTAD( idx, 0 ) = mSourceCenterOfMass.x * ( 1.0f - t ) + mTargetCenterOfMass.x * t;
        PTAD( idx, 1 ) = mSourceCenterOfMass.y * ( 1.0f - t ) + mTargetCenterOfMass.y * t;
        ++idx;
    }

    // User defined constraints values

    //float offset;
    for ( FInbetweenerTrajectory* trajectory : mTrajectoryList )
    {
        ::ULIS::FVec2D* cubicBezier = trajectory->GetCubicBezier();
        ::ULIS::FVec2D coords = ::ULIS::CubicBezierPointAtParameter<::ULIS::FVec2D>( cubicBezier[0]
                                                                                   , cubicBezier[1]
                                                                                   , cubicBezier[2]
                                                                                   , cubicBezier[3]
                                                                                   , t );
        BLPoint inbetweenCoords = iInbetween->inverseMatrix.mapPoint( coords.x, coords.y );

        //offset = traj->localOffset()->get();
        //Point::VectorType pos = traj->eval(t + (std::abs(offset) < 1e-5f ? 0.0f : offset));

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
    for ( FInbetweenerPoint& point : mPointBuffer )
    {
        if( point.GetQuadCount() )
        {
            FInbetweenerPoint::VectorType coords = V.row( point.GetID() );

            point.SetInterpPosition( coords.x(), coords.y() );

    /*
            if ( useRigidTransform )
            {
                FInbetweenerPoint::VectorType rigidPoint = globalRigidTransform * coords;

                point.SetMotionPosition( rigidPoint.x(), rigidPoint.y() );
            }
    */
        }
    }

    //m_arapDirty = false;
    //sw.stop();

    return true;
}

void
FInbetweenerGrid::MapInterpolatedPaths( std::vector<FInterpolatedPath>& iPathBuffer
                                      , const BLMatrix2D& iSpaceInverseMatrix  )
{
}
