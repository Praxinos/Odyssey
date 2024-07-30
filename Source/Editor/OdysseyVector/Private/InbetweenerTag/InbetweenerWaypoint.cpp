#include "InbetweenerTag/InbetweenerWaypoint.h"
#include "InbetweenerTag/InbetweenerTrajectory.h"
#include "InbetweenerTag/InbetweenerRoute.h"
#include "OdysseyVectorTagInbetweener.h"

FInbetweenerWaypoint::FInbetweenerWaypoint( FInbetweenerTrajectory* iTrajectory )
    : mTrajectory( iTrajectory )
    , mRatio( 0.0f )
{
}

FInbetweenerTrajectory*
FInbetweenerWaypoint::GetTrajectory()
{
    return mTrajectory;
}

void
FInbetweenerWaypoint::SetRatio( float iRatio )
{
    mRatio = iRatio;
}

float
FInbetweenerWaypoint::GetRatio()
{
    return mRatio;
}

void
FInbetweenerWaypoint::SetT( float iT )
{
    uint32 waypointIndex = this - &mTrajectory->GetWaypointBuffer()[0];
    double drawingT = mTrajectory->GetRoute()->GetInbetweenerTag()->GetChart().drawingBuffer[waypointIndex].spacing;

    // waypoint is precisely on inbetween
    mRatio = 0.0f;

    // positive waypoint is after on inbetween
    if( iT > drawingT )
    {
        mRatio =  ( iT - drawingT ) / ( 1.0f - drawingT );
    }

    // negative waypoint is before on inbetween
    if ( iT < drawingT )
    {
        mRatio = -( drawingT - iT ) / (        drawingT );
    }

    mTrajectory->GetRoute()->GetInbetweenerTag()->Invalidate( FOdysseyVectorTagInbetweener::INVALIDATE_SPACING );
}

float
FInbetweenerWaypoint::GetT()
{
    uint32 waypointIndex = this - &mTrajectory->GetWaypointBuffer()[0];
    double drawingT = mTrajectory->GetRoute()->GetInbetweenerTag()->GetChart().drawingBuffer[waypointIndex].spacing;

    if( mRatio > 0.0f )
    {
        return drawingT + ( mRatio * ( 1.0f - drawingT ) );
    }

    if( mRatio < 0.0f )
    {
        return drawingT + ( mRatio * (        drawingT ) );
    }

    return drawingT;
}
