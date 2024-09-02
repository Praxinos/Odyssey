#include "InbetweenerTag/InbetweenerRoute.h"
#include "InbetweenerTag/InbetweenerGrid.h"
#include "InbetweenerTag/InbetweenerBreakdown.h"
#include "OdysseyVectorTagInbetweener.h"
#include "OdysseyVector.h"

FInbetweenerRoute::~FInbetweenerRoute()
{
}

FInbetweenerRoute::FInbetweenerRoute( FOdysseyVectorTagInbetweener* iInbetweenerTag
                                    , uint32 iQuadIndex
                                    , double iQuadU
                                    , double iQuadV )
    : mInbetweenerTag( iInbetweenerTag )
{
    Init( iQuadIndex, iQuadU, iQuadV );
}

void
FInbetweenerRoute::Init( uint32 iQuadIndex
                       , double iQuadU
                       , double iQuadV )
{
    uint32 breakdownCount = mInbetweenerTag->GetBreakdownList().size();
    uint32 i = 0;

    mQuadIndex = iQuadIndex;
    mQuadU = iQuadU;
    mQuadV = iQuadV;

    Resize();
}

void
FInbetweenerRoute::Resize()
{
    uint32 breakdownCount = mInbetweenerTag->GetBreakdownCount();
    std::vector<FInbetweenerTrajectory> saveTrajectoryBuffer = mTrajectoryBuffer;
    uint32 i = 0;

    mStepBuffer.clear();
    mStepBuffer.reserve( breakdownCount + 1 );

    for( uint32 j = 0; j < ( breakdownCount + 1 ); j++ )
    {
        mStepBuffer.emplace_back( this );
    }

    mTrajectoryBuffer.clear();
    mTrajectoryBuffer.reserve( breakdownCount );

    for( FInbetweenerBreakdown* breakdown : mInbetweenerTag->GetBreakdownList() )
    {
        uint32 n = ( i + 1 );

        mTrajectoryBuffer.emplace_back( this, &mStepBuffer[i], &mStepBuffer[n], breakdown );

        if( i < saveTrajectoryBuffer.size() )
        {
            // restore values for trajectories that are kept
            mTrajectoryBuffer[i].Import( saveTrajectoryBuffer[i] );
        }

        i++;
    }
}

std::vector<FInbetweenerTrajectory>&
FInbetweenerRoute::GetTrajectoryBuffer()
{
    return mTrajectoryBuffer;
}

std::vector<FInbetweenerStep>&
FInbetweenerRoute::GetStepBuffer()
{
    return mStepBuffer;
}

void
FInbetweenerRoute::Update( uint32 iUpdateFlags
                         , uint64 iOwnerInvalidationFlags
                         , uint64 iTagInvalidationFlags )
{
    if( ( iTagInvalidationFlags & FOdysseyVectorTagInbetweener::INVALIDATE_ROUTES  )
     || ( iTagInvalidationFlags & FOdysseyVectorTagInbetweener::INVALIDATE_TARGET  ) 
     || ( iTagInvalidationFlags & FOdysseyVectorTagInbetweener::INVALIDATE_SPACING ) ) // when chart is modified
    {
        for( FInbetweenerTrajectory& trajectory : mTrajectoryBuffer )
        {
            // update the bezier
            trajectory.Update();
        }
    }
}

void
FInbetweenerRoute::ResizeWaypoints()
{
    uint32 i = 0;

    for( FInbetweenerBreakdown* breakdown : mInbetweenerTag->GetBreakdownList() )
    {
        uint32 drawingCount = breakdown->GetTargetDrawingIndex() - breakdown->GetSourceDrawingIndex() + 1;

        // update the bezier
        mTrajectoryBuffer[i].Resize( drawingCount );

        i++;
    }
}

uint32
FInbetweenerRoute::GetQuadIndex()
{
    return mQuadIndex;
}

double
FInbetweenerRoute::GetQuadU()
{
    return mQuadU;
}

double
FInbetweenerRoute::GetQuadV()
{
    return mQuadV;
}

FOdysseyVectorTagInbetweener*
FInbetweenerRoute::GetInbetweenerTag()
{
    return mInbetweenerTag;
}
