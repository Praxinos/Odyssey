#include "InbetweenerTag/InbetweenerTrajectory.h"
#include "OdysseyVectorTagInbetweener.h"

FInbetweenerTrajectory::~FInbetweenerTrajectory()
{
}

FInbetweenerTrajectory::FInbetweenerTrajectory( FInbetweenerQuad* iQuad
                                              , double iQuadU
                                              , double iQuadV
                                                     /*, const ::ULIS::FVec2D& iP0
                                                      , const ::ULIS::FVec2D& iP1 
                                                      , const ::ULIS::FVec2D& iP2 
                                                      , const ::ULIS::FVec2D& iP3*/ )
    : mQuad( iQuad )
    , mQuadU( iQuadU )
    , mQuadV( iQuadV )
{
/*
    FInbetweenerGridPoint** quadPoint = mQuad->GetPoints();
    ::ULIS::FVec2D p0Coords = quadPoint[0]->GetSourcePosition();
    ::ULIS::FVec2D p1Coords = quadPoint[1]->GetSourcePosition();
    ::ULIS::FVec2D p2Coords = quadPoint[2]->GetSourcePosition();
    ::ULIS::FVec2D p3Coords = quadPoint[3]->GetSourcePosition();
    double difX = p1Coords.x - p0Coords.x;
    double difY = p2Coords.y - p1Coords.y;

    mQuadU = difX ? ( iP0.x - p0Coords.x ) / difX : 0.0f;
    mQuadV = difY ? ( iP0.y - p0Coords.y ) / difY : 0.0f;
*/
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
