#include "InbetweenerTag/InbetweenerTrajectory.h"
#include "InbetweenerTag/InbetweenerGrid.h"
#include "OdysseyVectorTagInbetweener.h"
#include "OdysseyVector.h"

FInbetweenerTrajectory::~FInbetweenerTrajectory()
{
}

FInbetweenerTrajectory::FInbetweenerTrajectory( FInbetweenerGrid* iGrid
                                              , FInbetweenerQuad* iQuad
                                              , double iQuadU
                                              , double iQuadV )
    : mGrid( iGrid )
    , mHandle{ (this), (this) }
{
    Init( iQuad, iQuadU, iQuadV );
}

void
FInbetweenerTrajectory::Init( FInbetweenerQuad* iQuad
                            , double iQuadU
                            , double iQuadV )
{
    mQuad = iQuad;
    mQuadU = iQuadU;
    mQuadV = iQuadV;
}

void
FInbetweenerTrajectory::Update()
{
    BLMatrix2D targetLocalMatrix = mGrid->GetInbetweenerTag()->GetTargetLocalMatrix();
    double bezierLength;

    mCubicBezier[0] = mQuad->GetPoint( eInbetweenerPointPositionType::SourcePosition
                                     , mQuadU
                                     , mQuadV );

    mCubicBezier[3] = FOdysseyVector::MapPoint( targetLocalMatrix
                                              , mQuad->GetPoint( eInbetweenerPointPositionType::TargetPosition
                                                               , mQuadU
                                                               , mQuadV ) );

    bezierLength = ( mCubicBezier[3] - mCubicBezier[0] ).Distance();

    mCubicBezier[1] = mCubicBezier[0] + ( mHandle[0].GetDirection() * mHandle[0].GetLengthRatio() * bezierLength );
    mCubicBezier[2] = mCubicBezier[3] + ( mHandle[1].GetDirection() * mHandle[1].GetLengthRatio() * bezierLength );
}

FInbetweenerHandleTrajectory*
FInbetweenerTrajectory::GetHandle( uint32 index )
{
    return &mHandle[index];
}

FInbetweenerGrid*
FInbetweenerTrajectory::GetGrid()
{
    return mGrid;
}

::ULIS::FVec2D*
FInbetweenerTrajectory::GetCubicBezier()
{
    return mCubicBezier;
}

FInbetweenerQuad* 
FInbetweenerTrajectory::GetQuad()
{
    return mQuad;
}

double
FInbetweenerTrajectory::GetQuadU()
{
    return mQuadU;
}

double
FInbetweenerTrajectory::GetQuadV()
{
    return mQuadV;
}
