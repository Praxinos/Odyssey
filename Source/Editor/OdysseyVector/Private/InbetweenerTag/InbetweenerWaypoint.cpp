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
    FOdysseyVectorTagInbetweener* inbetweenerTag = mTrajectory->GetRoute()->GetInbetweenerTag();
    uint32 sourceDrawingIndex = mTrajectory->GetBreakdown()->GetSourceDrawingIndex();
    uint32 targetDrawingIndex = mTrajectory->GetBreakdown()->GetTargetDrawingIndex();
    uint32 waypointIndex = ( this - &mTrajectory->GetWaypointBuffer()[0] );
    uint32 waypointDrawingIndex = waypointIndex + sourceDrawingIndex + 1;
    double sourceDrawingT   = inbetweenerTag->GetChart().drawingBuffer[sourceDrawingIndex  ].spacing;
    double targetDrawingT   = inbetweenerTag->GetChart().drawingBuffer[targetDrawingIndex  ].spacing;
    double waypointDrawingT = inbetweenerTag->GetChart().drawingBuffer[waypointDrawingIndex].spacing;
    double relativeT = ( waypointDrawingT - sourceDrawingT ) / ( targetDrawingT - sourceDrawingT );

    // waypoint is precisely on inbetween
    mRatio = 0.0f;

    // positive waypoint is after on inbetween
    if( iT > relativeT )
    {
        mRatio =  ( iT - relativeT ) / ( 1.0f - relativeT );
    }

    // negative waypoint is before on inbetween
    if ( iT < relativeT )
    {
        mRatio = -( relativeT - iT ) / (        relativeT );
    }

    mTrajectory->GetRoute()->GetInbetweenerTag()->Invalidate( FOdysseyVectorTagInbetweener::INVALIDATE_SPACING );
}

float
FInbetweenerWaypoint::GetT()
{
    FOdysseyVectorTagInbetweener* inbetweenerTag = mTrajectory->GetRoute()->GetInbetweenerTag();
    uint32 sourceDrawingIndex = mTrajectory->GetBreakdown()->GetSourceDrawingIndex();
    uint32 targetDrawingIndex = mTrajectory->GetBreakdown()->GetTargetDrawingIndex();
    uint32 waypointIndex = ( this - &mTrajectory->GetWaypointBuffer()[0] );
    uint32 waypointDrawingIndex = waypointIndex + sourceDrawingIndex + 1;
    double sourceDrawingT   = inbetweenerTag->GetChart().drawingBuffer[sourceDrawingIndex  ].spacing;
    double targetDrawingT   = inbetweenerTag->GetChart().drawingBuffer[targetDrawingIndex  ].spacing;
    double waypointDrawingT = inbetweenerTag->GetChart().drawingBuffer[waypointDrawingIndex].spacing;
    double relativeT = ( waypointDrawingT - sourceDrawingT ) / ( targetDrawingT - sourceDrawingT );

    if( mRatio > 0.0f )
    {
        return relativeT + ( mRatio * ( 1.0f - relativeT ) );
    }

    if( mRatio < 0.0f )
    {
        return relativeT + ( mRatio * (        relativeT ) );
    }

    return relativeT;
}
