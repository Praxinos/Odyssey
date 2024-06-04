#include "OdysseyVectorTagInbetweener.h"
#include "OdysseyVectorSharedEnv.h"
#include "OdysseyVector.h"
#include "OdysseyVectorVertex.h"
#include "OdysseyVectorSegment.h"
#include "OdysseyVectorSegmentCubic.h"
#include "OdysseyVectorObject.h"
#include "OdysseyVectorPath.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorAnimationCell.h"

// for pow()
#include <unsupported/Eigen/MatrixFunctions>

#ifndef M_PI
#define M_PI 3.14159265358979323846L
#endif

FInterpolatedPoint::~FInterpolatedPoint()
{
}

FInterpolatedPoint::FInterpolatedPoint( FOdysseyVectorPoint* iPoint
                                      , uint32 iIndex
                                      , double iU
                                      , double iV  )
    : mOriginalPoint( iPoint )
    , mIndex ( iIndex )
    , mU( iU )
    , mV( iV )
{
}

FInterpolatedSegment::~FInterpolatedSegment()
{
}

FInterpolatedSegment::FInterpolatedSegment( FOdysseyVectorSegment* iSegment
                                          , FInterpolatedPoint* iInterpolatedPoint0
                                          , FInterpolatedPoint* iInterpolatedPoint1 )
    : mOriginalSegment( iSegment )
    , mInterpolatedVertex{ iInterpolatedPoint0, iInterpolatedPoint1 }
{
}

FInterpolatedSegmentCubic::~FInterpolatedSegmentCubic()
{
}

FInterpolatedSegmentCubic::FInterpolatedSegmentCubic( FOdysseyVectorSegmentCubic* iCubicSegment
                                                    , FInterpolatedPoint* iInterpolatedPoint0
                                                    , FInterpolatedPoint* iInterpolatedHandle0
                                                    , FInterpolatedPoint* iInterpolatedHandle1
                                                    , FInterpolatedPoint* iInterpolatedPoint1 )
    : FInterpolatedSegment( iCubicSegment, iInterpolatedPoint0, iInterpolatedPoint1 )
    , mInterpolatedHandle{ iInterpolatedHandle0, iInterpolatedHandle1 }
{
}

FInterpolatedPath::~FInterpolatedPath()
{
}

FInterpolatedPath::FInterpolatedPath( FOdysseyVectorPath* iPath
                                    , const ::ULIS::FRectD& iSpaceBBox
                                    , const BLMatrix2D& iSpaceInverseMatrix 
                                    , uint32 iInbetweenCount )
    : mOriginalPath( iPath )
{
    BLMatrix2D conversionMatrix;
    uint32 pointID = 0;
    uint32 segmentID = 0;

    FOdysseyVector::MatrixMultiply( iSpaceInverseMatrix
                                  , iPath->GetWorldMatrix()
                                  , conversionMatrix );

                                       // alloc 1 point per vertex
    mInterpolatedPointBuffer.reserve(  iPath->GetVertexList().size()
                                       // alloc 2 handles per segment
                                   + ( iPath->GetSegmentList().size() * 2 ) );

    for( FOdysseyVectorVertex* vertex : iPath->GetVertexList() )
    {
        BLPoint pt = conversionMatrix.mapPoint( vertex->GetX(), vertex->GetY() );
        double spaceX = pt.x - iSpaceBBox.x;
        double spaceY = pt.y - iSpaceBBox.y;
        //double u = std::clamp<double>( spaceX / iSpaceBBox.w, 0.0f, 1.0f );
        //double v = std::clamp<double>( spaceY / iSpaceBBox.h, 0.0f, 1.0f );
        double u = std::clamp<double>( spaceX / iSpaceBBox.w, 0.0f, 1.0f );
        double v = std::clamp<double>( spaceY / iSpaceBBox.h, 0.0f, 1.0f );

            // Note: we add +1 for the target position
        mInterpolatedPointBuffer.emplace_back( vertex, mInterpolatedPointBuffer.size(), u, v );

        vertex->SetID( pointID++ );
    }

    for( FOdysseyVectorSegment* segment : iPath->GetSegmentList() )
    {
        FOdysseyVectorVertex* vertex0 = segment->GetVertex(0);
        FOdysseyVectorVertex* vertex1 = segment->GetVertex(1);

        if( segment->GetClass() == FOdysseyVectorSegmentCubic::StaticClass() )
        {
            FOdysseyVectorSegmentCubic* cubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(segment);
            FOdysseyVectorHandleSegment* handle0 = cubicSegment->GetHandle(0);
            FOdysseyVectorHandleSegment* handle1 = cubicSegment->GetHandle(1);
            BLPoint pt0 = conversionMatrix.mapPoint( handle0->GetX(), handle0->GetY() );
            BLPoint pt1 = conversionMatrix.mapPoint( handle1->GetX(), handle1->GetY() );
            // compute UV for first handle
            double space0X = pt0.x - iSpaceBBox.x;
            double space0Y = pt0.y - iSpaceBBox.y;
            double u0 = std::clamp<double>( space0X / iSpaceBBox.w, 0.0f, 1.0f );
            double v0 = std::clamp<double>( space0Y / iSpaceBBox.h, 0.0f, 1.0f );
            // compute UV for second handle
            double space1X = pt1.x - iSpaceBBox.x;
            double space1Y = pt1.y - iSpaceBBox.y;
            double u1 = std::clamp<double>( space1X / iSpaceBBox.w, 0.0f, 1.0f );
            double v1 = std::clamp<double>( space1Y / iSpaceBBox.h, 0.0f, 1.0f );

            // Note: we add +1 for the target position
            mInterpolatedPointBuffer.emplace_back( handle0, mInterpolatedPointBuffer.size(), u0, v0 );
            handle0->SetID( pointID++ );
            // Note: we add +1 for the target position
            mInterpolatedPointBuffer.emplace_back( handle1, mInterpolatedPointBuffer.size(), u1, v1 );
            handle1->SetID( pointID++ );

            mInterpolatedSegmentCubicBuffer.emplace_back( cubicSegment
                                                        , &mInterpolatedPointBuffer[vertex0->GetID()]
                                                        , &mInterpolatedPointBuffer[handle0->GetID()]
                                                        , &mInterpolatedPointBuffer[handle1->GetID()]
                                                        , &mInterpolatedPointBuffer[vertex1->GetID()] );

            cubicSegment->SetID( segmentID++ );
        }
    }
}

std::vector<::ULIS::FVec2D>&
FInterpolatedPath::GetInterpolatedPointPositionBuffer()
{
    return mInterpolatedPointPositionBuffer;
}

std::vector<FInterpolatedPoint>&
FInterpolatedPath::GetInterpolatedPointBuffer()
{
    return mInterpolatedPointBuffer;
}

/*
uint32
FOdysseyVectorTagInbetweener::MapPoint( FOdysseyVectorObject* iObject
                                      , FOdysseyVectorPoint* iPoint
                                      , double iSpaceX
                                      , double iSpaceY )
{
    double paramX = iSpaceX / mSelectionBox.rect.w;
    double paramY = iSpaceY / mSelectionBox.rect.h;

    if( ( paramX >= 0.0f ) && ( paramX < 1.0f ) && ( paramY >= 0.0f ) && ( paramY < 1.0f ) )
    {
        uint32 rowid = paramX * mGridTool->DivisionsX;
        uint32 colid = paramY * mGridTool->DivisionsY;
        uint32 offset = ( colid * mGridTool->DivisionsX ) + rowid;
        double s = ( iSpaceX - (double) rowid * mCellSizeX ) / mCellSizeX;
        double t = ( iSpaceY - (double) colid * mCellSizeY ) / mCellSizeY;
        FInbetweenerGridPoint gridPoint = { iObject, iPoint, s, t };

        mCellArray[offset].mPointArray.push_back( gridPoint );

        return 1;
    }

    return 0;
}
*/

FInbetweenerGridPoint::FInbetweenerGridPoint()
{
}

void
FInbetweenerGridPoint::Init( FInbetweenerGrid* iGrid )
{
    mGrid = iGrid;
}

void
FInbetweenerGridPoint::SetSourcePosition( double iX, double iY )
{
    mSourcePosition.x = iX;
    mSourcePosition.y = iY;

    mGrid->GetInbetweenerTag()->Invalidate( FOdysseyVectorTagInbetweener::INVALIDATE_BBOX );
}

void
FInbetweenerGridPoint::SetTargetPosition( double iX, double iY )
{
    mTargetPosition.x = iX;
    mTargetPosition.y = iY;

    mGrid->GetInbetweenerTag()->Invalidate( FOdysseyVectorTagInbetweener::INVALIDATE_BBOX );
}

void
FInbetweenerGridPoint::SetMotionPosition( double iX, double iY )
{
    mMotionPosition.x = iX;
    mMotionPosition.y = iY;
}

const ::ULIS::FVec2D&
FInbetweenerGridPoint::GetSourcePosition()
{
    return mSourcePosition;
}

const ::ULIS::FVec2D&
FInbetweenerGridPoint::GetTargetPosition()
{
    return mTargetPosition;
}

void
FInbetweenerGridPoint::AddQuad( FInbetweenerGridQuad* iQuad )
{
    mQuadList.push_back( iQuad );
}

void
FInbetweenerGridPoint::RemoveQuad( FInbetweenerGridQuad* iQuad )
{
    mQuadList.remove( iQuad );
}

std::list<FInbetweenerGridQuad*>&
FInbetweenerGridPoint::GetQuadList()
{
    return mQuadList;
}

void
FInbetweenerGridPoint::SetID( uint32 iID )
{
    mID = iID;
}

uint32
FInbetweenerGridPoint::GetID()
{
    return mID;
}

::ULIS::FVec2D
FInbetweenerGridPoint::GetPosition( eGridPointPositionType iPositionType )
{
    switch( iPositionType )
    {
        case eGridPointPositionType::SourcePosition : 
        return mSourcePosition;

        case eGridPointPositionType::MotionPosition : 
        return mMotionPosition;

        case eGridPointPositionType::TargetPosition : 
        return mTargetPosition;

        default:
        break;
    }

    return ::ULIS::FVec2D( 0.0f, 0.0f );
}

FInbetweenerGridQuad::FInbetweenerGridQuad( )
    : mFlags ( 0 )
{
}

FInbetweenerGridPoint** 
FInbetweenerGridQuad::GetPoints()
{
    return mPoint;
}

bool
FInbetweenerGridQuad::IsLinked()
{
    return ( mFlags & LINKED ) ? true : false; 
}

void
FInbetweenerGridQuad::Link()
{
    mFlags |= LINKED;

    for( uint32 i = 0; i < 4; i++ )
    {
        mPoint[i]->AddQuad( this );
    }
}

void
FInbetweenerGridQuad::Unlink()
{
    for( uint32 i = 0; i < 4; i++ )
    {
        mPoint[i]->RemoveQuad( this );
    }

    mFlags &= (~LINKED);
}

FInbetweenerGrid::FInbetweenerGrid( FOdysseyVectorTagInbetweener* iInbetweenerTag
                                  , uint32 iNumQuadX
                                  , uint32 iNumQuadY )
    : mNumQuadX( iNumQuadX )
    , mNumQuadY( iNumQuadY )
    , mInbetweenerTag( iInbetweenerTag )
    , mUsedQuadCount( 0 )
    , mUsedPointCount( 0 )
    , mQuadArea( 0 )
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
FInbetweenerGrid::GetCenterOfMass( eGridPointPositionType iPositionType )
{
    ::ULIS::FVec2D center = ::ULIS::FVec2D( 0.0f, 0.0f );
    uint32 pointCount = 0;

    for ( FInbetweenerGridPoint& point : mPointBuffer )
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
                FInbetweenerGridQuad* quad = &mQuadBuffer[quadOffset];
                FInbetweenerGridPoint** gridPoint = quad->GetPoints();

                gridPoint[0] = &mPointBuffer[vertexOffset];
                gridPoint[1] = &mPointBuffer[vertexOffset+1];
                gridPoint[2] = &mPointBuffer[vertexOffset+1+numVertexX];
                gridPoint[3] = &mPointBuffer[vertexOffset+numVertexX];

                quad->Link();
            }
        }

        for( FInbetweenerGridPoint& point : mPointBuffer )
        {
            if( point.GetQuadList().size() )
            {
                point.SetID( pointID++ );
            }
        }

        mUsedQuadCount = mQuadBuffer.size();
        mUsedPointCount = pointID;

        mQuadArea = FOdysseyVector::Cross2D( ( mQuadBuffer[0].GetPoints()[1]->mSourcePosition - mQuadBuffer[0].GetPoints()[0]->mSourcePosition )
                                           , ( mQuadBuffer[0].GetPoints()[2]->mSourcePosition - mQuadBuffer[0].GetPoints()[1]->mSourcePosition ) );
    }
}

FInbetweenerGridFFD::FInbetweenerGridFFD( FOdysseyVectorTagInbetweener* iInbetweenerTag
                                        , uint32 iNumQuadX
                                        , uint32 iNumQuadY )
    : FInbetweenerGrid( iInbetweenerTag, iNumQuadX, iNumQuadY )
{
}

void
FInbetweenerGrid::DeformPaths( std::vector<FInterpolatedPath>& iInterpolatedPathBuffer
                             , uint32 iInbetweenIndex )
{
}

::ULIS::FVec2D
FInbetweenerGridFFD::DeformPoint( FInterpolatedPoint* iInterpolatedPoint )
{
    ::ULIS::FRectD bbox = mInbetweenerTag->GetOwner()->GetBBox( false );
    ::ULIS::FVec2D vi = ::ULIS::FVec2D( 0.0f, 0.0f );
    uint32 numVertexX = mNumQuadX + 1;
    uint32 numVertexY = mNumQuadY + 1;

    for ( uint32 i = 0; i < numVertexY; i++ )
    {
        ::ULIS::FVec2D vj = ::ULIS::FVec2D( 0.0f, 0.0f );
        double bcv = mVBinomialCoefficientBuffer[i];

        for ( uint32 j = 0; j < numVertexX; j++ )
        {
            double bcu = mUBinomialCoefficientBuffer[j];
            uint32 offset = ( i * numVertexX ) + j;

            vj.x += ( bcu * pow ( ( 1 - iInterpolatedPoint->mU ), (mNumQuadX) - j ) * pow ( iInterpolatedPoint->mU, j ) * mPointBuffer[offset].u );
            vj.y += ( bcu * pow ( ( 1 - iInterpolatedPoint->mU ), (mNumQuadX) - j ) * pow ( iInterpolatedPoint->mU, j ) * mPointBuffer[offset].v );
        }

        vi.x += ( bcv * pow ( ( 1 - iInterpolatedPoint->mV ), (mNumQuadY) - i ) * pow ( iInterpolatedPoint->mV, i ) * vj.x );
        vi.y += ( bcv * pow ( ( 1 - iInterpolatedPoint->mV ), (mNumQuadY) - i ) * pow ( iInterpolatedPoint->mV, i ) * vj.y );
    }

    return ::ULIS::FVec2D( ( bbox.x + ( bbox.w * vi.x ) )
                         , ( bbox.y + ( bbox.h * vi.y ) ) );
}

void
FInbetweenerGridFFD::DeformPaths( std::vector<FInterpolatedPath>& iInterpolatedPathBuffer
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

std::vector<FInbetweenerGridQuad>&
FInbetweenerGrid::GetQuadBuffer()
{
    return mQuadBuffer;
}

std::vector<FInbetweenerGridPoint>&
FInbetweenerGrid::GetPointBuffer()
{
    return mPointBuffer;
}

void
FInbetweenerGridFFD::Make( uint32 iNumQuadX, uint32 iNumQuadY )
{
    FInbetweenerGrid::Make( iNumQuadX, iNumQuadY );

    if( iNumQuadX && iNumQuadY )
    {
        mUBinomialCoefficientBuffer.resize( mPointBuffer.size() );
        mVBinomialCoefficientBuffer.resize( mPointBuffer.size() );

        ComputeBinomialCoefficients();
    }
}

FInbetweenerGridARAP::FInbetweenerGridARAP( FOdysseyVectorTagInbetweener* iInbetweenerTag
                                          , uint32 iNumQuadX
                                          , uint32 iNumQuadY )
    : FInbetweenerGrid( iInbetweenerTag, iNumQuadX, iNumQuadY )
{
}

void
FInbetweenerGridARAP::Make( uint32 iNumQuadX, uint32 iNumQuadY )
{
    FInbetweenerGrid::Make( iNumQuadX, iNumQuadY );
}

/** 
 * Compute P* for the two triangles of the given quad and add them to the sparse matrix P (via the triplet list)
 * See Baxter et al. 2008
 */
void
FInbetweenerGridARAP::ComputePStar( FInbetweenerGridPoint* iTriangle[3]
                                  , int triRow
                                  , eGridPointPositionType iPositionType
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
FInbetweenerGridARAP::Precompute()
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
    for( FInbetweenerGridQuad& quad : mQuadBuffer )
    {
        FInbetweenerGridPoint** points = quad.GetPoints();
        FInbetweenerGridPoint* triangleA[3] = { points[0], points[1], points[2] };
        FInbetweenerGridPoint* triangleB[3] = { points[2], points[3], points[0] };

        ComputePStar( triangleA, triRow, eGridPointPositionType::SourcePosition, P_triplets );
        triRow++;
        ComputePStar( triangleB, triRow, eGridPointPositionType::SourcePosition, P_triplets );
        triRow++;
    }

    for( FInbetweenerGridQuad& quad : mQuadBuffer )
    {
        FInbetweenerGridPoint** points = quad.GetPoints();
        FInbetweenerGridPoint* triangleA[3] = { points[0], points[1], points[2] };
        FInbetweenerGridPoint* triangleB[3] = { points[2], points[3], points[0] };

        ComputePStar( triangleA, triRow, eGridPointPositionType::TargetPosition, P_triplets );
        triRow++;
        ComputePStar( triangleB, triRow, eGridPointPositionType::TargetPosition, P_triplets );
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
    mSourceCenterOfMass = GetCenterOfMass( eGridPointPositionType::SourcePosition );
    mTargetCenterOfMass = GetCenterOfMass( eGridPointPositionType::TargetPosition );

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
void FInbetweenerGridARAP::ComputeJAM( FInbetweenerGridPoint* iTriangle[3]
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
FInbetweenerGridARAP::PolarDecomp( Eigen::Matrix2d& iA, Eigen::Matrix2d& oS )
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
FInbetweenerGridARAP::ComputeQuadA( FInbetweenerGridQuad* iQuad
                                  , Eigen::MatrixXd& iAt
                                  , int &i
                                  , float iT
                                  , bool iInverseOrientation )
{
    FInbetweenerGridPoint** points = iQuad->GetPoints();
    FInbetweenerGridPoint* triangle0[3] = { points[0], points[1], points[2] };
    FInbetweenerGridPoint* triangle1[3] = { points[2], points[3], points[0] };
    Eigen::Matrix2d A_interp, I;
    Eigen::Matrix2d A, Rt, S;
    double angle;

    I = Eigen::Matrix2d::Identity();

    if ( iInverseOrientation ) iT = 1.0f - iT;

    // Compute and concatenate the interpolated linear transformation of the triangle
    // formed by CornerA, CornerB and the bottom left corner of the quad
    auto computeTriangleA = [&]( FInbetweenerGridPoint* iTriangle[3] )
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
FInbetweenerGridARAP::InterpolateARAP( float alphaLinear
                                     , float alpha
                                     //, const FInbetweenerGridPoint::Affine& globalRigidTransform
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
    for ( FInbetweenerGridQuad& quad : mQuadBuffer )
    {
        ComputeQuadA( &quad, A, i, t, false );
    }

    for ( FInbetweenerGridQuad& quad : mQuadBuffer )
    {
        ComputeQuadA( &quad, A, i, t, true );
    }

    // Assembling final RHS matrix and concatenating constraints values
    unsigned int constraintCount = mConstraintsIdx.size() > 0 ? mConstraintsIdx.size() : 1;
    unsigned int idx = mUsedPointCount;
    Eigen::MatrixXd PTAD( mUsedPointCount + constraintCount, 2 );

    PTAD.block( 0, 0, mUsedPointCount, 2 ) = mPt * mW.asDiagonal() * A.transpose();

    // Main constraint (linear interp of center of mass)
    if ( constraintCount == 0 )
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
    for ( FInbetweenerGridPoint& point : mPointBuffer )
    {
        FInbetweenerGridPoint::VectorType coords = V.row( point.GetID() );

        point.SetMotionPosition( coords.x(), coords.y() );

/*
        if ( useRigidTransform )
        {
            FInbetweenerGridPoint::VectorType rigidPoint = globalRigidTransform * coords;

            point.SetMotionPosition( rigidPoint.x(), rigidPoint.y() );
        }
*/
    }

    //m_arapDirty = false;
    //sw.stop();

    return true;
}


void
FOdysseyVectorTagInbetweener::UpdateGridBBox()
{
    double xmin = DBL_MAX, ymin = DBL_MAX, xmax = -DBL_MAX, ymax = -DBL_MAX;
    bool hasBBox = false;

    for( FInbetweenerGridPoint& gridPoint : mGrid->GetPointBuffer() )
    {
        const ::ULIS::FVec2D& targetPosition = gridPoint.GetTargetPosition();

        hasBBox = true;

        if ( targetPosition.x < xmin ) xmin = targetPosition.x;
        if ( targetPosition.y < ymin ) ymin = targetPosition.y;
        if ( targetPosition.x > xmax ) xmax = targetPosition.x;
        if ( targetPosition.y > ymax ) ymax = targetPosition.y;
    }

    mTargetGridBBox = ( hasBBox ) ? ::ULIS::FRectD::FromMinMax( xmin, ymin, xmax, ymax ) 
                                  : ::ULIS::FRectD( 0.0f, 0.0f, 0.0f, 0.0f );
}

void
FOdysseyVectorTagInbetweener::Map()
{
    FOdysseyVectorEngine* vectorEngine = mOwner->GetEngine();
    uint32 pathCount = 0;

    mInterpolatedPathBuffer.clear();

    vectorEngine->Traverse
    ( mOwner
    , 0
    , [ this
      , &pathCount ]( FOdysseyVectorObject* object, uint64 travesalFlags ) -> uint64
      {
          FOdysseyVectorTag* objectInbetweenerTag = object->GetTagByType( FOdysseyVectorTagInbetweener::StaticClass() );

          if( ( objectInbetweenerTag == nullptr ) || ( objectInbetweenerTag == this ) )
          {
              if( object->GetClass() == FOdysseyVectorPath::StaticClass() )
              {
                  pathCount++;

                  return FOdysseyVectorEngine::TRAVERSE_OBJECT_ACCEPTED;
              }
          }
          else
          {
              return FOdysseyVectorEngine::TRAVERSE_OBJECT_IGNORE_CHILDREN;
          }

          return 0;
      } );

    mInterpolatedPathBuffer.reserve( pathCount );

    vectorEngine->Traverse
    ( mOwner
    , 0
    , [ this ]( FOdysseyVectorObject* object, uint64 travesalFlags ) -> uint64
      {
          BLMatrix2D& inverseSpaceMatrix = mOwner->GetInverseWorldMatrix();
          FOdysseyVectorTag* objectInbetweenerTag = object->GetTagByType( FOdysseyVectorTagInbetweener::StaticClass() );

          if( ( objectInbetweenerTag == nullptr ) || ( objectInbetweenerTag == this ) )
          {
              if( object->GetClass() == FOdysseyVectorPath::StaticClass() )
              {
                  FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(object);

                  mInterpolatedPathBuffer.emplace_back( path
                                                      , mOwner->GetBBox( false )
                                                      , inverseSpaceMatrix
                                                      , mInbetweenCount );

                  return FOdysseyVectorEngine::TRAVERSE_OBJECT_ACCEPTED;
              }
          }
          else
          {
              return FOdysseyVectorEngine::TRAVERSE_OBJECT_IGNORE_CHILDREN;
          }

          return 0;
      } );
}

FOdysseyVectorTagInbetweener::~FOdysseyVectorTagInbetweener()
{
    delete mGrid;
}

FOdysseyVectorTagInbetweener::FOdysseyVectorTagInbetweener( FOdysseyVectorSharedEnv* iSharedEnv
                                                          , FOdysseyVectorObject* iOwnerObject
                                                          , uint32 iNumQuadX
                                                          , uint32 iNumQuadY
                                                          , uint32 iInbetweenCount )
    : FOdysseyVectorTag( iOwnerObject )
    // note: mSharedEnv is remebered as a member variable because GetEngine() calls
    // GetClass() and the latter is a virtual function. virtual function don't work
    // in destructors.
    , mSharedEnv ( iSharedEnv )
    , mGrid( nullptr )
    , mGridType( eInbetweenerGridType::FFD )
    , mInbetweenCount( iInbetweenCount )
    , mInvalidationFlags( 0 )
{
    mTargetTranslationX = 0.0f;
    mTargetTranslationY = 0.0f;
    mTargetScalingX     = 1.0f;
    mTargetScalingY     = 1.0f;
    mTargetRotation     = 0.0f;

    UpdateMatrix();

    SetGridType( mGridType );

    //mGrid->Make( iNumQuadX, iNumQuadY, iOwnerObject->GetBBox( false ), this );

    //Map();
    //ResetChart();
    //AllocBuffers();
    //Interpolate();
}

void
FOdysseyVectorTagInbetweener::Translate( double iX, double iY )
{
    mTargetTranslationX = iX;
    mTargetTranslationY = iY;
}

void
FOdysseyVectorTagInbetweener::Rotate( double iAngle )
{
    mTargetRotation = iAngle;
}

void
FOdysseyVectorTagInbetweener::Scale( double iX, double iY )
{
    mTargetScalingX = iX;
    mTargetScalingY = iY;
}

void FOdysseyVectorTagInbetweener::Added()
{
    mSharedEnv->AddTag( this );
}

void FOdysseyVectorTagInbetweener::Removed()
{
    mSharedEnv->RemoveTag( this );
}

void FOdysseyVectorTagInbetweener::Update( uint32 iUpdateFlags )
{
    if( ( iUpdateFlags & FOdysseyVectorObject::INVALIDATE_HIERARCHY )

     || ( iUpdateFlags & FOdysseyVectorObject::INVALIDATE_SHAPE    )
     || ( iUpdateFlags & FOdysseyVectorObject::INVALIDATE_TOPOLOGY )
     || ( iUpdateFlags & FOdysseyVectorObject::INVALIDATE_TAGS     )
     || ( iUpdateFlags & FOdysseyVectorObject::INVALIDATE_MATRIX   )

     || ( iUpdateFlags & FOdysseyVectorObject::INVALIDATE_CHILD_SHAPE    )
     || ( iUpdateFlags & FOdysseyVectorObject::INVALIDATE_CHILD_TOPOLOGY )
     || ( iUpdateFlags & FOdysseyVectorObject::INVALIDATE_CHILD_TAGS     )
     || ( iUpdateFlags & FOdysseyVectorObject::INVALIDATE_CHILD_MATRIX   ) )
    {
        mInvalidationFlags |= ( INVALIDATE_MAP | INVALIDATE_BUFFERS | INVALIDATE_SPACING );
    }

    if( mInvalidationFlags & INVALIDATE_MAP )
    {
        Map();
    }

    if( mInvalidationFlags & INVALIDATE_BUFFERS )
    {
        AllocBuffers();
    }

    if( mInvalidationFlags & INVALIDATE_BBOX )
    {
        UpdateGridBBox();
    }

    if( mInvalidationFlags & INVALIDATE_SPACING )
    {
        Interpolate();
    }

    if( mInvalidationFlags & INVALIDATE_CELLS )
    {
        UpdateAnimationCells();
    }

    // reset tag's invalidation flags (do not confuse with object's invalidation flags)
    mInvalidationFlags = 0;
}

void
FOdysseyVectorTagInbetweener::Invalidate( uint64 iInvalidationFlags )
{
    mOwner->Invalidate( FOdysseyVectorObject::INVALIDATE_TAGS );

    mInvalidationFlags |= iInvalidationFlags;
}

BLMatrix2D&
FOdysseyVectorTagInbetweener::GetTargetWorldMatrix()
{
    return mTargetWorldMatrix;
}

BLMatrix2D&
FOdysseyVectorTagInbetweener::GetTargetInverseWorldMatrix()
{
    return mTargetInverseWorldMatrix;
}

::ULIS::FRectD
FOdysseyVectorTagInbetweener::GetTargetGridBBox( bool iWorld )
{
    if ( iWorld == true )
    {
        BLPoint p0 = mTargetWorldMatrix.mapPoint( mTargetGridBBox.x                    , mTargetGridBBox.y                     );
        BLPoint p1 = mTargetWorldMatrix.mapPoint( mTargetGridBBox.x + mTargetGridBBox.w, mTargetGridBBox.y                     );
        BLPoint p2 = mTargetWorldMatrix.mapPoint( mTargetGridBBox.x + mTargetGridBBox.w, mTargetGridBBox.y + mTargetGridBBox.h );
        BLPoint p3 = mTargetWorldMatrix.mapPoint( mTargetGridBBox.x                    , mTargetGridBBox.y + mTargetGridBBox.h );
        ::ULIS::FRectD worldBBox = ::ULIS::FRectD::FromMinMax( ::ULIS::FMath::Min4( p0.x, p1.x, p2.x, p3.x )
                                                             , ::ULIS::FMath::Min4( p0.y, p1.y, p2.y, p3.y )
                                                             , ::ULIS::FMath::Max4( p0.x, p1.x, p2.x, p3.x )
                                                             , ::ULIS::FMath::Max4( p0.y, p1.y, p2.y, p3.y ) );

        return worldBBox;
    }

    return mTargetGridBBox;
}

void
FOdysseyVectorTagInbetweener::ResetChart()
{
    float step = 1.0f / ( mInbetweenCount + 1 );
    float spacing = step;

    mChart.inbetweenBuffer.clear();
    // Note: +1 for target position
    mChart.inbetweenBuffer.resize( mInbetweenCount + 1 );

    for( uint32 i = 0; i <= mInbetweenCount; i++ )
    {
        mChart.inbetweenBuffer[i].spacing = spacing;

        spacing += step;
    }
}

FInbetweenerChart&
FOdysseyVectorTagInbetweener::GetChart()
{
    return mChart;
}

void
FOdysseyVectorTagInbetweener::UpdateMatrix()
{
    mTargetLocalMatrix.reset();
    mTargetLocalMatrix.translate( mTargetTranslationX, mTargetTranslationY );
    mTargetLocalMatrix.rotate( mTargetRotation * M_PI / 180.0f );
    mTargetLocalMatrix.scale( mTargetScalingX, mTargetScalingY );

    mTargetWorldMatrix = mOwner->GetWorldMatrix();
    mTargetWorldMatrix.transform( mTargetLocalMatrix );

    BLMatrix2D::invert( mTargetInverseWorldMatrix, mTargetWorldMatrix );

    for( uint32 inbetweenIndex = 0; inbetweenIndex < mInbetweenCount; inbetweenIndex++ )
    {
        InterpolateTransform( inbetweenIndex );
    }

    Invalidate( INVALIDATE_CELLS );
}

static
int Factorial ( int n )
{
    return ( n <= 1 ) ? 1 : n * Factorial ( n - 1 ); 
}

static
double BinomialCoeff ( int n, int k )
{
    int div = Factorial ( k ) * Factorial ( n - k );

    if ( div == 0 ) return 0.0f;

    return ( double ) Factorial ( n ) / div;
}

// precompute binaomial coefficient
void
FInbetweenerGridFFD::ComputeBinomialCoefficients()
{
    uint32 numVertexX = mNumQuadX + 1;
    uint32 numVertexY = mNumQuadY + 1;

    for ( uint32 i = 0; i < numVertexX; i++ )
    {
        double coeffU = BinomialCoeff ( mNumQuadX, i );

        mUBinomialCoefficientBuffer[i] = coeffU;
    }

    for ( uint32 i = 0; i < numVertexY; i++ )
    {
        double coeffV = BinomialCoeff ( mNumQuadY, i );

        mVBinomialCoefficientBuffer[i] = coeffV;
    }
}

void
FOdysseyVectorTagInbetweener::InterpolateGeometry( uint32 iInbetweenIndex )
{
    ::ULIS::FRectD bbox = mOwner->GetBBox( false );

    if( mGridType == eInbetweenerGridType::FFD )
    {
        for( FInbetweenerGridPoint& point : mGrid->GetPointBuffer() )
        {
            ::ULIS::FVec2D diff = ( point.mTargetPosition - point.mSourcePosition );
            double t = mChart.inbetweenBuffer[iInbetweenIndex].spacing;
            ::ULIS::FVec2D step = diff * t;

            point.mMotionPosition = point.mSourcePosition + step;

            point.u = ( point.mMotionPosition.x - bbox.x ) / bbox.w;
            point.v = ( point.mMotionPosition.y - bbox.y ) / bbox.h;
        }
    }

    if( mGridType == eInbetweenerGridType::ARAP )
    {
        FInbetweenerGridARAP* gridARAP = static_cast<FInbetweenerGridARAP*>(mGrid);
        double t = mChart.inbetweenBuffer[iInbetweenIndex].spacing;

        gridARAP->InterpolateARAP( t
                                 , t
                                // , const FInbetweenerGridPoint::Affine &globalRigidTransform
                                 , false );
    }

    // deform the path according to grid geometry
    mGrid->DeformPaths( mInterpolatedPathBuffer, iInbetweenIndex );
}

void
FOdysseyVectorTagInbetweener::UpdateAnimationCells()
{
    UpdateAnimationCells( mInbetweenCount );
}


void
FOdysseyVectorTagInbetweener::UpdateAnimationCells( uint32 iInbetweenCount )
{
    IOdysseyVectorAnimationCell* animationCell = mOwner->GetScene()->GetEngine()->GetAnimationCell();

    if( animationCell )
    {
        int32 animationCellIndex = animationCell->GetIndex();

        // Redraw impacted cells
        for( uint32 i = 0; ( i < iInbetweenCount ) && ( animationCell != nullptr ); i++ )
        {
            IOdysseyVectorAnimationCell* nextAnimationCell = animationCell->GetCellByIndex( animationCellIndex + i + 1 );

            if( nextAnimationCell )
            {
                //nextAnimationCell->GetEngine()->Invalidate();
                nextAnimationCell->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );
            }

            animationCell = nextAnimationCell;
        }
    }
}

void
FOdysseyVectorTagInbetweener::InterpolateTransform( uint32 iInbetweenIndex )
{
    FInbetweenerInbetween* inbetween = &mChart.inbetweenBuffer[iInbetweenIndex];
    double translationX, translationY, rotation, scalingX, scalingY;
    double spacing = inbetween->spacing;

    translationX = mTargetTranslationX * spacing;
    translationY = mTargetTranslationY * spacing;
    rotation = mTargetRotation * spacing;
    scalingX = 1.0f + ( ( mTargetScalingX - 1.0f ) * spacing );
    scalingY = 1.0f + ( ( mTargetScalingY - 1.0f ) * spacing );

    inbetween->matrix.reset();
    inbetween->matrix.translate( translationX, translationY );
    inbetween->matrix.rotate( rotation );
    inbetween->matrix.scale( scalingX, scalingY );
}

void
FOdysseyVectorTagInbetweener::Interpolate()
{
    if( mGridType == eInbetweenerGridType::ARAP )
    {
        FInbetweenerGridARAP* gridARAP = static_cast<FInbetweenerGridARAP*>(mGrid);

        gridARAP->Precompute();
    }

    for( uint32 i = 0; i <= mInbetweenCount; i++ )
    {
        InterpolateGeometry( i );
        InterpolateTransform( i );
    }
}

uint32
FOdysseyVectorTagInbetweener::GetInbetweenCount()
{
    return mInbetweenCount;
}

void
FOdysseyVectorTagInbetweener::DrawMotionGrid( BLContext* iBLContext
                                            , const ::ULIS::FRectD& iInvalidationArea
                                            , double iAncestorsOpacity
                                            , uint64 iDrawingFlags )
{
    BLMatrix2D worldMatrix = mOwner->GetWorldMatrix();

    iBLContext->save();

    iBLContext->setStrokeStyle( BLRgba32( 255, 0, 255, 255 ) );
    iBLContext->setStrokeWidth( 1.0f );

    for( FInbetweenerGridQuad& quad : mGrid->GetQuadBuffer() )
    {
        FInbetweenerGridPoint** gridPoint = quad.GetPoints();
        BLPoint pt[4] = { worldMatrix.mapPoint( gridPoint[0]->GetPosition( eGridPointPositionType::MotionPosition ).x
                                              , gridPoint[0]->GetPosition( eGridPointPositionType::MotionPosition ).y )
                        , worldMatrix.mapPoint( gridPoint[1]->GetPosition( eGridPointPositionType::MotionPosition ).x
                                              , gridPoint[1]->GetPosition( eGridPointPositionType::MotionPosition ).y )
                        , worldMatrix.mapPoint( gridPoint[2]->GetPosition( eGridPointPositionType::MotionPosition ).x
                                              , gridPoint[2]->GetPosition( eGridPointPositionType::MotionPosition ).y )
                        , worldMatrix.mapPoint( gridPoint[3]->GetPosition( eGridPointPositionType::MotionPosition ).x
                                              , gridPoint[3]->GetPosition( eGridPointPositionType::MotionPosition ).y ) };

        iBLContext->strokeLine( pt[0], pt[1] );
        iBLContext->strokeLine( pt[1], pt[2] );
        iBLContext->strokeLine( pt[2], pt[3] );
        iBLContext->strokeLine( pt[3], pt[0] );
    }

    iBLContext->restore();
}

void
FOdysseyVectorTagInbetweener::Draw( BLContext* iBLContext
                                  , const ::ULIS::FRectD& iInvalidationArea
                                  , double iAncestorsOpacity
                                  , uint64 iDrawingFlags )
{
    //DrawPaths( iBLContext, iInvalidationArea, iAncestorsOpacity, iDrawingFlags );

    //DrawGrid( iBLContext, iInvalidationArea, iAncestorsOpacity, iDrawingFlags );
}

// when drawn as a shared tag
void
FOdysseyVectorTagInbetweener::Draw( FOdysseyVectorGroupPaint* iDisplayedScene
                                  , BLContext* iBLContext
                                  , const ::ULIS::FRectD& iInvalidationArea
                                  , double iAncestorsOpacity
                                  , uint64 iDrawingFlags )
{
    IOdysseyVectorAnimationCell* displayedCell = iDisplayedScene->GetEngine()->GetAnimationCell();

    // check the object is still displayed (it could have been removed but still in memory)
    if( mOwner->GetScene() )
    {
        IOdysseyVectorAnimationCell* tagCell = mOwner->GetScene()->GetEngine()->GetAnimationCell();

        iBLContext->save();
        iBLContext->resetMatrix();

        iBLContext->setStrokeStyle( BLRgba32( 0, 0, 0, 255 ) );
        iBLContext->setStrokeWidth( 3.0f );

        // if th eobject hasn't been removed from the scene
        if( displayedCell && tagCell )
        {
            uint32 tagCellIndex = tagCell->GetIndex();
            uint32 displayedCellIndex = displayedCell->GetIndex();

            if ( ( displayedCellIndex >    tagCellIndex                     )
              && ( displayedCellIndex <= ( tagCellIndex + mInbetweenCount ) ) )
            {
                DrawPathsInbetween( displayedCellIndex - tagCellIndex - 1, iBLContext );
            }
        }

        iBLContext->restore();

        DrawMotionGrid( iBLContext, iInvalidationArea, iAncestorsOpacity, iDrawingFlags );
    }
}

void
FOdysseyVectorTagInbetweener::DrawPathAt( FInterpolatedPath* iInterpolatedPath
                                        , ::ULIS::FVec2D* iPointPositionBuffer
                                        , const BLMatrix2D& iWorldMatrix
                                        , BLContext* iBLContext )
{
    for( FInterpolatedSegmentCubic& interpolatedCubicSegment : iInterpolatedPath->mInterpolatedSegmentCubicBuffer )
    {
        FInterpolatedPoint* interpolatedPoint[4] = { interpolatedCubicSegment.mInterpolatedVertex[0]
                                                   , interpolatedCubicSegment.mInterpolatedHandle[0]
                                                   , interpolatedCubicSegment.mInterpolatedHandle[1]
                                                   , interpolatedCubicSegment.mInterpolatedVertex[1] };
        BLPoint pt[4] = { iWorldMatrix.mapPoint( iPointPositionBuffer[interpolatedPoint[0]->mIndex].x
                                               , iPointPositionBuffer[interpolatedPoint[0]->mIndex].y )
                        , iWorldMatrix.mapPoint( iPointPositionBuffer[interpolatedPoint[1]->mIndex].x
                                               , iPointPositionBuffer[interpolatedPoint[1]->mIndex].y )
                        , iWorldMatrix.mapPoint( iPointPositionBuffer[interpolatedPoint[2]->mIndex].x
                                               , iPointPositionBuffer[interpolatedPoint[2]->mIndex].y )
                        , iWorldMatrix.mapPoint( iPointPositionBuffer[interpolatedPoint[3]->mIndex].x
                                               , iPointPositionBuffer[interpolatedPoint[3]->mIndex].y ) };
        BLPath path;

        path.moveTo ( pt[0].x, pt[0].y );
        path.cubicTo( pt[1].x, pt[1].y
                    , pt[2].x, pt[2].y
                    , pt[3].x, pt[3].y );

        iBLContext->strokePath( path );
    }
}

void
FOdysseyVectorTagInbetweener::DrawPathsInbetween( uint32 iInbetweenIndex
                                                , BLContext* iBLContext )
{
    BLMatrix2D worldMatrix = mOwner->GetWorldMatrix();

    worldMatrix.transform( mChart.inbetweenBuffer[iInbetweenIndex].matrix );

    for( FInterpolatedPath& interpolatedPath : mInterpolatedPathBuffer )
    {
        uint32 pointCount = interpolatedPath.mInterpolatedPointBuffer.size();
        ::ULIS::FVec2D* pointPositionBuffer = &interpolatedPath.mInterpolatedPointPositionBuffer[pointCount * iInbetweenIndex];

        DrawPathAt( &interpolatedPath
                  , pointPositionBuffer
                  , worldMatrix
                  , iBLContext );
    }
}

void
FOdysseyVectorTagInbetweener::DrawPathsTarget( BLContext* iBLContext )
{
    BLMatrix2D worldMatrix = mOwner->GetWorldMatrix();

    worldMatrix.transform( mTargetLocalMatrix );

    for( FInterpolatedPath& interpolatedPath : mInterpolatedPathBuffer )
    {
        uint32 pointCount = interpolatedPath.mInterpolatedPointBuffer.size();
        ::ULIS::FVec2D* pointPositionBuffer = &interpolatedPath.mInterpolatedPointPositionBuffer[pointCount * mInbetweenCount];

        DrawPathAt( &interpolatedPath
                  , pointPositionBuffer
                  , worldMatrix
                  , iBLContext );
    }
}

FInbetweenerGrid*
FOdysseyVectorTagInbetweener::GetGrid()
{
    return mGrid;
}

void
FOdysseyVectorTagInbetweener::MoveInbetween( FInbetweenerInbetween* iInbetween
                                           , float iNewSpacing
                                           , bool iRelative )
{
    if( ( iNewSpacing > 0.0f ) && ( iNewSpacing < 1.0f ) )
    {
        IOdysseyVectorAnimationCell* animationCell = mOwner->GetScene()->GetEngine()->GetAnimationCell();
        int32 inbetweenIndex = iInbetween - &mChart.inbetweenBuffer[0];
        int32 prevIndex  = inbetweenIndex - 1;
        uint32 nextIndex = inbetweenIndex + 1;
        float prevSpacing = prevIndex > -1 ? mChart.inbetweenBuffer[prevIndex].spacing
                                           : 0.0f;
        float nextSpacing = nextIndex < mInbetweenCount ? mChart.inbetweenBuffer[nextIndex].spacing
                                                        : 1.0f;

        if( iRelative == false )
        {
            if( ( iNewSpacing > prevSpacing )
             && ( iNewSpacing < nextSpacing ) )
            {
                iInbetween->spacing = iNewSpacing;

                // recompute single inbetweens
                InterpolateGeometry( inbetweenIndex );
                InterpolateTransform( inbetweenIndex );
            }
        }
        else
        {
            for( FInbetweenerInbetween& otherInbetween : mChart.inbetweenBuffer )
            {
                if( &otherInbetween != iInbetween )
                {
                    if( otherInbetween.spacing < iInbetween->spacing )
                    {
                        float length = iInbetween->spacing;
                        float ratio = iInbetween->spacing ? ( otherInbetween.spacing / length ) : 0.0f;
                        float newLength = iNewSpacing;

                        otherInbetween.spacing = newLength * ratio;
                    }
                    else
                    {
                        float length = 1.0f - iInbetween->spacing;
                        float ratio = iInbetween->spacing ? ( ( otherInbetween.spacing - iInbetween->spacing ) / length ) : 0.0f;
                        float newLength = 1.0f - iNewSpacing;

                        otherInbetween.spacing = iNewSpacing + ( newLength * ratio );
                    }
                }
            }

            iInbetween->spacing = iNewSpacing;

            // recompute all inbetweens
            Interpolate();
        }
    }
}

double
FOdysseyVectorTagInbetweener::GetTargetTranslationX()
{
    return mTargetTranslationX;
}

double
FOdysseyVectorTagInbetweener::GetTargetTranslationY()
{
    return mTargetTranslationY;
}

double
FOdysseyVectorTagInbetweener::GetTargetRotation()
{
    return mTargetRotation;
}

double
FOdysseyVectorTagInbetweener::GetTargetScalingX()
{
    return mTargetScalingX;
}

double
FOdysseyVectorTagInbetweener::GetTargetScalingY()
{
    return mTargetScalingY;
}

void
FOdysseyVectorTagInbetweener::AllocBuffers()
{
    for( FInterpolatedPath& interpolatedPath : mInterpolatedPathBuffer )
    {
        uint32 pointCount = interpolatedPath.mInterpolatedPointBuffer.size();

        // Note: we add +1 to store the target too
        interpolatedPath.mInterpolatedPointPositionBuffer.resize( ( mInbetweenCount + 1 ) * pointCount );
    }
}

void
FOdysseyVectorTagInbetweener::SetInbetweenCount( uint32 iInbetweenCount )
{
    uint32 maxInbetweenCount = ::ULIS::FMath::Max( iInbetweenCount, mInbetweenCount );

    mInbetweenCount = iInbetweenCount;

    Invalidate( INVALIDATE_SPACING
              | INVALIDATE_BUFFERS
              | INVALIDATE_CELLS );

    ResetChart();

    UpdateAnimationCells( maxInbetweenCount );
}

eInbetweenerGridType
FOdysseyVectorTagInbetweener::GetGridType()
{
    return mGridType;
}

std::vector<FInbetweenerGridQuad>&
FOdysseyVectorTagInbetweener::GetGridQuadBuffer()
{
    return mGrid->GetQuadBuffer();
}

std::vector<FInbetweenerGridPoint>&
FOdysseyVectorTagInbetweener::GetGridPointBuffer()
{
    return mGrid->GetPointBuffer();
}

uint32
FOdysseyVectorTagInbetweener::GetGridNumQuadX()
{
    return mGrid->GetNumQuadX();
}

uint32
FOdysseyVectorTagInbetweener::GetGridNumQuadY()
{
    return mGrid->GetNumQuadY();
}

void
FOdysseyVectorTagInbetweener::SetGridType( eInbetweenerGridType iGridType )
{
    uint32 numQuadX = ( mGrid ) ? mGrid->GetNumQuadX() : 4;
    uint32 numQuadY = ( mGrid ) ? mGrid->GetNumQuadY() : 4;

    if( mGrid )
    {
        delete mGrid;

        mGrid = nullptr;
    }

    mGridType = iGridType;

    switch( iGridType )
    {
        case eInbetweenerGridType::ARAP :
            mGrid = new FInbetweenerGridARAP( this, numQuadX, numQuadY );
        break;

        default:
            mGrid = new FInbetweenerGridFFD( this, numQuadX, numQuadY );
        break;
    }

    Invalidate( INVALIDATE_MAP
              | INVALIDATE_SPACING
              | INVALIDATE_BUFFERS
              | INVALIDATE_CELLS );

    mGrid->Make( mGrid->GetNumQuadX(), mGrid->GetNumQuadY() );
}

void
FOdysseyVectorTagInbetweener::SetGridNumQuad( uint32 iNumQuadX, uint32 iNumQuadY )
{
    Invalidate( INVALIDATE_SPACING
              | INVALIDATE_CELLS );

    mGrid->Make( iNumQuadX, iNumQuadY );
}

void
FOdysseyVectorTagInbetweener::SetGridNumQuadX( uint32 iNumQuadX )
{
    Invalidate( INVALIDATE_SPACING
              | INVALIDATE_CELLS );

    mGrid->Make( iNumQuadX, mGrid->GetNumQuadY() );
}

void
FOdysseyVectorTagInbetweener::SetGridNumQuadY( uint32 iNumQuadY )
{
    Invalidate( INVALIDATE_SPACING
              | INVALIDATE_CELLS );

    mGrid->Make( mGrid->GetNumQuadX(), iNumQuadY );
}

void
FOdysseyVectorTagInbetweener::Commit()
{
    IOdysseyVectorAnimationCell* animationCell = mOwner->GetScene()->GetEngine()->GetAnimationCell();
    int32 animationCellIndex = animationCell->GetIndex();

    for( uint32 inbetweenIndex = 0; inbetweenIndex < mInbetweenCount; inbetweenIndex++ )
    {
        IOdysseyVectorAnimationCell* inbetweenAnimationCell = animationCell->GetCellByIndex( animationCellIndex + inbetweenIndex + 1 );

        if( inbetweenAnimationCell )
        {
            // change vertices coords before copying the object
            std::function<void(FOdysseyVectorObject*)> preProcess = [ inbetweenIndex ]( FOdysseyVectorObject* vectorObject )
            {
                FOdysseyVectorTag* tag = vectorObject->GetTagByType( FOdysseyVectorTagInbetweener::StaticClass() );

                if( tag )
                {
                    FOdysseyVectorTagInbetweener* inbetweenerTag = static_cast<FOdysseyVectorTagInbetweener*>(tag);
                    FInbetweenerInbetween* inbetween = &inbetweenerTag->mChart.inbetweenBuffer[inbetweenIndex];

                    for( FInterpolatedPath& interpolatedPath : inbetweenerTag->mInterpolatedPathBuffer )
                    {
                        uint32 pointCount = interpolatedPath.mInterpolatedPointBuffer.size();
                        uint32 skippedOffset = ( inbetweenIndex * pointCount );

                        for( uint32 i = 0; i < interpolatedPath.mInterpolatedPointBuffer.size(); i++ )
                        {
                            FInterpolatedPoint* interpolatedPoint = &interpolatedPath.mInterpolatedPointBuffer[i];
                            ::ULIS::FVec2D* commitPosition = &interpolatedPath.mInterpolatedPointPositionBuffer[skippedOffset + i];
                            ::ULIS::FVec2D swapPosition = interpolatedPoint->mOriginalPoint->GetCoords();
                            BLPoint transformedPosition;

                            transformedPosition = inbetween->matrix.mapPoint( commitPosition->x
                                                                            , commitPosition->y );

                            interpolatedPoint->mOriginalPoint->Set( transformedPosition.x
                                                                  , transformedPosition.y );

                            *commitPosition = swapPosition;
                        }
                    }

                    //tag->GetOwner()->RemoveTag( tag );
                }
            };

            std::function<void(FOdysseyVectorObject*,FOdysseyVectorObject*)> postProcess = [ inbetweenIndex
                                                                                           , preProcess ]( FOdysseyVectorObject* sourceObject
                                                                                                         , FOdysseyVectorObject* objectCopy )
            {
                FOdysseyVectorTag* tag = sourceObject->GetTagByType( FOdysseyVectorTagInbetweener::StaticClass() );

                if( tag )
                {
                    FOdysseyVectorTagInbetweener* inbetweenerTag = static_cast<FOdysseyVectorTagInbetweener*>(tag);
                    FInbetweenerInbetween* inbetween = &inbetweenerTag->mChart.inbetweenBuffer[inbetweenIndex];

                    inbetween->matrix.reset();
                }

                // revert vertices coords after having copied the object. It's actually the same thing.
                preProcess( sourceObject );
            };

            FOdysseyVectorGroupPaint* inbetweenScene = inbetweenAnimationCell->GetEngine()->GetScene();

            FOdysseyVectorObject* copiedObject = mOwner->Copy( preProcess, postProcess );

            inbetweenScene->AppendChild( copiedObject );

            inbetweenScene->UpdateMatrix();
            inbetweenScene->Update( 0 );
        }
    }

    mOwner->RecursiveRemoveTagByType( FOdysseyVectorTagInbetweener::StaticClass() );

    UpdateAnimationCells();
}
