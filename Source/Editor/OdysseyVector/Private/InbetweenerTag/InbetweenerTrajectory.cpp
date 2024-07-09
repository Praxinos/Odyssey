#include "InbetweenerTag/InbetweenerTrajectory.h"
#include "InbetweenerTag/InbetweenerGrid.h"
#include "OdysseyVectorTagInbetweener.h"
#include "OdysseyVector.h"

FInbetweenerTrajectory::~FInbetweenerTrajectory()
{
}

FInbetweenerTrajectory::FInbetweenerTrajectory( FOdysseyVectorTagInbetweener* iInbetweenerTag
                                              , uint32 iQuadIndex
                                              , double iQuadU
                                              , double iQuadV )
    : mInbetweenerTag( iInbetweenerTag )
    , mHandle{ (this), (this) }
{
    Init( iQuadIndex, iQuadU, iQuadV );
}

void
FInbetweenerTrajectory::Init( uint32 iQuadIndex
                            , double iQuadU
                            , double iQuadV )
{
    mQuadIndex = iQuadIndex;
    mQuadU = iQuadU;
    mQuadV = iQuadV;
}

void
FInbetweenerTrajectory::Update()
{
    BLMatrix2D targetLocalMatrix = mInbetweenerTag->GetTargetLocalMatrix();
    double bezierLength;
    FInbetweenerQuad* quad = GetQuad();

    mCubicBezier[0] = quad->GetPoint( eInbetweenerPointPositionType::SourcePosition
                                    , mQuadU
                                    , mQuadV );

    mCubicBezier[3] = FOdysseyVector::MapPoint( targetLocalMatrix
                                              , quad->GetPoint( eInbetweenerPointPositionType::TargetPosition
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

::ULIS::FVec2D*
FInbetweenerTrajectory::GetCubicBezier()
{
    return mCubicBezier;
}

FInbetweenerQuad* 
FInbetweenerTrajectory::GetQuad()
{
    return &mInbetweenerTag->GetGrid()->GetQuadBuffer()[mQuadIndex];
}

uint32
FInbetweenerTrajectory::GetQuadIndex()
{
    return mQuadIndex;
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

FOdysseyVectorTagInbetweener*
FInbetweenerTrajectory::GetInbetweenerTag()
{
    return mInbetweenerTag;
}
