#include "InbetweenerTag/InbetweenerGrid.h"
#include "OdysseyVectorTagInbetweener.h"
#include "OdysseyVector.h"
#include "OdysseyVectorObject.h"

// for pow()
#include <unsupported/Eigen/MatrixFunctions>

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
    return mNumQuadY;
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
        if( point.mQuadList.size() )
        {
            center += point.GetPosition( iPositionType );

            pointCount++;
        }
    }

    return pointCount ? ( center / pointCount ) : ::ULIS::FVec2D( 0.0f, 0.0f );
}

void
FInbetweenerGrid::Make( uint32 iNumQuadX, uint32 iNumQuadY )
{
    mNumQuadX = iNumQuadX;
    mNumQuadY = iNumQuadY;

    if( mNumQuadX && mNumQuadY )
    {
        ::ULIS::FRectD bbox = mInbetweenerTag->GetOwner()->GetBBox( false );
        double x = bbox.x;
        double y = bbox.y;
        double stepx = bbox.w / iNumQuadX;
        double stepy = bbox.h / iNumQuadY;
        uint32 numVertexX = iNumQuadX + 1;
        uint32 numVertexY = iNumQuadY + 1;
        uint32 pointID = 0;

        mBBox = bbox;
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
                mPointBuffer[offset].mTargetPosition = mPointBuffer[offset].mSourcePosition;

                mPointBuffer[offset].u = std::clamp<double>( ( x - bbox.x ) / bbox.w, 0.0f, 1.0f );
                mPointBuffer[offset].v = std::clamp<double>( ( y - bbox.y ) / bbox.h, 0.0f, 1.0f );

                x += stepx;
            }

            y += stepy;
            x = bbox.x;
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

                quad->Link();
            }
        }

        for( FInbetweenerPoint& point : mPointBuffer )
        {
            if( point.GetQuadList().size() )
            {
                point.SetID( pointID++ );
            }
        }

        mUsedQuadCount = mQuadBuffer.size();
        mUsedPointCount = pointID;
    }
}

FInbetweenerQuad*
FInbetweenerGrid::GetQuad( const ::ULIS::FVec2D& iLocalCoords )
{
    double difX = iLocalCoords.x - mBBox.x;
    double difY = iLocalCoords.y - mBBox.y;
    double u = difX / mBBox.w;
    double v = difY / mBBox.h;
    FInbetweenerQuad* quad = nullptr;

    if( ( u >= 0.0f ) && ( u < 1.0f )
     && ( v >= 0.0f ) && ( v < 1.0f ) )
    {
        uint32 coordU = ( u * mNumQuadX );
        uint32 coordV = ( v * mNumQuadY );
        uint32 offset = ( coordV * mNumQuadX ) + coordU;

        if( mQuadBuffer[offset].IsLinked() == true )
        {
            return &mQuadBuffer[offset];
        }
    }

    return quad;
}

bool
FInbetweenerGrid::AddTrajectory( const ::ULIS::FVec2D& iLocalCoords )
{
    FInbetweenerQuad* quad = GetQuad( iLocalCoords );

    if( quad )
    {
        FInbetweenerPoint** quadPoint = quad->GetPoints();
        ::ULIS::FVec2D p0Coords = quadPoint[0]->GetSourcePosition();
        ::ULIS::FVec2D p1Coords = quadPoint[1]->GetSourcePosition();
        ::ULIS::FVec2D p2Coords = quadPoint[2]->GetSourcePosition();
        ::ULIS::FVec2D p3Coords = quadPoint[3]->GetSourcePosition();
        double difX = p1Coords.x - p0Coords.x;
        double difY = p2Coords.y - p1Coords.y;
        double quadU = difX ? ( iLocalCoords.x - p0Coords.x ) / difX : 0.0f;
        double quadV = difY ? ( iLocalCoords.y - p0Coords.y ) / difY : 0.0f;

        mTrajectoryBuffer.emplace_back( quad, quadU, quadV );

        return true;
    }

    return false;
}

void
FInbetweenerGrid::DeformPaths( std::vector<FInterpolatedPath>& iInterpolatedPathBuffer
                             , uint32 iInbetweenIndex )
{
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
    double quadArea = FOdysseyVector::Cross2D( ( mQuadBuffer[0].GetPoints()[1]->mSourcePosition - mQuadBuffer[0].GetPoints()[0]->mSourcePosition )
                                             , ( mQuadBuffer[0].GetPoints()[2]->mSourcePosition - mQuadBuffer[0].GetPoints()[1]->mSourcePosition ) );

    std::vector<TripletD> P_triplets;
    uint32 P_rows = 8 * mUsedQuadCount; // P_rows
    double triArea = quadArea * 0.5f;
    int triRow = 0;

    // Compute P (sparse) and store its transpose to construct the RHS of the equation later
    // TODO refactorize concatenation
    for( FInbetweenerQuad& quad : mQuadBuffer )
    {
        FInbetweenerPoint** points = quad.GetPoints();
        FInbetweenerPoint* triangleA[3] = { points[0], points[1], points[2] };
        FInbetweenerPoint* triangleB[3] = { points[2], points[3], points[0] };

        ComputePStar( triangleA, triRow, eInbetweenerPointPositionType::SourcePosition, P_triplets );
        triRow++;
        ComputePStar( triangleB, triRow, eInbetweenerPointPositionType::SourcePosition, P_triplets );
        triRow++;
    }

    for( FInbetweenerQuad& quad : mQuadBuffer )
    {
        FInbetweenerPoint** points = quad.GetPoints();
        FInbetweenerPoint* triangleA[3] = { points[0], points[1], points[2] };
        FInbetweenerPoint* triangleB[3] = { points[2], points[3], points[0] };

        ComputePStar( triangleA, triRow, eInbetweenerPointPositionType::TargetPosition, P_triplets );
        triRow++;
        ComputePStar( triangleB, triRow, eInbetweenerPointPositionType::TargetPosition, P_triplets );
        triRow++;
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
    uint32 constraintCount = mConstraintsIdx.size() > 0 ? mConstraintsIdx.size() : 1;
    uint32 idx = mUsedPointCount;
    Eigen::SparseMatrix<double, Eigen::ColMajor> PTP = mPt * mW.asDiagonal() * P;
    // Left Hand Side is a square matrix
    Eigen::SparseMatrix<double, Eigen::ColMajor> LHS( mUsedPointCount + constraintCount
                                                    , mUsedPointCount + constraintCount );

    // main constraint (linear interp of center of mass)
    // TODO: is there a more efficient way to do this than using the intermediate var PTP?
    LHS.innerVectors( 0, mUsedPointCount ) = PTP.innerVectors( 0, mUsedPointCount );

    if ( mConstraintsIdx.size() == 0 )
    {
        float constraintMean = mUsedPointCount ? 1.0f / mUsedPointCount : 0.0f;

        for ( uint32 i = 0; i < mUsedPointCount; ++i )
        {
            LHS.insert( idx, i ) = constraintMean;
            LHS.insert( i, idx ) = constraintMean;
        }
        ++idx;
    }

/*
    // user defined hard constraints
    for ( uint32 constraintIdx : mConstraintsIdx )
    {
        const Trajectory *traj = m_keyframe->trajectoryConstraintPtr(constraintIdx);
        const UVInfo &latticeCoord = traj->latticeCoord();

        QuadPtr quad = m_hashTable[latticeCoord.quadKey];
        // the constraint coeff vector and its transpose are set at the same time
        LHS.insert(idx, quad->corners[TOP_LEFT]->getKey()) = LHS.insert(quad->corners[TOP_LEFT]->getKey(), idx) = (1.0 - latticeCoord.uv.x()) * (1.0 - latticeCoord.uv.y());
        LHS.insert(idx, quad->corners[TOP_RIGHT]->getKey()) = LHS.insert(quad->corners[TOP_RIGHT]->getKey(), idx) = latticeCoord.uv.x() * (1.0 - latticeCoord.uv.y());
        LHS.insert(idx, quad->corners[BOTTOM_RIGHT]->getKey()) = LHS.insert(quad->corners[BOTTOM_RIGHT]->getKey(), idx) = latticeCoord.uv.x() * latticeCoord.uv.y();
        LHS.insert(idx, quad->corners[BOTTOM_LEFT]->getKey()) = LHS.insert(quad->corners[BOTTOM_LEFT]->getKey(), idx) = (1.0 - latticeCoord.uv.x()) * latticeCoord.uv.y();
        ++idx;
    }
*/

    // Factorization of LHS
    LHS.makeCompressed();
    mLU.compute( LHS );

    if ( mLU.info() != Eigen::Success )
    {
        UE_LOG(LogTemp, Warning, TEXT("ERROR DURING FACTORIZATION"));

        return false;
    }

    // Compute ref and target center of mass
    mSourceCenterOfMass = GetCenterOfMass( eInbetweenerPointPositionType::SourcePosition );
    mTargetCenterOfMass = GetCenterOfMass( eInbetweenerPointPositionType::TargetPosition );

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
FInbetweenerGrid::ComputeARAPInterpolation( float alphaLinear
                                          , float alpha
                                          //, const FInbetweenerPoint::Affine& globalRigidTransform
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
    double t = alpha;

    // Compute A(t)
    int i = 0;
    for ( FInbetweenerQuad& quad : mQuadBuffer )
    {
        ComputeQuadA( &quad, A, i, t, false );
    }

    for ( FInbetweenerQuad& quad : mQuadBuffer )
    {
        ComputeQuadA( &quad, A, i, t, true );
    }

    // Assembling final RHS matrix and concatenating constraints values
    unsigned int constraintCount = mConstraintsIdx.size() > 0 ? mConstraintsIdx.size() : 1;
    unsigned int idx = mUsedPointCount;
    Eigen::MatrixXd PTAD( mUsedPointCount + constraintCount, 2 );

    PTAD.block( 0, 0, mUsedPointCount, 2 ) = mPt * mW.asDiagonal() * A.transpose();

    // Main constraint (linear interp of center of mass)
    if ( mConstraintsIdx.size() == 0 )
    {
        PTAD( idx, 0 ) = mSourceCenterOfMass.x * ( 1.0f - t ) + mTargetCenterOfMass.x * t;
        PTAD( idx, 1 ) = mSourceCenterOfMass.y * ( 1.0f - t ) + mTargetCenterOfMass.y * t;
        ++idx;
    }

    // User defined constraints values
/*
    float offset;
    for ( uint32 constraintIdx : mConstraintsIdx )
    {
        Trajectory *traj = m_keyframe->trajectoryConstraintPtr(constraintIdx);
        traj->localOffset()->frameChanged(alphaLinear);
        offset = traj->localOffset()->get();
        Point::VectorType pos = traj->eval(t + (std::abs(offset) < 1e-5f ? 0.0f : offset));
        PTAD(idx, 0) = pos.x();
        PTAD(idx, 1) = pos.y();
        ++idx;
    }
*/

    Eigen::MatrixXd V = mLU.solve( PTAD ).eval();

    if ( mLU.info() != Eigen::Success )
    {
        UE_LOG(LogTemp, Warning, TEXT("ERROR DURING SOLVE"));

        return false;
    }

    // Setting new interpolated vertices in corners INTERP_POS coordinates
    for ( FInbetweenerPoint& point : mPointBuffer )
    {
        FInbetweenerPoint::VectorType coords = V.row( point.GetID() );

        point.SetMotionPosition( coords.x(), coords.y() );

/*
        if ( useRigidTransform )
        {
            FInbetweenerPoint::VectorType rigidPoint = globalRigidTransform * coords;

            point.SetMotionPosition( rigidPoint.x(), rigidPoint.y() );
        }
*/
    }

    //m_arapDirty = false;
    //sw.stop();

    return true;
}
