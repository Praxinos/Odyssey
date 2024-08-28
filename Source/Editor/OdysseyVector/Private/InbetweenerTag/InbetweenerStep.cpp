#include "InbetweenerTag/InbetweenerStep.h"
#include "InbetweenerTag/InbetweenerTrajectory.h"

FInbetweenerStep::FInbetweenerStep()
    : mFlags ( ALIGNED )
{
    mTrajectoryArray.reserve( 2 );
}

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

bool
FInbetweenerStep::IsAligned()
{
    return ( mFlags & ALIGNED ) ? true : false;
}

void
FInbetweenerStep::AddTrajectory( FInbetweenerTrajectory* iTrajectory )
{
    mTrajectoryArray.push_back( iTrajectory );
}
