// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Undo/OdysseyVectorUndo.h"
#include "OdysseyVectorTagInbetweener.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorVertex.h"
#include "OdysseyVectorSegmentCubic.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorPath.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorLayer.h"
#include "OdysseyVectorCell.h"
#include "Misc/OdysseyUndoDelegates.h"

FOdysseyVectorUndo::~FOdysseyVectorUndo()
{
}

FOdysseyVectorUndo::FOdysseyVectorUndo( FOdysseyVectorLayer* iSharedEnv, uint64 iReturnFlags )
    : mApplied( true )
    , mLayer( iSharedEnv )
    , mReturnFlags( iReturnFlags )
{
}

void
FOdysseyVectorUndo::Apply( UObject* iIgnored )
{
    mApplied = true;
}

void
FOdysseyVectorUndo::Revert( UObject* iIgnored )
{
    mApplied = false;
}

void
FOdysseyVectorUndo::Update()
{
    // call callbacks if any (for refreshing GUI e.g)
    FOdysseyUndoDelegates::Get().OnAfterUndoRedo().AddLambda(
        [this]( bool iIsRedo )
        {
            // update invalidated objects
            mLayer->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

            mLayer->Notify( mReturnFlags );

            // prepare for full redraw
            for( FOdysseyVectorCell* cell : mLayer->GetInvalidateCellList() )
            {
                cell->InvalidateRect();
            }

            mLayer->RequestRedraw( nullptr, 0 );
        }
    );
}

FSnapshotTrajectory::~FSnapshotTrajectory()
{
    if( mInitialState )
        delete mInitialState;

    if( mAlteredState )
        delete mAlteredState;
}

FSnapshotTrajectory::FSnapshotTrajectory( FInbetweenerTrajectory* iTrajectory
                                        , uint64 iSnapshotFlags
                                        , eSnapshotState iState )
    : mSnapshotFlags( iSnapshotFlags )
    , mRoute( iTrajectory->GetRoute() )
    , mIndex( iTrajectory - &iTrajectory->GetRoute()->GetTrajectoryBuffer()[0] )
    , mInitialState ( nullptr )
    , mAlteredState ( nullptr )
{
    RecordState( iState );
}

// static
void
FSnapshotTrajectory::WaypointSpacingToArray( FInbetweenerTrajectory* iTrajectory
                                           , std::vector<float>& oSpacingBuffer )
{
    std::vector<FInbetweenerWaypoint>& waypointBuffer = iTrajectory->GetWaypointBuffer();

    oSpacingBuffer.clear();
    oSpacingBuffer.reserve( waypointBuffer.size() );

    for( FInbetweenerWaypoint& waypoint : waypointBuffer )
    {
        oSpacingBuffer.emplace_back( waypoint.GetT() );
    }
}

void
FSnapshotTrajectory::RecordState( eSnapshotState iStateType )
{
    FSnapshotTrajectory::State* requestedState = nullptr;

    switch( iStateType )
    {
        case eSnapshotState::Initial :
            if( mInitialState == nullptr )
                mInitialState = new FSnapshotTrajectory::State();

            requestedState = mInitialState;
        break;

        case eSnapshotState::Altered :
            if( mAlteredState == nullptr )
                mAlteredState = new FSnapshotTrajectory::State();

            requestedState = mAlteredState;
        break;

        default :
        break;
    }

    if( requestedState )
    {
        if( requestedState->inited == false )
        {
            FInbetweenerTrajectory* trajectory = &mRoute->GetTrajectoryBuffer()[mIndex];

            if( mSnapshotFlags & FSnapshotFlags::Trajectory::BEZIER )
            {
                requestedState->handleDirection[0] = trajectory->GetHandle(0)->GetDirection();
                requestedState->handleDirection[1] = trajectory->GetHandle(1)->GetDirection();
                requestedState->handleLengthRatio[0] = trajectory->GetHandle(0)->GetLengthRatio();
                requestedState->handleLengthRatio[1] = trajectory->GetHandle(1)->GetLengthRatio();
            }

            if( mSnapshotFlags & FSnapshotFlags::Trajectory::WAYPOINTS )
            {
                WaypointSpacingToArray( trajectory, requestedState->waypointSpacingBuffer );
            }

            requestedState->inited = true;
        }
    }
}

bool
FSnapshotTrajectory::LoadState( eSnapshotState iStateType )
{
    FInbetweenerTrajectory* trajectory = &mRoute->GetTrajectoryBuffer()[mIndex];
    FSnapshotTrajectory::State* requestedState = nullptr;

    switch( iStateType )
    {
        case eSnapshotState::Initial :
            requestedState = mInitialState;
        break;

        case eSnapshotState::Altered :
            requestedState = mAlteredState;
        break;

        default :
        break;
    }

    if( requestedState )
    {
        if( mSnapshotFlags & FSnapshotFlags::Trajectory::BEZIER )
        {
            trajectory->GetHandle(0)->Set( requestedState->handleDirection[0], requestedState->handleLengthRatio[0] );
            trajectory->GetHandle(1)->Set( requestedState->handleDirection[1], requestedState->handleLengthRatio[1] );
        }

        if( mSnapshotFlags & FSnapshotFlags::Trajectory::WAYPOINTS )
        {
            std::vector<FInbetweenerWaypoint>& waypointBuffer = trajectory->GetWaypointBuffer();

            waypointBuffer.clear();
            waypointBuffer.reserve( requestedState->waypointSpacingBuffer.size() );

            for( int i = 0; i < requestedState->waypointSpacingBuffer.size(); i++ )
            {
                FInbetweenerWaypoint& waypoint = waypointBuffer.emplace_back( trajectory );

                waypoint.SetT( requestedState->waypointSpacingBuffer[i] );
            }
        }

        return true; // loading succeded
    }

    return false;
}

FSnapshotStep::~FSnapshotStep()
{
    if( mInitialState )
        delete mInitialState;

    if( mAlteredState )
        delete mAlteredState;
}

FSnapshotStep::FSnapshotStep( FInbetweenerStep* iStep, eSnapshotState iState )
    : mRoute( iStep->GetRoute() )
    , mIndex( iStep - &iStep->GetRoute()->GetStepBuffer()[0] )
    , mInitialState ( nullptr )
    , mAlteredState ( nullptr )
{
    RecordState( iState );
}

void
FSnapshotStep::RecordState( eSnapshotState iStateType )
{
    FSnapshotStep::State* requestedState = nullptr;

    switch( iStateType )
    {
        case eSnapshotState::Initial :
            if( mInitialState == nullptr )
                mInitialState = new FSnapshotStep::State();

            requestedState = mInitialState;
        break;

        case eSnapshotState::Altered :
            if( mAlteredState == nullptr )
                mAlteredState = new FSnapshotStep::State();

            requestedState = mAlteredState;
        break;

        default :
        break;
    }

    if( requestedState )
    {
        if( requestedState->inited == false )
        {
            requestedState->aligned = mRoute->GetStepBuffer()[mIndex].IsAligned();

            requestedState->inited = true;
        }
    }
}

bool
FSnapshotStep::LoadState( eSnapshotState iStateType )
{
    FInbetweenerStep* step = &mRoute->GetStepBuffer()[mIndex];
    FSnapshotStep::State* requestedState = nullptr;

    switch( iStateType )
    {
        case eSnapshotState::Initial :
            requestedState = mInitialState;
        break;

        case eSnapshotState::Altered :
            requestedState = mAlteredState;
        break;

        default :
        break;
    }

    if( requestedState )
    {
        step->SetAligned( requestedState->aligned );

        return true;
    }

    return false;
}

FSnapshotRoute::~FSnapshotRoute()
{
    if( mInitialState )
        delete mInitialState;

    if( mAlteredState )
        delete mAlteredState;
}

FSnapshotRoute::FSnapshotRoute( FInbetweenerRoute* iRoute
                              , uint64 iSnapshotflags
                              , uint64 iTrajectorySnapshotFlags
                              , eSnapshotState iStateType )
    : mRoute( iRoute )
    , mSnapshotFlags( iSnapshotflags ) // unused
    , mTrajectorySnapshotFlags( iTrajectorySnapshotFlags )
    , mInitialState ( nullptr )
    , mAlteredState ( nullptr )
{
    RecordState( iStateType );
}

void
FSnapshotRoute::RecordState( eSnapshotState iStateType )
{
    FSnapshotRoute::State* requestedState = nullptr;

    switch( iStateType )
    {
        case eSnapshotState::Initial :
            if( mInitialState == nullptr )
                mInitialState = new FSnapshotRoute::State();

            requestedState = mInitialState;
        break;

        case eSnapshotState::Altered :
            if( mAlteredState == nullptr )
                mAlteredState = new FSnapshotRoute::State();

            requestedState = mAlteredState;
        break;

        default :
        break;
    }

    if( requestedState )
    {
        if( requestedState->inited == false )
        {
            if( mSnapshotFlags & FSnapshotFlags::Route::TRAJECTORIES )
            {
                requestedState->trajectorySnapshotBuffer.reserve( mRoute->GetInbetweenerTag()->GetBreakdownCount() );

                for( FInbetweenerTrajectory& trajectory : mRoute->GetTrajectoryBuffer() )
                {
                    requestedState->trajectorySnapshotBuffer.emplace_back( &trajectory
                                                                         , mTrajectorySnapshotFlags
                                                                         , iStateType );
                }
            }

            if( mSnapshotFlags & FSnapshotFlags::Route::STEPS )
            {
                requestedState->stepSnapshotBuffer.reserve( mRoute->GetInbetweenerTag()->GetBreakdownCount() + 1 );

                for( FInbetweenerStep& step : mRoute->GetStepBuffer() )
                {
                    requestedState->stepSnapshotBuffer.emplace_back( &step, iStateType );
                }
            }

            requestedState->inited = true;
        }
    }
}

bool
FSnapshotRoute::LoadState( eSnapshotState iStateType )
{
    FSnapshotRoute::State* requestedState = nullptr;

    switch( iStateType )
    {
        case eSnapshotState::Initial :
            requestedState = mInitialState;
        break;

        case eSnapshotState::Altered :
            requestedState = mAlteredState;
        break;

        default :
        break;
    }

    if( requestedState )
    {
        if( mSnapshotFlags & FSnapshotFlags::Route::TRAJECTORIES )
        {
            for( FSnapshotTrajectory& trajectorySnapshot : requestedState->trajectorySnapshotBuffer )
            {
                trajectorySnapshot.LoadState( iStateType );
            }
        }

        if( mSnapshotFlags & FSnapshotFlags::Route::STEPS )
        {
            for( FSnapshotStep& stepSnapshot : requestedState->stepSnapshotBuffer )
            {
                stepSnapshot.LoadState( iStateType );
            }
        }

        return true;
    }

    return false;
}

FSnapshotLayout::State::~State()
{
    for( FInbetweenerBreakdown* breakdown : breakdownArray )
    {
        if ( breakdown->GetInbetweenerTag() == nullptr )
        {
            delete breakdown;
        }
    }
}

FSnapshotLayout::~FSnapshotLayout()
{
    if( mInitialState )
        delete mInitialState;

    if( mAlteredState )
        delete mAlteredState;
}

FSnapshotLayout::FSnapshotLayout( FOdysseyVectorTagInbetweener* iInbetweenerTag
                                , eSnapshotState iStateType )
    : mInbetweenerTag ( iInbetweenerTag )
    , mInitialState ( nullptr )
    , mAlteredState ( nullptr )
{
    RecordState( iStateType );
}

void
FSnapshotLayout::RecordState( eSnapshotState iStateType )
{
    FSnapshotLayout::State* requestedState = nullptr;

    switch( iStateType )
    {
        case eSnapshotState::Initial :
            if( mInitialState == nullptr )
                mInitialState = new FSnapshotLayout::State();

            requestedState = mInitialState;
        break;

        case eSnapshotState::Altered :
            if( mAlteredState == nullptr )
                mAlteredState = new FSnapshotLayout::State();

            requestedState = mAlteredState;
        break;

        default :
        break;
    }

    if( requestedState )
    {
        if( requestedState->inited == false )
        {
            uint32 breakdownCount = mInbetweenerTag->GetBreakdownCount();

            requestedState->breakdownArray.clear();
            requestedState->breakdownArray.reserve( breakdownCount );

            requestedState->targetBuffer.clear();
            requestedState->targetBuffer.reserve( breakdownCount );

            for( FInbetweenerBreakdown* breakdown : mInbetweenerTag->GetBreakdownList() )
            {
                requestedState->breakdownArray.emplace_back( breakdown );
                requestedState->targetBuffer.emplace_back( breakdown->GetTargetDrawingIndex() );
            }

            requestedState->inited = true;
        }
    }
}

bool
FSnapshotLayout::LoadState( eSnapshotState iStateType )
{
    FSnapshotLayout::State* requestedState = nullptr;

    switch( iStateType )
    {
        case eSnapshotState::Initial :
            requestedState = mInitialState;
        break;

        case eSnapshotState::Altered :
            requestedState = mAlteredState;
        break;

        default :
        break;
    }

    if( requestedState )
    {
        // erase all breakdowns except the default one
        mInbetweenerTag->ResetLayout( false );

        for( uint32 i = 0; i < requestedState->breakdownArray.size(); i++ )
        {
            FInbetweenerBreakdown* breakdown =  requestedState->breakdownArray[i];

            mInbetweenerTag->AddBreakdown( requestedState->breakdownArray[i]
                                         , requestedState->targetBuffer[i]
                                         , false
                                         , false );
        }

        return true;
    }

    return false;
}

FSnapshotDynamics::~FSnapshotDynamics()
{
    for( FInbetweenerRoute* route : mInitialState->routeArray )
    {
        if( route->GetInbetweenerTag() == nullptr )
        {
            delete route;
        }
    }

    for( FInbetweenerRoute* route : mAlteredState->routeArray )
    {
        if( route->GetInbetweenerTag() == nullptr )
        {
            delete route;
        }
    }

    if( mInitialState )
        delete mInitialState;

    if( mAlteredState )
        delete mAlteredState;
}

FSnapshotDynamics::FSnapshotDynamics()
{

}

FSnapshotDynamics::FSnapshotDynamics( FOdysseyVectorTagInbetweener* iInbetweenerTag
                                    , eSnapshotState iStateType )
    : mInbetweenerTag( iInbetweenerTag )
    , mInitialState ( nullptr )
    , mAlteredState ( nullptr )
{
    RecordState( iStateType );
}

void
FSnapshotDynamics::RecordState( eSnapshotState iStateType )
{
    FSnapshotDynamics::State* requestedState = nullptr;

    switch( iStateType )
    {
        case eSnapshotState::Initial :
            if( mInitialState == nullptr )
                mInitialState = new FSnapshotDynamics::State();

            requestedState = mInitialState;
        break;

        case eSnapshotState::Altered :
            if( mAlteredState == nullptr )
                mAlteredState = new FSnapshotDynamics::State();

            requestedState = mAlteredState;
        break;

        default :
        break;
    }

    if( requestedState )
    {
        if( requestedState->inited == false )
        {
            requestedState->routeArray.clear();
            requestedState->routeArray.reserve( mInbetweenerTag->GetRouteList().size() );

            for( FInbetweenerRoute* route : mInbetweenerTag->GetRouteList() )
            {
                requestedState->routeArray.push_back( route );
            }

            requestedState->inited = true;
        }
    }
}

bool
FSnapshotDynamics::LoadState( eSnapshotState iStateType )
{
    FSnapshotDynamics::State* requestedState = nullptr;

    switch( iStateType )
    {
        case eSnapshotState::Initial :
            requestedState = mInitialState;
        break;

        case eSnapshotState::Altered :
            requestedState = mAlteredState;
        break;

        default :
        break;
    }

    if( requestedState )
    {
        mInbetweenerTag->RemoveAllRoutes();

        for( FInbetweenerRoute* route : requestedState->routeArray )
        {
            mInbetweenerTag->AddRoute( route );
        }

        return true;
    }

    return false;
}

FSnapshotInbetweenerChart::~FSnapshotInbetweenerChart()
{
    if( mInitialState )
        delete mInitialState;

    if( mAlteredState )
        delete mAlteredState;
}

FSnapshotInbetweenerChart::FSnapshotInbetweenerChart( FInbetweenerChart* iChart
                                                    , uint64 iSnapshotFlags
                                                    , eSnapshotState iStateType )
    : mChart( iChart )
    , mSnapshotFlags( iSnapshotFlags )
    , mInitialState ( nullptr )
    , mAlteredState ( nullptr )
{
    RecordState( iStateType );
}

void
FSnapshotInbetweenerChart::RecordState( eSnapshotState iStateType )
{
    FSnapshotInbetweenerChart::State* requestedState = nullptr;

    switch( iStateType )
    {
        case eSnapshotState::Initial :
            if( mInitialState == nullptr )
                mInitialState = new FSnapshotInbetweenerChart::State();

            requestedState = mInitialState;
        break;

        case eSnapshotState::Altered :
            if( mAlteredState == nullptr )
                mAlteredState = new FSnapshotInbetweenerChart::State();

            requestedState = mAlteredState;
        break;

        default :
        break;
    }

    if( requestedState->inited == false )
    {
        uint32 drawingCount = mChart->GetBreakdown()->GetDrawingCount();

        // save bezier
        if( mSnapshotFlags & FSnapshotFlags::Chart::BEZIER )
        {
            requestedState->HUDBezier[0] = mChart->GetHUDBezier()->GetPoints()[0].GetPosition();
            requestedState->HUDBezier[1] = mChart->GetHUDBezier()->GetPoints()[1].GetPosition();
            requestedState->HUDBezier[2] = mChart->GetHUDBezier()->GetPoints()[2].GetPosition();
        }

        // save spacing
        if( mSnapshotFlags & FSnapshotFlags::Chart::SPACING )
        {
            requestedState->spacing.resize( drawingCount );

            for( uint32 i = 0; i < drawingCount; i++ )
            {
                 requestedState->spacing[i] = mChart->GetInbetweenBuffer()[i].GetSpacing();
            }
        }

        requestedState->inited = true;
    }
}

bool
FSnapshotInbetweenerChart::LoadState( eSnapshotState iStateType )
{
    FSnapshotInbetweenerChart::State* requestedState = nullptr;

    switch( iStateType )
    {
        case eSnapshotState::Initial :
            requestedState = mInitialState;
        break;

        case eSnapshotState::Altered :
            requestedState = mAlteredState;
        break;

        default :
        break;
    }

    // restore bezier
    if( mSnapshotFlags & FSnapshotFlags::Chart::BEZIER )
    {
        mChart->GetHUDBezier()->GetPoints()[0].SetPosition( requestedState->HUDBezier[0].x, requestedState->HUDBezier[0].y );
        mChart->GetHUDBezier()->GetPoints()[1].SetPosition( requestedState->HUDBezier[1].x, requestedState->HUDBezier[1].y );
        mChart->GetHUDBezier()->GetPoints()[2].SetPosition( requestedState->HUDBezier[2].x, requestedState->HUDBezier[2].y );
    }

    // restore spacing
    if( mSnapshotFlags & FSnapshotFlags::Chart::SPACING )
    {
        for( uint32 i = 0; i < requestedState->spacing.size(); i++ )
        {
            mChart->GetInbetweenBuffer()[i].SetSpacing( requestedState->spacing[i] );
        }
    }

    return true;
}

FSnapshotInbetweenerBreakdown::~FSnapshotInbetweenerBreakdown()
{
    if( mInitialState )
        delete mInitialState;

    if( mAlteredState )
        delete mAlteredState;
}

FSnapshotInbetweenerBreakdown::FSnapshotInbetweenerBreakdown( FInbetweenerBreakdown* iBreakdown
                                                            , uint64 iSnapshotFlags
                                                            , eSnapshotState iState )
    : mBreakdown( iBreakdown )
    , mSnapshotFlags( iSnapshotFlags )
    , mInitialState ( nullptr )
    , mAlteredState ( nullptr )
{
    RecordState( iState );
}

void
FSnapshotInbetweenerBreakdown::RecordState( eSnapshotState iStateType )
{
    FSnapshotInbetweenerBreakdown::State* requestedState = nullptr;

    switch( iStateType )
    {
        case eSnapshotState::Initial :
            if( mInitialState == nullptr )
                mInitialState = new FSnapshotInbetweenerBreakdown::State();

            requestedState = mInitialState;
        break;

        case eSnapshotState::Altered :
            if( mAlteredState == nullptr )
                mAlteredState = new FSnapshotInbetweenerBreakdown::State();

            requestedState = mAlteredState;
        break;

        default :
        break;
    }

    if( requestedState )
    {
        if( requestedState->inited == false )
        {
            if( mSnapshotFlags & FSnapshotFlags::Breakdown::GRIDGEOMETRY )
            {
                mBreakdown->GetGrid()->GetGeometry( requestedState->gridGeometry
                                                  , eInbetweenerPointPositionType::TargetPosition );
            }

            if( mSnapshotFlags & FSnapshotFlags::Breakdown::TRANSFORMATIONS )
            {
                mBreakdown->GetTargetTransform( requestedState->translationX
                                              , requestedState->translationY
                                              , requestedState->rotation
                                              , requestedState->scalingX
                                              , requestedState->scalingY );
            }

            if( mSnapshotFlags & FSnapshotFlags::Breakdown::TARGETVISIBILITY )
            {
                requestedState->targetVisibility  = mBreakdown->IsTargetVisible();
            }

            if( mSnapshotFlags & FSnapshotFlags::Breakdown::CHART )
            {
                requestedState->chartSnapshotBuffer.emplace_back( mBreakdown->GetChart()
                                                                , ( ( mSnapshotFlags & FSnapshotFlags::Breakdown::CHART ) ? FSnapshotFlags::ALL : 0 )
                                                                , iStateType );
            }

            requestedState->inited = true;
        }
    }
}

bool
FSnapshotInbetweenerBreakdown::LoadState( eSnapshotState iStateType )
{
    FSnapshotInbetweenerBreakdown::State* requestedState = nullptr;

    switch( iStateType )
    {
        case eSnapshotState::Initial :
            requestedState = mInitialState;
        break;

        case eSnapshotState::Altered :
            requestedState = mAlteredState;
        break;

        default :
        break;
    }

    if( requestedState )
    {
        if( mSnapshotFlags & FSnapshotFlags::Breakdown::GRIDGEOMETRY )
        {
            mBreakdown->GetGrid()->SetGeometry( requestedState->gridGeometry
                                              , eInbetweenerPointPositionType::TargetPosition
                                              , true );
        }

        if( mSnapshotFlags & FSnapshotFlags::Breakdown::TRANSFORMATIONS )
        {
            mBreakdown->SetTargetTransform( requestedState->translationX
                                          , requestedState->translationY
                                          , requestedState->rotation
                                          , requestedState->scalingX
                                          , requestedState->scalingY );

            mBreakdown->UpdateMatrix();
        }

        if( mSnapshotFlags & FSnapshotFlags::Breakdown::TARGETVISIBILITY )
        {
            mBreakdown->SetTargetVisibility( requestedState->targetVisibility );
        }

        if( mSnapshotFlags & FSnapshotFlags::Breakdown::CHART )
        {
            requestedState->chartSnapshotBuffer[0].LoadState( iStateType );
        }

        return true; // restore succeeded
    }

    return false;
}

FSnapshotTagInbetweener::~FSnapshotTagInbetweener()
{
    if( mInitialState )
        delete mInitialState;

    if( mAlteredState )
        delete mAlteredState;
}

FSnapshotTagInbetweener::FSnapshotTagInbetweener( FOdysseyVectorTagInbetweener* iInbetweenerTag
                                                , uint64 iSnapshotFlags
                                                , uint64 iBreakdownSnapshotFlags
                                                , uint64 iRouteSnapshotFlags
                                                , uint64 iTrajectorySnapshotFlags
                                                , eSnapshotState iStateType )
    : mSnapshotFlags( iSnapshotFlags )
    , mBreakdownSnapshotFlags( iBreakdownSnapshotFlags )
    , mRouteSnapshotFlags( iRouteSnapshotFlags )
    , mTrajectorySnapshotFlags( iTrajectorySnapshotFlags )
    , mInbetweenerTag( iInbetweenerTag )
    , mInitialState( nullptr )
    , mAlteredState( nullptr )
{
    RecordState( iStateType );
}

void
FSnapshotTagInbetweener::RecordState( eSnapshotState iStateType )
{
    FSnapshotTagInbetweener::State *requestedState = nullptr;

    switch( iStateType )
    {
        case eSnapshotState::Initial :
            if( mInitialState == nullptr )
                mInitialState = new FSnapshotTagInbetweener::State();

            requestedState = mInitialState;
        break;

        case eSnapshotState::Altered :
            if( mAlteredState == nullptr )
                mAlteredState = new FSnapshotTagInbetweener::State();

            requestedState = mAlteredState;
        break;

        default :
        break;
    }

    if( requestedState )
    {
        if( requestedState->inited == false )
        {
            requestedState->interpolationDirection = mInbetweenerTag->GetInterpolationDirection();

            if( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::WITHTHICKNESS )
            {
                requestedState->withThickness = mInbetweenerTag->GetWithThickness();
            }

            if( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::CONSTANTWIDTH )
            {
                requestedState->constantWidth = mInbetweenerTag->HasConstantWidth();
            }

            if( ( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::GRIDSIZE )
             || ( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::GRIDTYPE )
             || ( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::SQUARE   ) )
            {
                requestedState->gridSizeX = mInbetweenerTag->GetGridNumQuadX();
                requestedState->gridSizeY = mInbetweenerTag->GetGridNumQuadY();
                requestedState->square    = mInbetweenerTag->IsSquare();
                requestedState->gridType  = mInbetweenerTag->GetGridType();
            }

            if( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::INTERPOLATIONTYPE )
            {
                requestedState->interpolationType = mInbetweenerTag->GetInterpolationType();
            }

            if( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::COLOR )
            {
                requestedState->inbetweenColor = mInbetweenerTag->GetInbetweenColor();
                requestedState->chartColor = mInbetweenerTag->GetChartColor();
                requestedState->gridColor = mInbetweenerTag->GetGridColor();
                requestedState->trajectoryColor = mInbetweenerTag->GetTrajectoryColor();
            }

            if( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::MAPASPOLYLINE )
            {
                requestedState->mapAsPolyline = mInbetweenerTag->GetMapAsPolyline();
            }

            // Snapshot breakdown layout
            if( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::BREAKDOWNS )
            {
                requestedState->layoutSnapshotBuffer.emplace_back( mInbetweenerTag, iStateType );
            }

            // Snapshot breakdowns if any
            if( mBreakdownSnapshotFlags )
            {
                requestedState->inbetweenerBreakdownSnapshotBuffer.reserve( mInbetweenerTag->GetBreakdownCount() );

                for( FInbetweenerBreakdown* breakdown : mInbetweenerTag->GetBreakdownList() )
                {
                    requestedState->inbetweenerBreakdownSnapshotBuffer.emplace_back( breakdown
                                                                                   , mBreakdownSnapshotFlags
                                                                                   , iStateType );
                }
            }

            // Snapshot route layout
            if( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::ROUTES )
            {
                requestedState->dynamicsSnapshotBuffer.emplace_back( mInbetweenerTag, iStateType );
            }

            // Snapshot routes if any
            if( mRouteSnapshotFlags )
            {
                requestedState->routeSnapshotBuffer.reserve( mInbetweenerTag->GetRouteList().size() );

                for( FInbetweenerRoute* route : mInbetweenerTag->GetRouteList() )
                {
                    requestedState->routeSnapshotBuffer.emplace_back( route
                                                                    , mRouteSnapshotFlags
                                                                    , mTrajectorySnapshotFlags
                                                                    , iStateType );
                }
            }

            requestedState->inited = true;
        }
    }
}

bool
FSnapshotTagInbetweener::LoadState( eSnapshotState iStateType )
{
    FSnapshotTagInbetweener::State *requestedState = nullptr;

    switch( iStateType )
    {
        case eSnapshotState::Initial :
            requestedState = mInitialState;
        break;

        case eSnapshotState::Altered :
            requestedState = mAlteredState;
        break;

        default :
        break;
    }

    if( requestedState )
    {
        mInbetweenerTag->SetInterpolationDirection( requestedState->interpolationDirection );

        if( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::WITHTHICKNESS )
        {
            mInbetweenerTag->SetWithThickness( requestedState->withThickness );
        }

        if( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::CONSTANTWIDTH )
        {
            mInbetweenerTag->SetConstantWidth( requestedState->constantWidth );
        }

        if( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::MAPASPOLYLINE )
        {
            mInbetweenerTag->SetMapAsPolyline( requestedState->mapAsPolyline );
        }

        if( ( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::GRIDSIZE )
         || ( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::GRIDTYPE )
         || ( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::SQUARE   ) )
        {
            mInbetweenerTag->SetGrid( requestedState->gridType
                                    , requestedState->gridSizeX
                                    , requestedState->gridSizeY
                                    , requestedState->square );
        }

        if( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::INTERPOLATIONTYPE )
        {
            mInbetweenerTag->SetInterpolationType( requestedState->interpolationType );
        }

        if( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::COLOR )
        {
            mInbetweenerTag->SetInbetweenColor( requestedState->inbetweenColor );
            mInbetweenerTag->SetChartColor( requestedState->chartColor );
            mInbetweenerTag->SetGridColor( requestedState->gridColor );
            mInbetweenerTag->SetTrajectoryColor( requestedState->trajectoryColor );
        }

        // restore breakdown layout
        if( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::BREAKDOWNS )
        {
            requestedState->layoutSnapshotBuffer[0].LoadState( iStateType );
        }

        // restore breakdowns if any
        for( FSnapshotInbetweenerBreakdown& inbetweenerBreakdownSnapshot : requestedState->inbetweenerBreakdownSnapshotBuffer )
        {
            inbetweenerBreakdownSnapshot.LoadState( iStateType );
        }

        // restore route layout
        if( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::ROUTES )
        {
            requestedState->dynamicsSnapshotBuffer[0].LoadState( iStateType );
        }

        // restore routes if any
        for( FSnapshotRoute& routeSnapshot : requestedState->routeSnapshotBuffer )
        {
            routeSnapshot.LoadState( iStateType );
        }

        return true; // restore succeeded
    }

    return false;
}

FSnapshotObject::~FSnapshotObject()
{
    if( mObjectInitialState )
        delete mObjectInitialState;

    if( mObjectAlteredState )
        delete mObjectAlteredState;
}

FSnapshotObject::FSnapshotObject( FOdysseyVectorObject* iObject
                                , uint64 iSnapshotFlags
                                , eSnapshotState iStateType )
    : mSnapshotFlags( iSnapshotFlags )
    , mObject( iObject )
    , mObjectInitialState ( nullptr )
    , mObjectAlteredState ( nullptr )
{
    RecordState( iStateType );
}

void
FSnapshotObject::RecordState( eSnapshotState iStateType )
{
    FSnapshotObject::State *requestedState = nullptr;

    switch( iStateType )
    {
        case eSnapshotState::Initial :
            if( mObjectInitialState == nullptr )
                mObjectInitialState = new FSnapshotObject::State( mObject, mSnapshotFlags, iStateType );

            requestedState = mObjectInitialState;
        break;

        case eSnapshotState::Altered :
            if( mObjectAlteredState == nullptr )
                mObjectAlteredState = new FSnapshotObject::State( mObject, mSnapshotFlags, iStateType );

            requestedState = mObjectAlteredState;
        break;

        default :
        break;
    }

    if( requestedState  )
    {
        if( requestedState->inited == false  )
        {
            if( mSnapshotFlags & FSnapshotFlags::Object::TRANSFORMATIONS )
            {
                mObject->GetTransform( requestedState->translationX
                                     , requestedState->translationY
                                     , requestedState->rotation
                                     , requestedState->scalingX
                                     , requestedState->scalingY );
            }

            if( mSnapshotFlags & FSnapshotFlags::Object::HIERARCHY )
            {
                requestedState->parent = mObject->GetParent();
                requestedState->previousChild = mObject->GetParent()->GetPreviousChild( mObject );
            }

            if( mSnapshotFlags & FSnapshotFlags::Object::OPACITY )
            {
                requestedState->opacity = mObject->GetOpacity();
            }

            if( mSnapshotFlags & FSnapshotFlags::Object::OPACITY )
            {
                requestedState->visibility = mObject->IsVisible( false );
            }

            if( mSnapshotFlags & FSnapshotFlags::Object::NAME )
            {
                requestedState->name = mObject->GetName();
            }

            requestedState->inited = true;
        }
    }
}

bool
FSnapshotObject::LoadState( eSnapshotState iStateType )
{
    FSnapshotObject::State *requestedState = nullptr;

    switch( iStateType )
    {
        case eSnapshotState::Initial :
            requestedState = mObjectInitialState;
        break;

        case eSnapshotState::Altered :
            requestedState = mObjectAlteredState;
        break;

        default :
        break;
    }

    if( requestedState )
    {
        if( mSnapshotFlags & FSnapshotFlags::Object::TRANSFORMATIONS )
        {
            mObject->SetTransform( requestedState->translationX
                                 , requestedState->translationY
                                 , requestedState->rotation
                                 , requestedState->scalingX
                                 , requestedState->scalingY );

            mObject->UpdateMatrix();
        }

        if( mSnapshotFlags & FSnapshotFlags::Object::HIERARCHY )
        {
            uint32 transferRetval = requestedState->parent->TransferChild( mObject
                                                                         , requestedState->previousChild );

            // the object could not be transferred, normally because the previous child
            // has not been transferred yet.
            if( transferRetval == FOdysseyVectorObject::HIERARCHY_CHANGE_ERROR )
            {
                return false;
            }

            if( transferRetval == FOdysseyVectorObject::HIERARCHY_CHANGE_FORBIDDEN )
            {
            // nothing to do, this is nominal for the root object
            }
        }

        if( mSnapshotFlags & FSnapshotFlags::Object::COLORING )
        {
            requestedState->foregroundBucketSnapshot.LoadState( iStateType );
            requestedState->backgroundBucketSnapshot.LoadState( iStateType );
        }

        if( mSnapshotFlags & FSnapshotFlags::Object::OPACITY )
        {
            mObject->SetOpacity( requestedState->opacity );
        }

        if( mSnapshotFlags & FSnapshotFlags::Object::VISIBILITY )
        {
            mObject->SetVisible( requestedState->visibility );
        }

        if( mSnapshotFlags & FSnapshotFlags::Object::NAME )
        {
            mObject->SetName( requestedState->name );
        }

        return true; // restore succeeded
    }

    return false;
}

FSnapshotPoint::~FSnapshotPoint()
{
    if( mPointInitialState )
        delete mPointInitialState;

    if( mPointAlteredState )
        delete mPointAlteredState;
}

FSnapshotPoint::FSnapshotPoint( FOdysseyVectorPoint* iPoint, uint64 iSnapshotFlags, eSnapshotState iStateType )
    : mSnapshotFlags( iSnapshotFlags )
    , mPoint( iPoint )
    , mPointInitialState ( nullptr )
    , mPointAlteredState ( nullptr )
{
    RecordState( iStateType );
}

void
FSnapshotPoint::RecordState( eSnapshotState iStateType )
{
    FSnapshotPoint::State* requestedState = nullptr;

    switch( iStateType )
    {
        case eSnapshotState::Initial :
            if( mPointInitialState == nullptr )
                mPointInitialState = new FSnapshotPoint::State();

            requestedState = mPointInitialState;
        break;

        case eSnapshotState::Altered :
            if( mPointAlteredState == nullptr )
                mPointAlteredState = new FSnapshotPoint::State();

            requestedState = mPointAlteredState;
        break;

        default :
        break;
    }

    if( requestedState )
    {
        if( requestedState->inited == false )
        {
            if( mSnapshotFlags & FSnapshotFlags::Point::POSITION )
            {
                requestedState->coords = mPoint->GetCoords();
            }

            requestedState->inited = true;
        }
    }
}

bool
FSnapshotPoint::LoadState( eSnapshotState iStateType )
{
    FSnapshotPoint::State* requestedState = nullptr;

    switch( iStateType )
    {
        case eSnapshotState::Initial :
            requestedState = mPointInitialState;
        break;

        case eSnapshotState::Altered :
            requestedState = mPointAlteredState;
        break;

        default :
        break;
    }

    if( requestedState )
    {
        if( mSnapshotFlags & FSnapshotFlags::Point::POSITION )
        {
            mPoint->Set( requestedState->coords.x, requestedState->coords.y );
        }
    }

    return true;
}

FSnapshotVertex::~FSnapshotVertex()
{
    if( mVertexInitialState )
        delete mVertexInitialState;

    if( mVertexAlteredState )
        delete mVertexAlteredState;
}

FSnapshotVertex::FSnapshotVertex( FOdysseyVectorVertex* iVertex
                                , uint64 iSnapshotFlags
                                , eSnapshotState iStateType )
    : FSnapshotPoint( iVertex, iSnapshotFlags, eSnapshotState::None )
    , mVertex ( iVertex )
    , mVertexInitialState ( nullptr )
    , mVertexAlteredState ( nullptr )
{
    // Note: even though the ctor for the base class FSnapshotPoint calls RecordState, it will not call the overriden
    // method because vtable don't exist at construction time. So we call it here, knowing that
    // FSnapshotPoint::Recordstate will receive eSnapshotState::None so we don't do thing twice.
    RecordState( iStateType );
}

void
FSnapshotVertex::RecordState( eSnapshotState iStateType )
{
    FSnapshotVertex::State* requestedState = nullptr;

    FSnapshotPoint::RecordState( iStateType );

    switch( iStateType )
    {
        case eSnapshotState::Initial :
            if( mVertexInitialState == nullptr )
                mVertexInitialState = new FSnapshotVertex::State();

            requestedState = mVertexInitialState;
        break;

        case eSnapshotState::Altered :
            if( mVertexAlteredState == nullptr )
                mVertexAlteredState = new FSnapshotVertex::State();

            requestedState = mVertexAlteredState;
        break;

        default :
        break;
    }

    if( requestedState )
    {
        if( requestedState->inited == false )
        {
            if( mSnapshotFlags & FSnapshotFlags::Point::Vertex::ALIGNMENT )
            {
                requestedState->alignment = mVertex->IsHandleAligned();
            }

            if( mSnapshotFlags & FSnapshotFlags::Point::Vertex::LOCK )
            {
                requestedState->locked = mVertex->IsLocked();
            }

            if( mSnapshotFlags & FSnapshotFlags::Point::Vertex::RADIUS )
            {
                requestedState->radius = mVertex->GetRadius();
            }

            requestedState->inited = true;
        }
    }
}

FOdysseyVectorVertex*
FSnapshotVertex::GetVertex()
{
    return mVertex;
}

bool
FSnapshotVertex::LoadState( eSnapshotState iStateType )
{
    FSnapshotVertex::State* requestedState = nullptr;

    FSnapshotPoint::LoadState( iStateType );

    switch( iStateType )
    {
        case eSnapshotState::Initial :
            requestedState = mVertexInitialState;
        break;

        case eSnapshotState::Altered :
            requestedState = mVertexAlteredState;
        break;

        default :
        break;
    }

    if( requestedState )
    {
        if( mSnapshotFlags & FSnapshotFlags::Point::Vertex::ALIGNMENT )
        {
            mVertex->SetHandleAligned( requestedState->alignment );
        }

        if( mSnapshotFlags & FSnapshotFlags::Point::Vertex::LOCK )
        {
            mVertex->SetLocked( requestedState->locked );
        }

        if( mSnapshotFlags & FSnapshotFlags::Point::Vertex::RADIUS )
        {
            mVertex->SetRadius( requestedState->radius );
        }

        return true;
    }

    return false;
}

FSnapshotBucket::~FSnapshotBucket()
{
    if( mBucketInitialState )
        delete mBucketInitialState;

    if( mBucketAlteredState )
        delete mBucketAlteredState;
}

FSnapshotBucket::FSnapshotBucket( FOdysseyVectorBucket* iBucket
                                , uint64 iSnapshotFlags
                                , eSnapshotState iStateType )
    : FSnapshotPoint( iBucket, iSnapshotFlags, eSnapshotState::None )
    , mBucket( iBucket )
    , mBucketInitialState ( nullptr )
    , mBucketAlteredState ( nullptr )
{
    RecordState( iStateType );
}

void
FSnapshotBucket::RecordState( eSnapshotState iStateType )
{
    FSnapshotBucket::State* requestedState = nullptr;

    FSnapshotPoint::RecordState( iStateType );

    switch( iStateType )
    {
        case eSnapshotState::Initial :
            if( mBucketInitialState == nullptr )
                mBucketInitialState = new FSnapshotBucket::State();

            requestedState = mBucketInitialState;
        break;

        case eSnapshotState::Altered :
            if( mBucketAlteredState == nullptr )
                mBucketAlteredState = new FSnapshotBucket::State();

            requestedState = mBucketAlteredState;
        break;

        default :
        break;
    }

    if( requestedState )
    {
        if( requestedState->inited == false )
        {
            if( mSnapshotFlags & FSnapshotFlags::Point::Bucket::COLORMODE )
            {
                requestedState->colorMode = mBucket->GetColorMode();
            }

            if( mSnapshotFlags & FSnapshotFlags::Point::Bucket::SOLIDCOLOR )
            {
                requestedState->solidColor = mBucket->GetSolidColor();
            }

            if( mSnapshotFlags & FSnapshotFlags::Point::Bucket::ROTATION )
            {
                requestedState->rotation = mBucket->GetRotation();
            }

            if( mSnapshotFlags & FSnapshotFlags::Point::Bucket::PROPAGATION )
            {
                requestedState->propagated = mBucket->IsPropagated();
            }

            if( mSnapshotFlags & FSnapshotFlags::Point::Bucket::GRADIENTCOLOR0 )
            {
                requestedState->gradientColor0 = mBucket->GetGradientColor0();
            }

            if( mSnapshotFlags & FSnapshotFlags::Point::Bucket::GRADIENTCOLOR1 )
            {
                requestedState->gradientColor1 = mBucket->GetGradientColor1();
            }

            if( mSnapshotFlags & FSnapshotFlags::Point::Bucket::RADIALRADIUS )
            {
                requestedState->radialRadius = mBucket->GetRadialRadius();
            }

            if( mSnapshotFlags & FSnapshotFlags::Point::Bucket::RADIALOFFSET )
            {
                requestedState->radialOffset = mBucket->GetRadialOffset();
            }

            if( mSnapshotFlags & FSnapshotFlags::Point::Bucket::LINEARP0 )
            {
                requestedState->linearP0 = mBucket->GetLinearP0();
            }

            if( mSnapshotFlags & FSnapshotFlags::Point::Bucket::LINEARP1 )
            {
                requestedState->linearP1 = mBucket->GetLinearP1();
            }

            if( mSnapshotFlags & FSnapshotFlags::Point::Bucket::PALETTEENTRY )
            {
                requestedState->paletteEntry = mBucket->GetPaletteEntry();
            }

            requestedState->inited = true;
        }
    }
}

FOdysseyVectorBucket*
FSnapshotBucket::GetBucket()
{
    return mBucket;
}

bool
FSnapshotBucket::LoadState( eSnapshotState iStateType )
{
    FSnapshotBucket::State* requestedState = nullptr;

    FSnapshotPoint::LoadState( iStateType );

    switch( iStateType )
    {
        case eSnapshotState::Initial :
            requestedState = mBucketInitialState;
        break;

        case eSnapshotState::Altered :
            requestedState = mBucketAlteredState;
        break;

        default :
        break;
    }

    if( requestedState )
    {
        if( mSnapshotFlags & FSnapshotFlags::Point::Bucket::COLORMODE )
        {
            mBucket->SetColorMode( requestedState->colorMode );
        }

        if( mSnapshotFlags & FSnapshotFlags::Point::Bucket::SOLIDCOLOR )
        {
            mBucket->SetSolidColor( requestedState->solidColor );
        }

        if( mSnapshotFlags & FSnapshotFlags::Point::Bucket::LINEARP0 )
        {
            mBucket->SetLinearP0( requestedState->linearP0 );
        }

        if( mSnapshotFlags & FSnapshotFlags::Point::Bucket::LINEARP1 )
        {
            mBucket->SetLinearP1( requestedState->linearP1 );
        }

        if( mSnapshotFlags & FSnapshotFlags::Point::Bucket::PROPAGATION )
        {
            mBucket->SetPropagated( requestedState->propagated );
        }

        if( mSnapshotFlags & FSnapshotFlags::Point::Bucket::GRADIENTCOLOR0 )
        {
            mBucket->SetGradientColor0( requestedState->gradientColor0 );
        }

        if( mSnapshotFlags & FSnapshotFlags::Point::Bucket::GRADIENTCOLOR1 )
        {
            mBucket->SetGradientColor1( requestedState->gradientColor1 );
        }

        if( mSnapshotFlags & FSnapshotFlags::Point::Bucket::RADIALRADIUS )
        {
            mBucket->SetRadialRadius( requestedState->radialRadius );
        }

        if( mSnapshotFlags & FSnapshotFlags::Point::Bucket::RADIALOFFSET )
        {
            mBucket->SetRadialOffset( requestedState->radialOffset );
        }

        if( mSnapshotFlags & FSnapshotFlags::Point::Bucket::PALETTEENTRY )
        {
            mBucket->SetPaletteEntry( requestedState->paletteEntry );
        }

        return true;
    }

    return false;
}

FSnapshotSegmentCubic::~FSnapshotSegmentCubic()
{
    if( mInitialState )
        delete mInitialState;

    if( mAlteredState )
        delete mAlteredState;
}

FSnapshotSegmentCubic::FSnapshotSegmentCubic( FOdysseyVectorSegmentCubic* iCubicSegment
                                            , uint64 iSnapshotFlags
                                            , eSnapshotState iSnapshotState )
    : mSnapshotFlags( iSnapshotFlags )
    , mCubicSegment( iCubicSegment )
    , mInitialState( nullptr )
    , mAlteredState( nullptr )
{
    RecordState( iSnapshotState );
}

FOdysseyVectorSegmentCubic*
FSnapshotSegmentCubic::GetCubicSegment()
{
    return mCubicSegment;
}

void
FSnapshotSegmentCubic::RecordState( eSnapshotState iStateType )
{
    FSnapshotSegmentCubic::State* requestedState = nullptr;

    switch( iStateType )
    {
        case eSnapshotState::Initial :
            if( mInitialState == nullptr )
                mInitialState = new FSnapshotSegmentCubic::State();

            requestedState = mInitialState;
        break;

        case eSnapshotState::Altered :
            if( mAlteredState == nullptr )
                mAlteredState = new FSnapshotSegmentCubic::State();

            requestedState = mAlteredState;
        break;

        default :
        break;
    }

    if( requestedState->inited == false )
    {
        if( mSnapshotFlags & FSnapshotFlags::Segment::Cubic::HANDLES )
        {
            requestedState->handleCoords[0] = mCubicSegment->GetHandle(0)->GetCoords();
            requestedState->handleCoords[1] = mCubicSegment->GetHandle(1)->GetCoords();
        }

        requestedState->inited = true;
    }
}

bool
FSnapshotSegmentCubic::LoadState( eSnapshotState iStateType )
{
    FSnapshotSegmentCubic::State *requestedState = nullptr;

    switch( iStateType )
    {
        case eSnapshotState::Initial :
            requestedState = mInitialState;
        break;

        case eSnapshotState::Altered :
            requestedState = mAlteredState;
        break;

        default :
        break;
    }

    if( requestedState )
    {
        if( mSnapshotFlags & FSnapshotFlags::Segment::Cubic::HANDLES )
        {
            mCubicSegment->GetHandle(0)->Set( requestedState->handleCoords[0].x, requestedState->handleCoords[0].y );
            mCubicSegment->GetHandle(1)->Set( requestedState->handleCoords[1].x, requestedState->handleCoords[1].y );
        }

        return true;
    }

    return false;
}

FSnapshotPath::~FSnapshotPath()
{
    FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(mObject);

    // free removed vertices and segments
    if( mSnapshotFlags & FSnapshotFlags::Object::Path::TOPOLOGY )
    {
        for( FOdysseyVectorVertex* vertex : mPathInitialState->topologyVertexList )
        {
            if( path->HasVertex( vertex ) == false )
            {
                delete vertex;
            }
        }

        for( FOdysseyVectorSegment* segment : mPathInitialState->topologySegmentList )
        {
            if( path->HasSegment( segment ) == false )
            {
                delete segment;
            }
        }



        for( FOdysseyVectorVertex* vertex : mPathAlteredState->topologyVertexList )
        {
            if( path->HasVertex( vertex ) == false )
            {
                delete vertex;
            }
        }

        for( FOdysseyVectorSegment* segment : mPathAlteredState->topologySegmentList )
        {
            if( path->HasSegment( segment ) == false )
            {
                delete segment;
            }
        }
    }

    if( mPathInitialState )
        delete mPathInitialState;

    if( mPathAlteredState )
        delete mPathAlteredState;
}

FSnapshotPath::FSnapshotPath( FOdysseyVectorPath* iPath
                            , uint64 iSnapshotFlags
                            , eSnapshotState iStateType )
    : FSnapshotObject( iPath, iSnapshotFlags, eSnapshotState::None )
    , mPath( iPath )
    , mPathInitialState ( nullptr )
    , mPathAlteredState ( nullptr )
{
    // Note: even though the ctor for the base class FSnapshotObject calls RecordState, it will not call the overriden
    // method because vtable don't exist at construction time. So we call it here, knowing that
    // FSnapshotObject::Recordstate will receive eSnapshotState::None so we don't do thing twice.
    RecordState( iStateType );
}

void
FSnapshotPath::RecordState( eSnapshotState iStateType )
{
    FSnapshotPath::State* requestedState = nullptr;

    FSnapshotObject::RecordState( iStateType );

    switch( iStateType )
    {
        case eSnapshotState::Initial :
            if( mPathInitialState == nullptr )
                mPathInitialState = new FSnapshotPath::State();

            requestedState = mPathInitialState;
        break;

        case eSnapshotState::Altered :
            if( mPathAlteredState == nullptr )
                mPathAlteredState = new FSnapshotPath::State();

            requestedState = mPathAlteredState;
        break;

        default :
        break;
    }

    if( requestedState )
    {
        if( requestedState->inited == false )
        {
            // snapshot vertex and segment list
            if( mSnapshotFlags & FSnapshotFlags::Object::Path::TOPOLOGY )
            {
                requestedState->topologyVertexList = mPath->GetVertexList();
                requestedState->topologySegmentList = mPath->GetSegmentList();
            }

             // Snapshot vertices position & radius
            if( mSnapshotFlags & FSnapshotFlags::Object::Path::VERTICES )
            {
                std::list<FOdysseyVectorVertex*>& vertexList = mPath->GetVertexList();

                requestedState->vertexSnapshotBuffer.reserve( vertexList.size() );

                for( FOdysseyVectorVertex* vertex : vertexList )
                {
                    requestedState->vertexSnapshotBuffer.emplace_back( vertex
                                                                     , FSnapshotFlags::ALL
                                                                     , iStateType );
                }
            }

             // Snapshot segments' handles position
            if( mSnapshotFlags & FSnapshotFlags::Object::Path::SEGMENTS )
            {
                std::list<FOdysseyVectorSegment*>& segmentList = mPath->GetSegmentList();

                requestedState->cubicSegmentSnapshotBuffer.reserve( segmentList.size() );

                for( FOdysseyVectorSegment* segment : segmentList )
                {
                    if( segment->GetClass() == FOdysseyVectorSegmentCubic::StaticClass() )
                    {
                        FOdysseyVectorSegmentCubic* cubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(segment);

                        requestedState->cubicSegmentSnapshotBuffer.emplace_back( cubicSegment
                                                                               , FSnapshotFlags::ALL
                                                                               , iStateType );
                    }
                }
            }

            if( mSnapshotFlags & FSnapshotFlags::Object::Path::SELECTED_VERTICES )
            {
                requestedState->selectedVertexList = mPath->GetSelectedVertexList();
            }

            if( mSnapshotFlags & FSnapshotFlags::Object::Path::JOINTTYPE )
            {
                requestedState->jointType = mPath->GetJointType();
            }

            if( mSnapshotFlags & FSnapshotFlags::Object::Path::MITERLIMIT )
            {
                requestedState->miterLimit = mPath->GetMiterLimit();
            }

            if( mSnapshotFlags & FSnapshotFlags::Object::Path::BRUSH )
            {
                requestedState->brush = mPath->GetBrush();
            }

            requestedState->inited = true;
        }
    }
}

bool
FSnapshotPath::LoadState( eSnapshotState iStateType )
{
    FSnapshotPath::State* requestedState = nullptr;

    switch( iStateType )
    {
        case eSnapshotState::Initial :
            requestedState = mPathInitialState;
        break;

        case eSnapshotState::Altered :
            requestedState = mPathAlteredState;
        break;

        default :
        break;
    }

    if( FSnapshotObject::LoadState( iStateType ) )
    {
        if( requestedState )
        {
            if( mSnapshotFlags & FSnapshotFlags::Object::Path::TOPOLOGY )
            {
                mPath->RemoveAllSegments();
                mPath->RemoveAllVertices();

                for( FOdysseyVectorVertex* vertex : requestedState->topologyVertexList )
                {
                    mPath->AddVertex( vertex );
                }

                for( FOdysseyVectorSegment* segment : requestedState->topologySegmentList )
                {
                    mPath->AddSegment( segment );
                }
            }

            if( mSnapshotFlags & FSnapshotFlags::Object::Path::VERTICES )
            {
                for( int i = 0; i < requestedState->vertexSnapshotBuffer.size(); i++ )
                {
                    requestedState->vertexSnapshotBuffer[i].LoadState( iStateType );
                }
            }

            if( mSnapshotFlags & FSnapshotFlags::Object::Path::SEGMENTS )
            {
                for( int i = 0; i < requestedState->cubicSegmentSnapshotBuffer.size(); i++ )
                {
                    requestedState->cubicSegmentSnapshotBuffer[i].LoadState( iStateType );
                }
            }

            if( mSnapshotFlags & FSnapshotFlags::Object::Path::SELECTED_VERTICES )
            {
                mPath->UnselectAllVertices();

                for( FOdysseyVectorVertex* vertex : requestedState->selectedVertexList )
                {
                    mPath->SelectVertex( vertex );
                }
            }

            if( mSnapshotFlags & FSnapshotFlags::Object::Path::JOINTTYPE )
            {
                mPath->SetJointType( requestedState->jointType, true );
            }

            if( mSnapshotFlags & FSnapshotFlags::Object::Path::MITERLIMIT )
            {
                mPath->SetMiterLimit( requestedState->miterLimit, true );
            }

            if( mSnapshotFlags & FSnapshotFlags::Object::Path::BRUSH )
            {
                mPath->SetBrush( requestedState->brush );
            }

            return true;
        }
    }

    return false;
}

FSnapshotGroup::~FSnapshotGroup()
{
    if( mGroupInitialState )
        delete mGroupInitialState;

    if( mGroupAlteredState )
        delete mGroupAlteredState;
}

FSnapshotGroup::FSnapshotGroup( FOdysseyVectorGroup* iGroup
                              , uint64 iSnapshotFlags
                              , eSnapshotState iStateType )
    : FSnapshotObject( iGroup, iSnapshotFlags, eSnapshotState::None )
    , mGroup ( iGroup )
    , mGroupInitialState( nullptr )
    , mGroupAlteredState( nullptr )
{
    // Note: even though the ctor for the base class FSnapshotObject calls RecordState, it will not call the overriden
    // method because vtable don't exist at construction time. So we call it here, knowing that
    // FSnapshotObject::Recordstate will receive eSnapshotState::None so we don't do thing twice.
    RecordState( iStateType );
}

void
FSnapshotGroup::RecordState( eSnapshotState iStateType )
{
    FSnapshotGroup::State *requestedState = nullptr;

    FSnapshotObject::RecordState( iStateType );

    switch( iStateType )
    {
        case eSnapshotState::Initial :
            if( mGroupInitialState == nullptr )
                mGroupInitialState = new FSnapshotGroup::State();

            requestedState = mGroupInitialState;
        break;

        case eSnapshotState::Altered :
            if( mGroupAlteredState == nullptr )
                mGroupAlteredState = new FSnapshotGroup::State();

            requestedState = mGroupAlteredState;
        break;

        default :
        break;
    }

    if( requestedState )
    {
        if( requestedState->inited == false )
        {

            if( mSnapshotFlags & FSnapshotFlags::Object::Group::HUDCOLOR )
            {
                requestedState->HUDColor = mGroup->GetHUDColor();
            }

            requestedState->inited = true;
        }
    }
}

bool
FSnapshotGroup::LoadState( eSnapshotState iStateType )
{
    FSnapshotGroup::State *requestedState = nullptr;

    switch( iStateType )
    {
        case eSnapshotState::Initial :
            requestedState = mGroupInitialState;
        break;

        case eSnapshotState::Altered :
            requestedState = mGroupAlteredState;
        break;

        default :
        break;
    }

    if( FSnapshotObject::LoadState( iStateType ) )
    {
        if( requestedState )
        {
            if( mSnapshotFlags & FSnapshotFlags::Object::Group::HUDCOLOR )
            {
                mGroup->SetHUDColor( requestedState->HUDColor );
            }

            return true;
        }
    }

    return false;
}

FSnapshotGroupPaint::~FSnapshotGroupPaint()
{
    if( mPaintgroupInitialState )
        delete mPaintgroupInitialState;

    if( mPaintgroupAlteredState )
        delete mPaintgroupAlteredState;
}

FSnapshotGroupPaint::FSnapshotGroupPaint( FOdysseyVectorGroupPaint* iPaintGroup
                                        , uint64 iSnapshotFlags
                                        , eSnapshotState iStateType )
    : FSnapshotGroup( iPaintGroup, iSnapshotFlags, eSnapshotState::None )
    , mPaintgroup ( iPaintGroup )
    , mPaintgroupInitialState( nullptr )
    , mPaintgroupAlteredState( nullptr )
{
    // Note: even though the ctor for the base class FSnapshotGroup calls RecordState, it will not call the overriden
    // method because vtable don't exist at construction time. So we call it here, knowing that
    // FSnapshotGroup::Recordstate will receive eSnapshotState::None so we don't do thing twice.
    RecordState( iStateType );
}

void
FSnapshotGroupPaint::RecordState( eSnapshotState iStateType )
{
    FSnapshotGroupPaint::State *requestedState = nullptr;

    FSnapshotGroup::RecordState( iStateType );

    switch( iStateType )
    {
        case eSnapshotState::Initial :
            if( mPaintgroupInitialState == nullptr )
                mPaintgroupInitialState = new FSnapshotGroupPaint::State();

            requestedState = mPaintgroupInitialState;
        break;

        case eSnapshotState::Altered :
            if( mPaintgroupAlteredState == nullptr )
                mPaintgroupAlteredState = new FSnapshotGroupPaint::State();

            requestedState = mPaintgroupAlteredState;
        break;

        default :
        break;
    }

    if( requestedState )
    {
        if( requestedState->inited == false )
        {
            if( mSnapshotFlags & FSnapshotFlags::Object::Group::Paint::BUCKETS )
            {
                std::list<FOdysseyVectorBucket*>& bucketList = mPaintgroup->GetBucketList();

                requestedState->bucketSnapshotBuffer.reserve( bucketList.size() );

                for( FOdysseyVectorBucket* bucket : bucketList )
                {
                    requestedState->bucketSnapshotBuffer.emplace_back( bucket
                                                                     , FSnapshotFlags::ALL
                                                                     , iStateType );
                }
            }

            if( mSnapshotFlags & FSnapshotFlags::Object::Group::Paint::SELECTED_BUCKETS )
            {
                requestedState->selectedBucketList = mPaintgroup->GetSelectedBucketList();
            }

            if( mSnapshotFlags & FSnapshotFlags::Object::Group::Paint::PAINTED )
            {
                requestedState->painted = mPaintgroup->IsPainted();
            }

            if( mSnapshotFlags & FSnapshotFlags::Object::Group::Paint::INTERSECTSCANVAS )
            {
                requestedState->intersectsCanvas = mPaintgroup->IntersectsCanvas();
            }

            if( mSnapshotFlags & FSnapshotFlags::Object::Group::Paint::MONOCHROME )
            {
                requestedState->monochrome = mPaintgroup->IsMonochrome();
            }

            if( mSnapshotFlags & FSnapshotFlags::Object::Group::Paint::MONOCHROMECOLOR )
            {
                requestedState->monochromeColor = mPaintgroup->GetMonochromeColor();
            }
            if( mSnapshotFlags & FSnapshotFlags::Object::Group::Paint::REALTIME )
            {
                requestedState->realtime = mPaintgroup->IsRealtime();
            }

            if( mSnapshotFlags & FSnapshotFlags::Object::Group::Paint::GAPTOLERANCE )
            {
                requestedState->gapTolerance = mPaintgroup->GetGapTolerance();
            }

            if( mSnapshotFlags & FSnapshotFlags::Object::Group::Paint::SEGMENTEXTENSIONSCHEME )
            {
                requestedState->segmentExtensionScheme = mPaintgroup->GetSegmentExtensionScheme();
            }

            if( mSnapshotFlags & FSnapshotFlags::Object::Group::Paint::GAPDETECTIONSCHEME )
            {
                requestedState->gapDetectionScheme = mPaintgroup->GetGapDetectionScheme();
            }

            if( mSnapshotFlags & FSnapshotFlags::Object::Group::Paint::SEGMENTEXTENSIONSIMPLIFIED )
            {
                requestedState->segmentExtensionSimplified = mPaintgroup->IsSegmentExtensionSimplified();
            }

            if( mSnapshotFlags & FSnapshotFlags::Object::Group::Paint::WIREFRAME )
            {
                requestedState->wireframe = mPaintgroup->IsWireframe();
            }

            if( mSnapshotFlags & FSnapshotFlags::Object::Group::Paint::WIREFRAMECOLOR )
            {
                requestedState->wireframeColor = mPaintgroup->GetWireframeColor();
            }

            requestedState->inited = true;
        }
    }
}

bool
FSnapshotGroupPaint::LoadState( eSnapshotState iStateType )
{
    FSnapshotGroupPaint::State *requestedState = nullptr;

    switch( iStateType )
    {
        case eSnapshotState::Initial :
            requestedState = mPaintgroupInitialState;
        break;

        case eSnapshotState::Altered :
            requestedState = mPaintgroupAlteredState;
        break;

        default :
        break;
    }

    if( requestedState )
    {
        if( FSnapshotGroup::LoadState( iStateType ) )
        {
            if( mSnapshotFlags & FSnapshotFlags::Object::Group::Paint::BUCKETS )
            {
                for( FSnapshotBucket& bucketSnapshot : requestedState->bucketSnapshotBuffer )
                {
                    bucketSnapshot.LoadState( iStateType );
                }
            }

            if( mSnapshotFlags & FSnapshotFlags::Object::Group::Paint::SELECTED_BUCKETS )
            {
                mPaintgroup->UnselectAllBuckets();

                for( FOdysseyVectorBucket* bucket : requestedState->selectedBucketList )
                {
                    mPaintgroup->SelectBucket( bucket );
                }
            }

            if( mSnapshotFlags & FSnapshotFlags::Object::Group::Paint::PAINTED )
            {
                mPaintgroup->SetPainted( requestedState->painted );
            }

            if( mSnapshotFlags & FSnapshotFlags::Object::Group::Paint::INTERSECTSCANVAS )
            {
                mPaintgroup->SetIntersectsCanvas( requestedState->intersectsCanvas );
            }

            if( mSnapshotFlags & FSnapshotFlags::Object::Group::Paint::MONOCHROME )
            {
                mPaintgroup->SetMonochrome( requestedState->monochrome );
            }

            if( mSnapshotFlags & FSnapshotFlags::Object::Group::Paint::MONOCHROMECOLOR )
            {
                mPaintgroup->SetMonochromeColor( requestedState->monochromeColor );
            }
            if( mSnapshotFlags & FSnapshotFlags::Object::Group::Paint::REALTIME )
            {
                mPaintgroup->SetRealtime( requestedState->realtime );
            }

            if( mSnapshotFlags & FSnapshotFlags::Object::Group::Paint::GAPTOLERANCE )
            {
                mPaintgroup->SetGapTolerance( requestedState->gapTolerance );
            }

            if( mSnapshotFlags & FSnapshotFlags::Object::Group::Paint::SEGMENTEXTENSIONSCHEME )
            {
                mPaintgroup->SetSegmentExtensionScheme( requestedState->segmentExtensionScheme );
            }

            if( mSnapshotFlags & FSnapshotFlags::Object::Group::Paint::GAPDETECTIONSCHEME )
            {
                mPaintgroup->SetGapDetectionScheme( requestedState->gapDetectionScheme );
            }

            if( mSnapshotFlags & FSnapshotFlags::Object::Group::Paint::SEGMENTEXTENSIONSIMPLIFIED )
            {
                mPaintgroup->SetSegmentExtensionSimplified( requestedState->segmentExtensionSimplified );
            }

            if( mSnapshotFlags & FSnapshotFlags::Object::Group::Paint::WIREFRAME )
            {
                mPaintgroup->SetWireframe( requestedState->wireframe );
            }

            if( mSnapshotFlags & FSnapshotFlags::Object::Group::Paint::WIREFRAMECOLOR )
            {
                mPaintgroup->SetWireframeColor( requestedState->wireframeColor );
            }

            return true;
        }
    }

    return false;
}

FSnapshotCell::~FSnapshotCell()
{
    if( mInitialState )
        delete mInitialState;

    if( mAlteredState )
        delete mAlteredState;
}

FSnapshotCell::FSnapshotCell( FOdysseyVectorCell* iCell
                            , uint64 iSnapshotFlags
                            , eSnapshotState iStateType )
    : FSnapshotObject ( iCell, iSnapshotFlags, eSnapshotState::None )
    , mSnapshotFlags( iSnapshotFlags )
    , mInitialState ( nullptr )
    , mAlteredState ( nullptr )
{
    // Note: even though the ctor for the base class FSnapshotObject calls RecordState, it will not call the overriden
    // method because vtable don't exist at construction time. So we call it here, knowing that
    // FSnapshotObject::Recordstate will receive eSnapshotState::None so we don't do thing twice.
    RecordState( iStateType );
}

FOdysseyVectorCell*
FSnapshotCell::GetCell()
{
    return static_cast<FOdysseyVectorCell*>(mObject);
}

void
FSnapshotCell::RecordState( eSnapshotState iStateType )
{
    FOdysseyVectorCell* cell = static_cast<FOdysseyVectorCell*>(mObject);
    FSnapshotCell::State *requestedState = nullptr;

    FSnapshotObject::RecordState( iStateType );

    switch( iStateType )
    {
        case eSnapshotState::Initial :
            if( mInitialState == nullptr )
                mInitialState = new FSnapshotCell::State();

            requestedState = mInitialState;
        break;

        case eSnapshotState::Altered :
            if( mAlteredState == nullptr )
                mAlteredState = new FSnapshotCell::State();

            requestedState = mAlteredState;
        break;

        default :
        break;
    }

    if( requestedState )
    {
        if( requestedState->inited == false )
        {
            requestedState->selectedObjectArray.reserve( cell->GetSelectedObjectList().size() );

            for( FOdysseyVectorObject* selectedObject : cell->GetSelectedObjectList() )
            {
                requestedState->selectedObjectArray.push_back( selectedObject );
            }

            requestedState->inited = true;
        }
    }
}

bool
FSnapshotCell::LoadState( eSnapshotState iStateType )
{
    FOdysseyVectorCell* cell = static_cast<FOdysseyVectorCell*>(mObject);
    FSnapshotCell::State *requestedState = nullptr;

    FSnapshotObject::LoadState( iStateType );

    switch( iStateType )
    {
        case eSnapshotState::Initial :
            requestedState = mInitialState;
        break;

        case eSnapshotState::Altered :
            requestedState = mAlteredState;
        break;

        default :
        break;
    }

    if( requestedState )
    {
        cell->ClearObjectSelection();

        for( FOdysseyVectorObject* selectedObject : requestedState->selectedObjectArray )
        {
            cell->SelectObject( selectedObject );
        }

        return true; // restore succeeded
    }

    return false;
}
