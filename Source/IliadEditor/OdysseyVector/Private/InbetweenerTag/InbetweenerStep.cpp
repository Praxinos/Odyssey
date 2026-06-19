// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "InbetweenerTag/InbetweenerStep.h"
#include "InbetweenerTag/InbetweenerRoute.h"
#include "InbetweenerTag/InbetweenerTrajectory.h"
#include "OdysseyVectorTagInbetweener.h"
#include "OdysseyVector.h"

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

        // force realigning now. Only needed for the first handle
        FInbetweenerTrajectory* trajectory = mTrajectoryArray.size() > 0 ? mTrajectoryArray[0] : nullptr;
        if( trajectory )
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

uint32
FInbetweenerStep::GetIndex()
{
    return ( this - &mRoute->GetStepBuffer()[0] );
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

void
FInbetweenerStep::SmoothTrajectories()
{
    ::ULIS::FVec2D combinedVector = ::ULIS::FVec2D( 0, 0 );

    if( mTrajectoryArray.size() == 2 )
    {
        for( FInbetweenerTrajectory* trajectory : mTrajectoryArray )
        {
            ::ULIS::FVec2D *cubicBezier = trajectory->GetCubicBezier();
            ::ULIS::FVec2D directionVector = ( trajectory->GetStep(0) == this ) ? cubicBezier[3] - cubicBezier[0]
                                                                                : cubicBezier[0] - cubicBezier[3];

            if( directionVector.DistanceSquared() )
            {
                directionVector.Normalize();

                combinedVector += directionVector;
            }
        }

        if( combinedVector.DistanceSquared() )
        {
            combinedVector.Normalize();

            for( FInbetweenerTrajectory* trajectory : mTrajectoryArray )
            {
                ::ULIS::FVec2D perpendicularVector = ::ULIS::FVec2D( -combinedVector.y, combinedVector.x );
                ::ULIS::FVec2D *cubicBezier = trajectory->GetCubicBezier();
                ::ULIS::FVec2D directionVector = ( trajectory->GetStep(0) == this ) ? cubicBezier[3] - cubicBezier[0]
                                                                                    : cubicBezier[0] - cubicBezier[3];

                if ( directionVector.DotProduct( perpendicularVector ) < 0.0f )
                {
                    perpendicularVector = -perpendicularVector;
                }

                if ( trajectory->GetStep(0) == this )
                {
                    trajectory->GetHandle(0)->Set( FOdysseyVector::MapVector( GetDrawing()->inverseMatrix, perpendicularVector )
                                                 , trajectory->GetHandle(0)->GetLengthRatio() );
                }

                if ( trajectory->GetStep(1) == this )
                {
                    trajectory->GetHandle(1)->Set( FOdysseyVector::MapVector( GetDrawing()->inverseMatrix, perpendicularVector )
                                                 , trajectory->GetHandle(1)->GetLengthRatio() );
                }
            }
        }
    }
}

FInbetweenerDrawing*
FInbetweenerStep::GetDrawing()
{
    for( FInbetweenerTrajectory* trajectory : mTrajectoryArray )
    {
        if ( trajectory->GetStep(0) == this )
        {
            uint32 drawingIndex = trajectory->GetBreakdown()->GetSourceDrawingIndex();

            return mRoute->GetInbetweenerTag()->GetDrawing( drawingIndex );
        }

        if ( trajectory->GetStep(1) == this )
        {
            uint32 drawingIndex = trajectory->GetBreakdown()->GetTargetDrawingIndex();

            return mRoute->GetInbetweenerTag()->GetDrawing( drawingIndex );
        }
    }

    return nullptr;
}
