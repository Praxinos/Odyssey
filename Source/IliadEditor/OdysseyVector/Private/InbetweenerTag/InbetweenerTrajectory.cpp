// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "InbetweenerTag/InbetweenerTrajectory.h"
#include "InbetweenerTag/InbetweenerGrid.h"
#include "InbetweenerTag/InbetweenerRoute.h"
#include "InbetweenerTag/InbetweenerBreakdown.h"
#include "OdysseyVectorTagInbetweener.h"
#include "OdysseyVector.h"

#define FRACTIONCOUNT 24

FInbetweenerTrajectory::~FInbetweenerTrajectory()
{
}

FInbetweenerTrajectory::FInbetweenerTrajectory( FInbetweenerRoute* iRoute
                                              , FInbetweenerStep* iStep0
                                              , FInbetweenerStep* iStep1
                                              , FInbetweenerBreakdown* iBreakdown )
    : mRoute( iRoute )
    , mBreakdown ( iBreakdown )
    , mStep { iStep0, iStep1 }
    , mHandle { ( this ), ( this ) }
{
    uint32 drawingCount = iBreakdown->GetTargetDrawingIndex() - iBreakdown->GetSourceDrawingIndex() + 1;

    Init( drawingCount );
}

void
FInbetweenerTrajectory::Init( uint32 iDrawingCount )
{
    mStep[0]->AddTrajectory( this );
    mStep[1]->AddTrajectory( this );

    mFractionBuffer.resize( FRACTIONCOUNT );

    Resize( iDrawingCount );
    // we need the cubic bezier to be ready so that we can mooth the trajectories
    Update();
}

FInbetweenerTrajectory*
FInbetweenerTrajectory::GetNext()
{
    int32 index = this - &mRoute->GetTrajectoryBuffer()[0];
    int32 nextIndex = index + 1;

    if ( ( nextIndex >= 0  ) && ( nextIndex < mRoute->GetTrajectoryBuffer().size() ) )
    {
        return &mRoute->GetTrajectoryBuffer()[nextIndex];
    }

    return nullptr;
}

FInbetweenerTrajectory*
FInbetweenerTrajectory::GetPrev()
{
    int32 index = this - &mRoute->GetTrajectoryBuffer()[0];
    int32 prevIndex = index - 1;

    if ( ( prevIndex >= 0  ) && ( prevIndex < mRoute->GetTrajectoryBuffer().size() ) )
    {
        return &mRoute->GetTrajectoryBuffer()[prevIndex];
    }

    return nullptr;
}

void
FInbetweenerTrajectory::Import( FInbetweenerTrajectory& iImportFrom )
{
    mHandle[0].Set( iImportFrom.GetHandle(0)->GetDirection(), iImportFrom.GetHandle(0)->GetLengthRatio() );
    mHandle[1].Set( iImportFrom.GetHandle(1)->GetDirection(), iImportFrom.GetHandle(1)->GetLengthRatio() );
}

/*
* Updates the bezier between the source grid and the target grid
*/
void
FInbetweenerTrajectory::Update()
{
    const BLMatrix2D& sourceLocalMatrix = mBreakdown->GetSourceLocalMatrix();
    const BLMatrix2D& targetLocalMatrix = mBreakdown->GetTargetLocalMatrix();
    double bezierLength;
    FInbetweenerQuad* quad = GetQuad();
    double quadU = mRoute->GetQuadU();
    double quadV = mRoute->GetQuadV();
    double cubicT0 = 0.0f;
    double linearT0 = 0.0f;
    double stepT = 1.0f / FRACTIONCOUNT;
    ::ULIS::FVec2D p0;
    double cubicBezierLength;
    double totalFractionLength = 0.0f;
    std::vector<double> fractionLengthBuffer;

    mCubicBezier[0] = FOdysseyVector::MapPoint( sourceLocalMatrix
                                              , quad->GetPoint( eInbetweenerPointPositionType::SourcePosition
                                                              , quadU
                                                              , quadV ) );

    mCubicBezier[3] = FOdysseyVector::MapPoint( targetLocalMatrix
                                              , quad->GetPoint( eInbetweenerPointPositionType::TargetPosition
                                                              , quadU
                                                              , quadV ) );

    bezierLength = ( mCubicBezier[3] - mCubicBezier[0] ).Distance();

    mCubicBezier[1] = mCubicBezier[0] + ( FOdysseyVector::MapVector( sourceLocalMatrix, mHandle[0].GetDirection() ) * mHandle[0].GetLengthRatio() * bezierLength );
    mCubicBezier[2] = mCubicBezier[3] + ( FOdysseyVector::MapVector( targetLocalMatrix, mHandle[1].GetDirection() ) * mHandle[1].GetLengthRatio() * bezierLength );

    cubicBezierLength = FOdysseyVector::GetCubicBezierApproximateLength( mCubicBezier
                                                                       , FRACTIONCOUNT
                                                                       , &fractionLengthBuffer );

    // build a lookup table for getting linear values for t
    for( uint32 i = 0; i < FRACTIONCOUNT; i++ )
    {
        double cubicT1 = cubicT0 + stepT;
        double linearT1 = linearT0 + ( fractionLengthBuffer[i] / cubicBezierLength );

        totalFractionLength += fractionLengthBuffer[i];

        mFractionBuffer[i].linearT0 = linearT0;
        mFractionBuffer[i].linearT1 = linearT1;
        mFractionBuffer[i].cubicT0  = cubicT0;
        mFractionBuffer[i].cubicT1  = cubicT1;

        cubicT0 = cubicT1;
        linearT0 = linearT1;
    }
    mFractionBuffer.back().linearT1 = 1.0f;
    mFractionBuffer.back().cubicT1 = 1.0f;
}

double
FInbetweenerTrajectory::GetLinearT( float iCubicT )
{
    float linearT = 0.0f;

    for( uint32 i = 0; i < FRACTIONCOUNT; i++ )
    {
        Fraction* fraction = &mFractionBuffer[i];

        if( ( iCubicT >= fraction->cubicT0 ) && ( iCubicT <= fraction->cubicT1 ) )
        {
            float diffCubic = fraction->cubicT1 - fraction->cubicT0;

            if( diffCubic )
            {
                float diffLinear = fraction->linearT1 - fraction->linearT0;
                float ratio = ( iCubicT - fraction->cubicT0 ) / diffCubic;

                linearT = fraction->linearT0 + ( diffLinear * ratio );

                break;
            }
        }
    }

    return linearT;
}

double
FInbetweenerTrajectory::GetCubicT( float iLinearT )
{
    float cubicT = 0.0f;

    for( uint32 i = 0; i < FRACTIONCOUNT; i++ )
    {
        Fraction* fraction = &mFractionBuffer[i];

        if( ( iLinearT >= fraction->linearT0 ) && ( iLinearT <= fraction->linearT1 ) )
        {
            float diffLinear = fraction->linearT1 - fraction->linearT0;

            if( diffLinear )
            {
                float diffCubic = fraction->cubicT1 - fraction->cubicT0;
                float ratio = ( iLinearT - fraction->linearT0 ) / diffLinear;

                cubicT = fraction->cubicT0 + ( diffCubic * ratio );

                break;
            }
        }
    }

    return cubicT;
}

::ULIS::FVec2D
FInbetweenerTrajectory::GetPointFromCubicT( float iCubicT )
{
    return ::ULIS::CubicBezierPointAtParameter<::ULIS::FVec2D>( mCubicBezier[0]
                                                              , mCubicBezier[1]
                                                              , mCubicBezier[2]
                                                              , mCubicBezier[3]
                                                              , iCubicT );
}

::ULIS::FVec2D
FInbetweenerTrajectory::GetPointFromLinearT( float iLinearT )
{
    float cubicT = GetCubicT( iLinearT );

    return ::ULIS::CubicBezierPointAtParameter<::ULIS::FVec2D>( mCubicBezier[0]
                                                              , mCubicBezier[1]
                                                              , mCubicBezier[2]
                                                              , mCubicBezier[3]
                                                              , cubicT );
}

FInbetweenerWaypoint*
FInbetweenerTrajectory::GetWaypoint( uint32 iIndex )
{
    return &mWaypointBuffer[iIndex];
}

FInbetweenerStep*
FInbetweenerTrajectory::GetStep( uint32 iIndex )
{
    return mStep[iIndex];
}

FInbetweenerBreakdown*
FInbetweenerTrajectory::GetBreakdown()
{
    return mBreakdown;
}

std::vector<FInbetweenerTrajectory::Fraction>&
FInbetweenerTrajectory::GetFractionBuffer()
{
    return mFractionBuffer;
}

void
FInbetweenerTrajectory::Resize( uint32 iDrawingCount )
{
    mWaypointBuffer.clear();
    mWaypointBuffer.resize( iDrawingCount, this );
}

FInbetweenerHandleTrajectory*
FInbetweenerTrajectory::GetHandle( uint32 index )
{
    return &mHandle[index];
}

::ULIS::FVec2D*
FInbetweenerTrajectory::GetCubicBezier()
{
    return mCubicBezier;
}

FInbetweenerQuad*
FInbetweenerTrajectory::GetQuad()
{
    return &mBreakdown->GetGrid()->GetQuadBuffer()[mRoute->GetQuadIndex()];
}

std::vector<FInbetweenerWaypoint>&
FInbetweenerTrajectory::GetWaypointBuffer()
{
    return mWaypointBuffer;
}

FInbetweenerRoute*
FInbetweenerTrajectory::GetRoute()
{
    return mRoute;
}

void
FInbetweenerTrajectory::Reset()
{
    mHandle[0].Set( ::ULIS::FVec2D( 0.0f, 0.0f ), 0.0f );
    mHandle[1].Set( ::ULIS::FVec2D( 0.0f, 0.0f ), 0.0f );

    for( FInbetweenerWaypoint& waypoint : mWaypointBuffer )
    {
        waypoint.SetRatio( 0.0f );
    }
}

void
FInbetweenerTrajectory::FitBezier( const std::vector<::ULIS::FVec2D> &iPointBuffer )
{
    const BLMatrix2D& sourceLocalMatrix = mBreakdown->GetSourceLocalMatrix();
    const BLMatrix2D& targetLocalMatrix = mBreakdown->GetTargetLocalMatrix();
    BLMatrix2D sourceLocalInverseMatrix;
    BLMatrix2D targetLocalInverseMatrix;
    double bezierLength;
    double handleRatio[2] = { 0.0f, 0.0f };
    ::ULIS::FVec2D cubicBezier[4];
    ::ULIS::FVec2D handleVector[2];
    const ::ULIS::FVec2D* firstRecord = &iPointBuffer.front();
    const ::ULIS::FVec2D* lastRecord  = &iPointBuffer.back();
    // Unit tangent vectors at endpoints
    ::ULIS::FVec2D leftTangent  = (*(firstRecord + 1 )) - (*firstRecord);
    ::ULIS::FVec2D rightTangent = (*(lastRecord  - 1 )) - (*lastRecord );

    BLMatrix2D::invert( sourceLocalInverseMatrix, sourceLocalMatrix );
    BLMatrix2D::invert( targetLocalInverseMatrix, targetLocalMatrix );

    if( leftTangent.DistanceSquared() && rightTangent.DistanceSquared() )
    {
        std::vector<double> uPrime;

        leftTangent.Normalize();
        rightTangent.Normalize();


        FOdysseyVector::ChordLengthParameterize( iPointBuffer
                                               , firstRecord
                                               , lastRecord
                                               , uPrime );

        if( iPointBuffer.size() > 1 )
        {
            FOdysseyVector::GenerateBezier( iPointBuffer
                                          , firstRecord
                                          , lastRecord
                                          , uPrime
                                          , leftTangent
                                          , rightTangent
                                          , cubicBezier );
        }

        handleVector[0] = cubicBezier[1] - cubicBezier[0];
        handleVector[1] = cubicBezier[2] - cubicBezier[3];

        double handleLength[2] = { handleVector[0].Distance()
                                 , handleVector[1].Distance() };

        bezierLength = FOdysseyVector::GetCubicBezierApproximateLength( cubicBezier, 16 );

        if( bezierLength )
        {
            handleRatio[0] = std::clamp<double>( handleLength[0] / bezierLength, 0.0f, 1.0f );
            handleRatio[1] = std::clamp<double>( handleLength[1] / bezierLength, 0.0f, 1.0f );
        }

        if( handleLength[0] ) handleVector[0].Normalize();
        if( handleLength[1] ) handleVector[1].Normalize();

        BLPoint vec0 = sourceLocalInverseMatrix.mapVector( handleVector[0].x, handleVector[0].y );
        BLPoint vec1 = targetLocalInverseMatrix.mapVector( handleVector[1].x, handleVector[1].y );

        mHandle[0].Set( ::ULIS::FVec2D( vec0.x, vec0.y ), handleRatio[0] );
        mHandle[1].Set( ::ULIS::FVec2D( vec1.x, vec1.y ), handleRatio[1] );
    }
}

// INRIA Style
void
FInbetweenerTrajectory::FitBezier( const std::vector<::ULIS::FVec2D> &data
                                 , const std::vector<float> &u )
{
    const BLMatrix2D& sourceLocalMatrix = mBreakdown->GetSourceLocalMatrix();
    const BLMatrix2D& targetLocalMatrix = mBreakdown->GetTargetLocalMatrix();
    BLMatrix2D sourceLocalInverseMatrix;
    BLMatrix2D targetLocalInverseMatrix;
    ::ULIS::FVec2D cubicBezier[4];
    ::ULIS::FVec2D handleVector[2];
    Eigen::Matrix4d M;  // bezier coeffs
    Eigen::MatrixXd T;  // param
    Eigen::MatrixXd D;  // data points
    Eigen::MatrixXd P;  // control points (unknowns)
    double bezierLength;
    double handleRatio[2] = { 0.0f, 0.0f };

    BLMatrix2D::invert( sourceLocalInverseMatrix, sourceLocalMatrix );
    BLMatrix2D::invert( targetLocalInverseMatrix, targetLocalMatrix );

    M << -1.0,  3.0, -3.0, 1.0,
          3.0, -6.0,  3.0, 0.0,
         -3.0,  3.0,  0.0, 0.0,
          1.0,  0.0,  0.0, 0.0;

    T = Eigen::MatrixXd(data.size(), 4);
    D = Eigen::MatrixXd(data.size(), 2);
    for (int i = 0; i < data.size(); ++i) {
        D.row(i) = FInbetweenerPoint::VectorType( data[i].x, data[i].y );
        T.coeffRef(i, 3) = 1.0;
        for (int j = 2; j >= 0; --j) {
            T.coeffRef(i, j) = u[i] * T.coeffRef(i, j + 1);
        }
    }

    P = Eigen::MatrixXd(4, 2);
    P = (T * M).colPivHouseholderQr().solve(D);

    cubicBezier[0].x = P.row(0).x();
    cubicBezier[0].y = P.row(0).y();

    cubicBezier[1].x = P.row(1).x();
    cubicBezier[1].y = P.row(1).y();

    cubicBezier[2].x = P.row(2).x();
    cubicBezier[2].y = P.row(2).y();

    cubicBezier[3].x = P.row(3).x();
    cubicBezier[3].y = P.row(3).y();

    handleVector[0] = cubicBezier[1] - cubicBezier[0];
    handleVector[1] = cubicBezier[2] - cubicBezier[3];

    double handleLength[2] = { handleVector[0].Distance()
                             , handleVector[1].Distance() };

    bezierLength = FOdysseyVector::GetCubicBezierApproximateLength( cubicBezier, 16 );

    if( bezierLength )
    {
        handleRatio[0] = std::clamp<double>( handleLength[0] / bezierLength, 0.0f, 1.0f );
        handleRatio[1] = std::clamp<double>( handleLength[1] / bezierLength, 0.0f, 1.0f );
    }

    if( handleLength[0] ) handleVector[0].Normalize();
    if( handleLength[1] ) handleVector[1].Normalize();

    BLPoint vec0 = sourceLocalInverseMatrix.mapVector( handleVector[0].x, handleVector[0].y );
    BLPoint vec1 = targetLocalInverseMatrix.mapVector( handleVector[1].x, handleVector[1].y );

    mHandle[0].Set( ::ULIS::FVec2D( vec0.x, vec0.y ), handleRatio[0] );
    mHandle[1].Set( ::ULIS::FVec2D( vec1.x, vec1.y ), handleRatio[1] );
}
