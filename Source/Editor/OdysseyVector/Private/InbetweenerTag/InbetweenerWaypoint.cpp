#include "InbetweenerTag/InbetweenerWaypoint.h"
#include "InbetweenerTag/InbetweenerTrajectory.h"
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
    double inbetweenT = mTrajectory->GetInbetweenerTag()->GetChart().inbetweenBuffer[waypointIndex].spacing;

    // waypoint is precisely on inbetween
    mRatio = 0.0f;

    // positive waypoint is after on inbetween
    if( iT > inbetweenT )
    {
        mRatio =  ( iT - inbetweenT ) / ( 1.0f - inbetweenT );
    }

    // negative waypoint is before on inbetween
    if ( iT < inbetweenT )
    {
        mRatio = -( inbetweenT - iT ) / (        inbetweenT );
    }

    mTrajectory->GetInbetweenerTag()->Invalidate( FOdysseyVectorTagInbetweener::INVALIDATE_SPACING );
}

float
FInbetweenerWaypoint::GetT()
{
    uint32 waypointIndex = this - &mTrajectory->GetWaypointBuffer()[0];
    double inbetweenT = mTrajectory->GetInbetweenerTag()->GetChart().inbetweenBuffer[waypointIndex].spacing;

    if( mRatio > 0.0f )
    {
        return inbetweenT + ( mRatio * ( 1.0f - inbetweenT ) );
    }

    if( mRatio < 0.0f )
    {
        return inbetweenT + ( mRatio * (        inbetweenT ) );
    }

    return inbetweenT;
}
