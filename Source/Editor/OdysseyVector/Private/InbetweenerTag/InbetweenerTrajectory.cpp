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

    Resize( iDrawingCount );
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

void
FInbetweenerTrajectory::Resize( uint32 iDrawingCount )
{
    uint32 inbetweenCount = ( iDrawingCount - 2 );

    mWaypointBuffer.clear();
    mWaypointBuffer.reserve( inbetweenCount );

    for( uint32 i = 0; i < inbetweenCount; i++ )
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
