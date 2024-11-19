// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

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

/*
    for( FInbetweenerTrajectory& trajectory : mTrajectoryBuffer )
    {
        std::vector<float> spacingBuffer;
        std::vector<::ULIS::FVec2D> pointBuffer;

        trajectory.GetBreakdown()->GetChart()->GetSpacing( spacingBuffer );

        pointBuffer.reserve( spacingBuffer.size() );

        for( FChartDivision& inbetween : trajectory.GetBreakdown()->GetChart()->GetDivisionBuffer() )
        {
            FInbetweenerGrid* grid = trajectory.GetBreakdown()->GetGrid();
            ::ULIS::FVec2D gridPoint;

            grid->ComputeARAPInterpolation( &inbetween, false );

            gridPoint = grid->GetQuadBuffer()[GetQuadIndex()].GetPoint( eInbetweenerPointPositionType::InterpPosition
                                                                      , mQuadU
                                                                      , mQuadV );

            pointBuffer.push_back( FOdysseyVector::MapPoint( inbetween.drawing->localMatrix, gridPoint ) );
        }

        trajectory.FitBezier(  pointBuffer, spacingBuffer );
    }
*/
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
    if( ( iTagInvalidationFlags & FOdysseyVectorTagInbetweener::INVALIDATE_ROUTES     )
     || ( iTagInvalidationFlags & FOdysseyVectorTagInbetweener::INVALIDATE_TARGETGRID )
     || ( iTagInvalidationFlags & FOdysseyVectorTagInbetweener::INVALIDATE_SPACING    ) ) // when chart is modified
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

void
FInbetweenerRoute::SetInbetweenerTag( FOdysseyVectorTagInbetweener* iInbetweenerTag )
{
    mInbetweenerTag = iInbetweenerTag;
}
