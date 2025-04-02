// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "InbetweenerTag/InbetweenerHandleTrajectory.h"
#include "InbetweenerTag/InbetweenerTrajectory.h"
#include "InbetweenerTag/InbetweenerRoute.h"
#include "InbetweenerTag/InbetweenerChart.h"
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

    if( this == mTrajectory->GetHandle(0) )
    {
        if( mTrajectory->GetStep(0)->IsAligned() )
        {
            FInbetweenerTrajectory* prevTrajectory = mTrajectory->GetPrev();

            if( prevTrajectory )
            {
                prevTrajectory->GetHandle(1)->mDirection = - mDirection;
            }
        }
    }

    if( this == mTrajectory->GetHandle(1) )
    {
        if( mTrajectory->GetStep(1)->IsAligned() )
        {
            FInbetweenerTrajectory* nextTrajectory = mTrajectory->GetNext();

            if( nextTrajectory )
            {
                nextTrajectory->GetHandle(0)->mDirection = - mDirection;
            }
        }
    }

    mTrajectory->GetRoute()->GetInbetweenerTag()->Invalidate( FOdysseyVectorTagInbetweener::INVALIDATE_ROUTES
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
