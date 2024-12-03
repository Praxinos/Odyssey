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
    bEnabled = true;

    Resize();
}


void
FInbetweenerRoute::Reset()
{
    for( FInbetweenerTrajectory& trajectory : mTrajectoryBuffer )
    {
        trajectory.Reset();
    }
}

void
FInbetweenerRoute::Resize()
{
    uint32 breakdownCount = mInbetweenerTag->GetBreakdownCount();
    uint32 trajectoryCount = mTrajectoryBuffer.size();
    std::vector<FInbetweenerTrajectory> saveTrajectoryBuffer = mTrajectoryBuffer;
    uint32 initFrom = breakdownCount > trajectoryCount ? trajectoryCount : UINT_MAX;
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

        FInbetweenerTrajectory& trajectory = mTrajectoryBuffer.emplace_back( this
                                                                           , &mStepBuffer[i]
                                                                           , &mStepBuffer[n]
                                                                           , breakdown );

        if( i < initFrom )
        {
            // restore values for trajectories that are kept
            trajectory.Import( saveTrajectoryBuffer[i] );
        }

        i++;
    }
}

void
FInbetweenerRoute::Fit( uint32 iFitFrom )
{
    uint32 i = 0;

    for( FInbetweenerBreakdown* breakdown : mInbetweenerTag->GetBreakdownList() )
    {
        if( i >= iFitFrom )
        {
            std::vector<FChartDivision>& divisionBuffer = breakdown->GetChart()->GetDivisionBuffer();
            int32 divisionCount = divisionBuffer.size();
            FInbetweenerGrid* grid = breakdown->GetGrid();
            std::vector<::ULIS::FVec2D> pointBuffer;
            std::vector<float> spacingBuffer;
            ::ULIS::FVec2D gridPoint;

            breakdown->GetChart()->GetSpacing( spacingBuffer );

            pointBuffer.reserve( divisionCount );

            gridPoint = grid->GetQuadBuffer()[GetQuadIndex()].GetPoint( eInbetweenerPointPositionType::SourcePosition
                                                                      , mQuadU
                                                                      , mQuadV );

            //// curve fitting is buggy when a breakdown is added. commented-out.
            ////pointBuffer.push_back( FOdysseyVector::MapPoint( divisionBuffer.front().drawing->localMatrix, gridPoint ) );

            //// route must be disable before ARAP Precompute or else ARAP will try to use it.
            //// but the route is not ready yet (not all trajectories are set )
            ////Disable();
            ////if( divisionCount - 2 > 0 )
            ////{
            ////    grid->PrecomputeARAPInterpolation();

            ////    for( int32 j = 1; j < ( divisionCount - 1 ); j++ )
            ////    {
            ////        FChartDivision& inbetween = divisionBuffer[j];

            ////        grid->ComputeARAPInterpolation( &inbetween, false );

            ////        gridPoint = grid->GetQuadBuffer()[GetQuadIndex()].GetPoint( eInbetweenerPointPositionType::InterpPosition
            ////                                                                  , mQuadU
            ////                                                                  , mQuadV );
            ////        pointBuffer.push_back( FOdysseyVector::MapPoint( inbetween.drawing->localMatrix, gridPoint ) );
            ////    }
            ////}
            ////Enable();

            gridPoint = grid->GetQuadBuffer()[GetQuadIndex()].GetPoint( eInbetweenerPointPositionType::TargetPosition
                                                                      , mQuadU
                                                                      , mQuadV );
            ////pointBuffer.push_back( FOdysseyVector::MapPoint( divisionBuffer.back().drawing->localMatrix, gridPoint ) );

            ////mTrajectoryBuffer[i].FitBezier(  pointBuffer, spacingBuffer );
        }

        i++;
    }
}

std::vector<FInbetweenerTrajectory>&
FInbetweenerRoute::GetTrajectoryBuffer()
{
    return mTrajectoryBuffer;
}

bool
FInbetweenerRoute::IsEnabled()
{
    return bEnabled;
}

void
FInbetweenerRoute::Disable()
{
    bEnabled = false;
}

void
FInbetweenerRoute::Enable()
{
    bEnabled = true;
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

void
FInbetweenerRoute::SetInbetweenerTag( FOdysseyVectorTagInbetweener* iInbetweenerTag )
{
    mInbetweenerTag = iInbetweenerTag;
}
