// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "InbetweenerTag/InbetweenerStep.h"
#include "InbetweenerTag/InbetweenerTrajectory.h"

FInbetweenerStep::FInbetweenerStep( FInbetweenerRoute* iRoute )
    : mFlags ( ALIGNED )
    , mRoute( iRoute )
{
    mTrajectoryArray.reserve( 2 );
}

#pragma warning(push)
#pragma warning(disable: 4702)
void
FInbetweenerStep::SetAligned( bool iIsAligned )
{
    if( iIsAligned == true )
    {
        mFlags |= ALIGNED;

        // force realigning now. Only needed for the first handle, hence the break
        for( FInbetweenerTrajectory* trajectory : mTrajectoryArray )
        {
            if( trajectory->GetStep(0) == this )
            {
                FInbetweenerHandleTrajectory* handle = trajectory->GetHandle(0);

                handle->Set( handle->GetDirection(), handle->GetLengthRatio() );
            }

            if( trajectory->GetStep(1) == this )
            {
                FInbetweenerHandleTrajectory* handle = trajectory->GetHandle(1);

                handle->Set( handle->GetDirection(), handle->GetLengthRatio() );
            }

            break;
        }
    }
    else
    {
        mFlags &= (~ALIGNED);
    }
}
#pragma warning(pop)

bool
FInbetweenerStep::IsAligned()
{
    return ( mFlags & ALIGNED ) ? true : false;
}

FInbetweenerRoute*
FInbetweenerStep::GetRoute()
{
    return mRoute;
}

void
FInbetweenerStep::AddTrajectory( FInbetweenerTrajectory* iTrajectory )
{
    mTrajectoryArray.push_back( iTrajectory );
}
