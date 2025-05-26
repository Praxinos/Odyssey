// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

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

    //mTrajectory->GetRoute()->GetInbetweenerTag()->Invalidate( FOdysseyVectorTagInbetweener::INVALIDATE_SPACING );
}

float
FInbetweenerWaypoint::GetRatio()
{
    return mRatio;
}

void
FInbetweenerWaypoint::SetT( float iT )
{
    FOdysseyVectorTagInbetweener* inbetweenerTag = mTrajectory->GetRoute()->GetInbetweenerTag();
    FInbetweenerBreakdown* breakdown = mTrajectory->GetBreakdown();
    uint32 inbetweenIndex = ( this - &mTrajectory->GetWaypointBuffer()[0] );
    double inbetweenT = breakdown->GetChart()->GetInbetweenBuffer()[inbetweenIndex].GetSpacing();

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

    mTrajectory->GetRoute()->GetInbetweenerTag()->Invalidate( FOdysseyVectorTagInbetweener::INVALIDATE_SPACING );
}

float
FInbetweenerWaypoint::GetT()
{
    FOdysseyVectorTagInbetweener* inbetweenerTag = mTrajectory->GetRoute()->GetInbetweenerTag();
    FInbetweenerBreakdown* breakdown = mTrajectory->GetBreakdown();
    uint32 inbetweenIndex = ( this - &mTrajectory->GetWaypointBuffer()[0] );
    double inbetweenT = breakdown->GetChart()->GetInbetweenBuffer()[inbetweenIndex].GetSpacing();

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
