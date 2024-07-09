#include "InbetweenerTag/InbetweenerHandleTrajectory.h"
#include "InbetweenerTag/InbetweenerTrajectory.h"
#include "OdysseyVectorTagInbetweener.h"

FInbetweenerHandleTrajectory::FInbetweenerHandleTrajectory( FInbetweenerTrajectory* iTrajectory )
    : mTrajectory ( iTrajectory )
    , mDirection( 0.0f, 0.0f )
    , mLengthRatio( 0.0f )
{

}

void
FInbetweenerHandleTrajectory::Set( const ::ULIS::FVec2D& iDirection
                                 , double iLengthRatio )
{
    mDirection = iDirection;
    mLengthRatio = iLengthRatio;

    mTrajectory->GetInbetweenerTag()->Invalidate( FOdysseyVectorTagInbetweener::INVALIDATE_TRAJECTORIES
                                                | FOdysseyVectorTagInbetweener::INVALIDATE_SPACING
                                                | FOdysseyVectorTagInbetweener::INVALIDATE_CELLS );
}

const ::ULIS::FVec2D&
FInbetweenerHandleTrajectory::GetDirection()
{
    return mDirection;
}

double
FInbetweenerHandleTrajectory::GetLengthRatio()
{
    return mLengthRatio;
}

FInbetweenerTrajectory*
FInbetweenerHandleTrajectory::GetTrajectory()
{
    return mTrajectory;
}
