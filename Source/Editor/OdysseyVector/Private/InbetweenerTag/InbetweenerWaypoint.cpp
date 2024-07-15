#include "InbetweenerTag/InbetweenerWaypoint.h"
#include "InbetweenerTag/InbetweenerTrajectory.h"
#include "OdysseyVectorTagInbetweener.h"

FInbetweenerWaypoint::FInbetweenerWaypoint( FInbetweenerTrajectory* iTrajectory )
    : mTrajectory( iTrajectory )
    , mT( 0.0f )
{
}

FInbetweenerTrajectory*
FInbetweenerWaypoint::GetTrajectory()
{
    return mTrajectory;
}

void
FInbetweenerWaypoint::SetT( float iT )
{
    mT = iT;

    mTrajectory->GetInbetweenerTag()->Invalidate( FOdysseyVectorTagInbetweener::INVALIDATE_SPACING );
}

float
FInbetweenerWaypoint::GetT()
{
    return mT;
}
