#include "InbetweenerTag/InbetweenerTrajectory.h"
#include "InbetweenerTag/InbetweenerGrid.h"
#include "InbetweenerTag/InbetweenerRoute.h"
#include "InbetweenerTag/InbetweenerBreakdown.h"
#include "OdysseyVectorTagInbetweener.h"
#include "OdysseyVector.h"

FInbetweenerTrajectory::~FInbetweenerTrajectory()
{
}

FInbetweenerTrajectory::FInbetweenerTrajectory( FInbetweenerRoute* iRoute
                                              , FInbetweenerBreakdown* iBreakdown )
    : mRoute( iRoute )
    , mBreakdown ( iBreakdown )
    , mHandle { ( this ), ( this ) }
{
    Init( iBreakdown->GetTargetDrawingIndex() - iBreakdown->GetSourceDrawingIndex() );
}

void
FInbetweenerTrajectory::Init( uint32 iDrawingCount )
{
    ResetSpacing( iDrawingCount );
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

    mCubicBezier[0] = FOdysseyVector::MapPoint( sourceLocalMatrix
                                              , quad->GetPoint( eInbetweenerPointPositionType::SourcePosition
                                                              , quadU
                                                              , quadV ) );

    mCubicBezier[3] = FOdysseyVector::MapPoint( targetLocalMatrix
                                              , quad->GetPoint( eInbetweenerPointPositionType::TargetPosition
                                                              , quadU
                                                              , quadV ) );

    bezierLength = ( mCubicBezier[3] - mCubicBezier[0] ).Distance();

    mCubicBezier[1] = mCubicBezier[0] + ( mHandle[0].GetDirection() * mHandle[0].GetLengthRatio() * bezierLength );
    mCubicBezier[2] = mCubicBezier[3] + ( mHandle[1].GetDirection() * mHandle[1].GetLengthRatio() * bezierLength );
}

FInbetweenerWaypoint*
FInbetweenerTrajectory::GetWaypoint( uint32 iIndex )
{
    return &mWaypointBuffer[iIndex];
}

void
FInbetweenerTrajectory::ResetSpacing( uint32 iDrawingCount )
{
    mWaypointBuffer.clear();
    mWaypointBuffer.reserve( iDrawingCount );

    for( uint32 i = 0; i < iDrawingCount; i++ )
    {
        FInbetweenerWaypoint& waypoint = mWaypointBuffer.emplace_back( this );
    }
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
