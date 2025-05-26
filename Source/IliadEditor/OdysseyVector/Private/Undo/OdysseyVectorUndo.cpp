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

            FOdysseyVectorEngine::Notify( nullptr, mReturnFlags );

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
}

FSnapshotTrajectory::FSnapshotTrajectory( FInbetweenerTrajectory* iTrajectory
                                        , uint64 iSnapshotFlags
                                        , eSnapshotState iState )
    : mSnapshotFlags( iSnapshotFlags )
    , mRoute( iTrajectory->GetRoute() )
    , mIndex( iTrajectory - &iTrajectory->GetRoute()->GetTrajectoryBuffer()[0] )
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
            requestedState = &mInitialState;
        break;

        case eSnapshotState::Altered :
            requestedState = &mAlteredState;
        break;

        default :
        break;
    }

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

bool
FSnapshotTrajectory::LoadState( eSnapshotState iStateType )
{
    FInbetweenerTrajectory* trajectory = &mRoute->GetTrajectoryBuffer()[mIndex];
    FSnapshotTrajectory::State* requestedState = nullptr;

    switch( iStateType )
    {
        case eSnapshotState::Initial :
            requestedState = &mInitialState;
        break;

        case eSnapshotState::Altered :
            requestedState = &mAlteredState;
        break;

        default :
        break;
    }

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

FSnapshotStep::~FSnapshotStep()
{
}

FSnapshotStep::FSnapshotStep( FInbetweenerStep* iStep, eSnapshotState iState )
    : mRoute( iStep->GetRoute() )
    , mIndex( iStep - &iStep->GetRoute()->GetStepBuffer()[0] )
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
            requestedState = &mInitialState;
        break;

        case eSnapshotState::Altered :
            requestedState = &mAlteredState;
        break;

        default :
        break;
    }

    if( requestedState->inited == false )
    {
        requestedState->aligned = mRoute->GetStepBuffer()[mIndex].IsAligned();

        requestedState->inited = true;
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
            requestedState = &mInitialState;
        break;

        case eSnapshotState::Altered :
            requestedState = &mAlteredState;
        break;

        default :
        break;
    }

    step->SetAligned( requestedState->aligned );

    return true;
}

FSnapshotRoute::~FSnapshotRoute()
{
}

FSnapshotRoute::FSnapshotRoute( FInbetweenerRoute* iRoute
                              , uint64 iSnapshotflags
                              , uint64 iTrajectorySnapshotFlags
                              , eSnapshotState iStateType )
    : mRoute( iRoute )
    , mSnapshotFlags( iSnapshotflags ) // unused
    , mTrajectorySnapshotFlags( iTrajectorySnapshotFlags )
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
            requestedState = &mInitialState;
        break;

        case eSnapshotState::Altered :
            requestedState = &mAlteredState;
        break;

        default :
        break;
    }

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
            requestedState->stepSnapshotBuffer.reserve( mRoute->GetInbetweenerTag()->GetBreakdownCount() );

            for( FInbetweenerStep& step : mRoute->GetStepBuffer() )
            {
                requestedState->stepSnapshotBuffer.emplace_back( &step, iStateType );
            }
        }

        requestedState->inited = true;
    }
}

bool
FSnapshotRoute::LoadState( eSnapshotState iStateType )
{
    FSnapshotRoute::State* requestedState = nullptr;

    switch( iStateType )
    {
        case eSnapshotState::Initial :
            requestedState = &mInitialState;
        break;

        case eSnapshotState::Altered :
            requestedState = &mAlteredState;
        break;

        default :
        break;
    }

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
}

FSnapshotLayout::FSnapshotLayout( FOdysseyVectorTagInbetweener* iInbetweenerTag
                                , eSnapshotState iStateType )
    : mInbetweenerTag ( iInbetweenerTag )
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
            requestedState = &mInitialState;
        break;

        case eSnapshotState::Altered :
            requestedState = &mAlteredState;
        break;

        default :
        break;
    }

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

bool
FSnapshotLayout::LoadState( eSnapshotState iStateType )
{
    FSnapshotLayout::State* requestedState = nullptr;

    switch( iStateType )
    {
        case eSnapshotState::Initial :
            requestedState = &mInitialState;
        break;

        case eSnapshotState::Altered :
            requestedState = &mAlteredState;
        break;

        default :
        break;
    }

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

FSnapshotDynamics::~FSnapshotDynamics()
{
    for( FInbetweenerRoute* route : mInitialState.routeArray )
    {
        if( route->GetInbetweenerTag() == nullptr )
        {
            delete route;
        }
    }

    for( FInbetweenerRoute* route : mAlteredState.routeArray )
    {
        if( route->GetInbetweenerTag() == nullptr )
        {
            delete route;
        }
    }
}

FSnapshotDynamics::FSnapshotDynamics()
{
}

FSnapshotDynamics::FSnapshotDynamics( FOdysseyVectorTagInbetweener* iInbetweenerTag
                                    , eSnapshotState iStateType )
    : mInbetweenerTag( iInbetweenerTag )
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
            requestedState = &mInitialState;
        break;

        case eSnapshotState::Altered :
            requestedState = &mAlteredState;
        break;

        default :
        break;
    }

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

bool
FSnapshotDynamics::LoadState( eSnapshotState iStateType )
{
    FSnapshotDynamics::State* requestedState = nullptr;

    switch( iStateType )
    {
        case eSnapshotState::Initial :
            requestedState = &mInitialState;
        break;

        case eSnapshotState::Altered :
            requestedState = &mAlteredState;
        break;

        default :
        break;
    }

    mInbetweenerTag->RemoveAllRoutes();

    for( FInbetweenerRoute* route : requestedState->routeArray )
    {
        mInbetweenerTag->AddRoute( route );
    }

    return true;
}

FSnapshotInbetweenerChart::~FSnapshotInbetweenerChart()
{
}

FSnapshotInbetweenerChart::FSnapshotInbetweenerChart( FInbetweenerChart* iChart
                                                    , uint64 iSnapshotFlags
                                                    , eSnapshotState iStateType )
    : mChart( iChart )
    , mSnapshotFlags( iSnapshotFlags )
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
            requestedState = &mInitialState;
        break;

        case eSnapshotState::Altered :
            requestedState = &mAlteredState;
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
            requestedState = &mInitialState;
        break;

        case eSnapshotState::Altered :
            requestedState = &mAlteredState;
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
}

FSnapshotInbetweenerBreakdown::FSnapshotInbetweenerBreakdown( FInbetweenerBreakdown* iBreakdown
                                                            , uint64 iSnapshotFlags
                                                            , eSnapshotState iState )
    : mBreakdown( iBreakdown )
    , mSnapshotFlags( iSnapshotFlags )
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
            requestedState = &mInitialState;
        break;

        case eSnapshotState::Altered :
            requestedState = &mAlteredState;
        break;

        default :
        break;
    }

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

bool
FSnapshotInbetweenerBreakdown::LoadState( eSnapshotState iStateType )
{
    FSnapshotInbetweenerBreakdown::State* requestedState = nullptr;

    switch( iStateType )
    {
        case eSnapshotState::Initial :
            requestedState = &mInitialState;
        break;

        case eSnapshotState::Altered :
            requestedState = &mAlteredState;
        break;

        default :
        break;
    }

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

FSnapshotTagInbetweener::~FSnapshotTagInbetweener()
{
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
            requestedState = &mInitialState;
        break;

        case eSnapshotState::Altered :
            requestedState = &mAlteredState;
        break;

        default :
        break;
    }

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

bool
FSnapshotTagInbetweener::LoadState( eSnapshotState iStateType )
{
    FSnapshotTagInbetweener::State *requestedState = nullptr;

    switch( iStateType )
    {
        case eSnapshotState::Initial :
            requestedState = &mInitialState;
        break;

        case eSnapshotState::Altered :
            requestedState = &mAlteredState;
        break;

        default :
        break;
    }

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

FSnapshotObject::~FSnapshotObject()
{
    for( int i = 0; i < mChildrenSnapshotArray.size(); i++ )
    {
        delete mChildrenSnapshotArray[i];
    }
}

FSnapshotObject::FSnapshotObject( FOdysseyVectorObject* iObject, uint64 iSnapshotFlags )
    : mSnapshotFlags( iSnapshotFlags )
    , mObject( iObject )
    , mPreviousChild( nullptr )
    , mForegroundBucketSnapshot( &iObject->GetForegroundBucket(), 0 )
    , mBackgroundBucketSnapshot( &iObject->GetBackgroundBucket(), 0 )
{
    if( mSnapshotFlags & FSnapshotFlags::Object::TRANSFORMATIONS )
    {
        iObject->GetTransform( mTranslationX
                             , mTranslationY
                             , mRotation
                             , mScalingX
                             , mScalingY );
    }


    if( mSnapshotFlags & FSnapshotFlags::Object::HIERARCHY )
    {
        mParent = iObject->GetParent();
        mPreviousChild = iObject->GetParent()->GetPreviousChild( iObject );
    }

    if( mSnapshotFlags & FSnapshotFlags::Object::CHILDREN_TRANSFORMATIONS )
    {
        std::list<FOdysseyVectorObject*>& childrenObjectList = iObject->GetChildrenList();

        mChildrenSnapshotArray.reserve( childrenObjectList.size() );

        for( FOdysseyVectorObject* child : childrenObjectList )
        {
            mChildrenSnapshotArray.push_back( new FSnapshotObject( child, FSnapshotFlags::Object::TRANSFORMATIONS ) );
        }
    }

    if( mSnapshotFlags & FSnapshotFlags::Object::COLORING )
    {
        FOdysseyVectorBucket& foregroundBucket = iObject->GetForegroundBucket();
        FOdysseyVectorBucket& backgroundBucket = iObject->GetBackgroundBucket();

        mForegroundBucketSnapshot = FSnapshotBucket( &foregroundBucket, FSnapshotFlags::Point::Bucket::PARAM );
        mBackgroundBucketSnapshot = FSnapshotBucket( &backgroundBucket, FSnapshotFlags::Point::Bucket::PARAM );
    }

    if( mSnapshotFlags & FSnapshotFlags::Object::OPACITY )
    {
        mOpacity = iObject->GetOpacity();
    }

    if( mSnapshotFlags & FSnapshotFlags::Object::OPACITY )
    {
        bVisibility = iObject->IsVisible( false );
    }

    if( mSnapshotFlags & FSnapshotFlags::Object::NAME )
    {
        mName = iObject->GetName();
    }
}

bool
FSnapshotObject::Restore()
{
    if( mSnapshotFlags & FSnapshotFlags::Object::TRANSFORMATIONS )
    {
        double swapTranslationX, swapTranslationY, swapRotation, swapScalingX, swapScalingY;

        mObject->GetTransform( swapTranslationX
                             , swapTranslationY
                             , swapRotation
                             , swapScalingX
                             , swapScalingY );

        mObject->SetTransform( mTranslationX
                             , mTranslationY
                             , mRotation
                             , mScalingX
                             , mScalingY );

        mObject->UpdateMatrix();

        mTranslationX = swapTranslationX;
        mTranslationY = swapTranslationY;
        mRotation     = swapRotation;
        mScalingX     = swapScalingX;
        mScalingY     = swapScalingY;
    }

    if( mSnapshotFlags & FSnapshotFlags::Object::HIERARCHY )
    {
        FOdysseyVectorObject* currentParent = mObject->GetParent();
        FOdysseyVectorObject* currentPreviousChild = mObject->GetParent()->GetPreviousChild(mObject);
        uint32 transferRetval = mParent->TransferChild( mObject, mPreviousChild );

        if( transferRetval == FOdysseyVectorObject::HIERARCHY_CHANGE_SUCCESS )
        {
            // swap
            mParent = currentParent;
            mPreviousChild = currentPreviousChild;
        }

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

    if( mSnapshotFlags & FSnapshotFlags::Object::CHILDREN_TRANSFORMATIONS )
    {
        for( int i = 0; i < mChildrenSnapshotArray.size(); i++ )
        {
            mChildrenSnapshotArray[i]->Restore();
        }
    }

    if( mSnapshotFlags & FSnapshotFlags::Object::COLORING )
    {
        mForegroundBucketSnapshot.Restore();
        mBackgroundBucketSnapshot.Restore();
    }

    if( mSnapshotFlags & FSnapshotFlags::Object::OPACITY )
    {
        double currentOpacity = mObject->GetOpacity();

        mObject->SetOpacity( mOpacity );
        // swap
        mOpacity = currentOpacity;
    }

    if( mSnapshotFlags & FSnapshotFlags::Object::VISIBILITY )
    {
        bool currentVisibility = mObject->IsVisible( false );

        mObject->SetVisible( bVisibility );
        // swap
        bVisibility = currentVisibility;
    }

    if( mSnapshotFlags & FSnapshotFlags::Object::NAME )
    {
        FString currentName = mObject->GetName();

        mObject->SetName( mName );
        // swap
        mName = currentName;
    }

    return true; // restore succeeded
}

FSnapshotPoint::~FSnapshotPoint()
{
}

FSnapshotPoint::FSnapshotPoint( FOdysseyVectorPoint* iPoint, uint64 iSnapshotFlags )
    : mSnapshotFlags( iSnapshotFlags )
    , mPoint( iPoint )
{
    if( iSnapshotFlags & FSnapshotFlags::Point::POSITION )
    {
        mCoords = iPoint->GetCoords();
    }
}

void
FSnapshotPoint::Restore()
{
    if( mSnapshotFlags & FSnapshotFlags::Point::POSITION )
    {
        ::ULIS::FVec2D swapCoords = mPoint->GetCoords();

        if( mPoint->GetClass() == FOdysseyVectorHandleSegment::StaticClass() )
        {
            FOdysseyVectorHandleSegment* handle = static_cast<FOdysseyVectorHandleSegment*>( mPoint );
            bool alignStatus = handle->GetAttachedVertex()->IsHandleAligned();

            if( alignStatus )
            {
                handle->GetAttachedVertex()->SetHandleAligned( false );
            }

            mPoint->Set( mCoords.x, mCoords.y );

            if( alignStatus )
            {
                // second parameter is false to prevent immediate alignment
                handle->GetAttachedVertex()->SetHandleAligned( true, false );
            }
        }
        else
        {
            mPoint->Set( mCoords.x, mCoords.y );
        }

        mCoords = swapCoords;
    }
}

FSnapshotVertex::~FSnapshotVertex()
{
}

FSnapshotVertex::FSnapshotVertex( FOdysseyVectorVertex* iVertex, uint64 iSnapshotFlags )
    : FSnapshotPoint( iVertex, iSnapshotFlags )
{
    if( iSnapshotFlags & FSnapshotFlags::Point::Vertex::ALIGNMENT )
    {
        mAlignment = iVertex->IsHandleAligned();
    }

    if( iSnapshotFlags & FSnapshotFlags::Point::Vertex::LOCK )
    {
        mLocked = iVertex->IsLocked();
    }

    if( iSnapshotFlags & FSnapshotFlags::Point::Vertex::RADIUS )
    {
        mRadius = iVertex->GetRadius();
    }
}

FOdysseyVectorVertex*
FSnapshotVertex::GetVertex()
{
    return static_cast<FOdysseyVectorVertex*>(mPoint);
}

void
FSnapshotVertex::Restore()
{
    FOdysseyVectorVertex* vertex = static_cast<FOdysseyVectorVertex*>(mPoint);

    FSnapshotPoint::Restore();

    if( mSnapshotFlags & FSnapshotFlags::Point::Vertex::ALIGNMENT )
    {
        bool alignment = vertex->IsHandleAligned();

        vertex->SetHandleAligned( mAlignment );

        // swap for redo
        mAlignment = alignment;
    }

    if( mSnapshotFlags & FSnapshotFlags::Point::Vertex::LOCK )
    {
        bool locked = vertex->IsLocked();

        vertex->SetLocked( mLocked );

        // swap for redo
        mLocked = locked;
    }

    if( mSnapshotFlags & FSnapshotFlags::Point::Vertex::RADIUS )
    {
        double swapRadius = vertex->GetRadius();

        vertex->SetRadius( mRadius );

        mRadius = swapRadius;
    }
}

FSnapshotBucket::~FSnapshotBucket()
{
}

FSnapshotBucket::FSnapshotBucket( FOdysseyVectorBucket* iBucket, uint64 iSnapshotFlags )
    : FSnapshotPoint( iBucket, iSnapshotFlags )
{
    if( iSnapshotFlags & FSnapshotFlags::Point::Bucket::COLORMODE )
    {
        mColorMode = iBucket->GetColorMode();
    }

    if( iSnapshotFlags & FSnapshotFlags::Point::Bucket::SOLIDCOLOR )
    {
        mSolidColor = iBucket->GetSolidColor();
    }

    if( iSnapshotFlags & FSnapshotFlags::Point::Bucket::ROTATION )
    {
        mRotation = iBucket->GetRotation();
    }

    if( iSnapshotFlags & FSnapshotFlags::Point::Bucket::PROPAGATION )
    {
        mPropagated = iBucket->IsPropagated();
    }

    if( iSnapshotFlags & FSnapshotFlags::Point::Bucket::GRADIENTCOLOR0 )
    {
        mGradientColor0 = iBucket->GetGradientColor0();
    }

    if( iSnapshotFlags & FSnapshotFlags::Point::Bucket::GRADIENTCOLOR1 )
    {
        mGradientColor1 = iBucket->GetGradientColor1();
    }

    if( iSnapshotFlags & FSnapshotFlags::Point::Bucket::RADIALRADIUS )
    {
        mRadialRadius = iBucket->GetRadialRadius();
    }

    if( iSnapshotFlags & FSnapshotFlags::Point::Bucket::RADIALOFFSET )
    {
        mRadialOffset = iBucket->GetRadialOffset();
    }

    if( iSnapshotFlags & FSnapshotFlags::Point::Bucket::LINEARP0 )
    {
        mLinearP0 = iBucket->GetLinearP0();
    }

    if( iSnapshotFlags & FSnapshotFlags::Point::Bucket::LINEARP1 )
    {
        mLinearP1 = iBucket->GetLinearP1();
    }

    if( iSnapshotFlags & FSnapshotFlags::Point::Bucket::PALETTEENTRY )
    {
        mPaletteEntry = iBucket->GetPaletteEntry();
    }

    if( iSnapshotFlags & FSnapshotFlags::Point::Bucket::PALETTESET )
    {
        mPaletteSet = iBucket->GetPaletteSet();
    }
}

FOdysseyVectorBucket*
FSnapshotBucket::GetBucket()
{
    return static_cast<FOdysseyVectorBucket*>(mPoint);
}

void
FSnapshotBucket::Restore()
{
    FOdysseyVectorBucket* bucket = static_cast<FOdysseyVectorBucket*>(mPoint);

    FSnapshotPoint::Restore();

    if( mSnapshotFlags & FSnapshotFlags::Point::Bucket::COLORMODE )
    {
        eBucketColorMode currentColorMode = bucket->GetColorMode();

        bucket->SetColorMode( mColorMode );
        // swap
        mColorMode = currentColorMode;
    }

    if( mSnapshotFlags & FSnapshotFlags::Point::Bucket::SOLIDCOLOR )
    {
        FColor currentSolidColor = bucket->GetSolidColor();

        bucket->SetSolidColor( mSolidColor );

        mSolidColor = currentSolidColor;
    }

    if( mSnapshotFlags & FSnapshotFlags::Point::Bucket::LINEARP0 )
    {
        ::ULIS::FVec2D currentLinearP0 = bucket->GetLinearP0();

        bucket->SetLinearP0( mLinearP0 );
        // swap
        mLinearP0 = currentLinearP0;
    }

    if( mSnapshotFlags & FSnapshotFlags::Point::Bucket::LINEARP1 )
    {
        ::ULIS::FVec2D currentLinearP1 = bucket->GetLinearP1();

        bucket->SetLinearP1( mLinearP1 );
        // swap
        mLinearP1 = currentLinearP1;
    }

    if( mSnapshotFlags & FSnapshotFlags::Point::Bucket::PROPAGATION )
    {
        bool currentPropagated = bucket->IsPropagated();

        bucket->SetPropagated( mPropagated );
        // swap
        mPropagated = currentPropagated;
    }

    if( mSnapshotFlags & FSnapshotFlags::Point::Bucket::GRADIENTCOLOR0 )
    {
        FColor currrentGradientColor0 = bucket->GetGradientColor0();

        bucket->SetGradientColor0( mGradientColor0 );
        // swap
        mGradientColor0 = currrentGradientColor0;
    }

    if( mSnapshotFlags & FSnapshotFlags::Point::Bucket::GRADIENTCOLOR1 )
    {
        FColor currrentGradientColor1 = bucket->GetGradientColor1();

        bucket->SetGradientColor1( mGradientColor1 );
        // swap
        mGradientColor1 = currrentGradientColor1;
    }

    if( mSnapshotFlags & FSnapshotFlags::Point::Bucket::RADIALRADIUS )
    {
        double currentRadialRadius = bucket->GetRadialRadius();

        bucket->SetRadialRadius( mRadialRadius );
        // swap
        mRadialRadius = currentRadialRadius;
    }

    if( mSnapshotFlags & FSnapshotFlags::Point::Bucket::RADIALOFFSET )
    {
        ::ULIS::FVec2D currentRadialOffset = bucket->GetRadialOffset();

        bucket->SetRadialOffset( mRadialOffset );
        // swap
        mRadialOffset = currentRadialOffset;
    }

    if( mSnapshotFlags & FSnapshotFlags::Point::Bucket::PALETTEENTRY )
    {
        UOdysseyPaletteEntry* currentPaletteEntry = bucket->GetPaletteEntry();

        bucket->SetPaletteEntry( mPaletteEntry );
        // swap
        mPaletteEntry = currentPaletteEntry;
    }

    if( mSnapshotFlags & FSnapshotFlags::Point::Bucket::PALETTESET )
    {
        int currentPaletteSet = bucket->GetPaletteSet();

        bucket->SetPaletteSet( mPaletteSet );
        // swap
        mPaletteSet = currentPaletteSet;
    }
}

FSnapshotSegmentCubic::~FSnapshotSegmentCubic()
{
}

FSnapshotSegmentCubic::FSnapshotSegmentCubic( FOdysseyVectorSegmentCubic *iCubicSegment
                                            , uint64 iSnapshotFlags )
    : mSnapshotFlags( iSnapshotFlags )
    , mCubicSegment( iCubicSegment )
{
    if( iSnapshotFlags & FSnapshotFlags::Segment::Cubic::HANDLES )
    {
        mHandleCoords[0] = iCubicSegment->GetHandle(0)->GetCoords();
        mHandleCoords[1] = iCubicSegment->GetHandle(1)->GetCoords();
    }
}

FOdysseyVectorSegmentCubic*
FSnapshotSegmentCubic::GetCubicSegment()
{
    return mCubicSegment;
}

void
FSnapshotSegmentCubic::Restore()
{
    if( mSnapshotFlags & FSnapshotFlags::Segment::Cubic::HANDLES )
    {
        ::ULIS::FVec2D swapCoords[2] = { mCubicSegment->GetHandle(0)->GetCoords()
                                       , mCubicSegment->GetHandle(1)->GetCoords() };

        mCubicSegment->GetHandle(0)->Set( mHandleCoords[0].x, mHandleCoords[0].y );
        mCubicSegment->GetHandle(1)->Set( mHandleCoords[1].x, mHandleCoords[1].y );

        mHandleCoords[0] = swapCoords[0];
        mHandleCoords[1] = swapCoords[1];
    }
}

FSnapshotPath::~FSnapshotPath()
{
    FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(mObject);

    // free removed vertices and segments
    if( mSnapshotFlags & FSnapshotFlags::Object::Path::TOPOLOGY )
    {
        for( FOdysseyVectorVertex* vertex : mTopologyVertexList )
        {
            if( path->HasVertex( vertex ) == false )
            {
                delete vertex;
            }
        }

        for( FOdysseyVectorSegment* segment : mTopologySegmentList )
        {
            if( path->HasSegment( segment ) == false )
            {
                delete segment;
            }
        }
    }
}

FSnapshotPath::FSnapshotPath( FOdysseyVectorPath* iPath, uint64 iSnapshotFlags )
    : FSnapshotObject( iPath, iSnapshotFlags )
    , mBrush( nullptr )
{
    // snapshot vertex and segment list
    if( iSnapshotFlags & FSnapshotFlags::Object::Path::TOPOLOGY )
    {
        mTopologyVertexList = iPath->GetVertexList();
        mTopologySegmentList = iPath->GetSegmentList();
    }

     // Snapshot vertices position & radius
    if( iSnapshotFlags & FSnapshotFlags::Object::Path::VERTICES )
    {
        std::list<FOdysseyVectorVertex*>& vertexList = iPath->GetVertexList();

        mVertexSnapshotArray.reserve( vertexList.size() );

        for( FOdysseyVectorVertex* vertex : vertexList )
        {
            mVertexSnapshotArray.push_back( FSnapshotVertex( vertex, FSnapshotFlags::ALL ) );
        }
    }

     // Snapshot segments' handles position
    if( iSnapshotFlags & FSnapshotFlags::Object::Path::SEGMENTS )
    {
        std::list<FOdysseyVectorSegment*>& segmentList = iPath->GetSegmentList();

        mCubicSegmentSnapshotArray.reserve( segmentList.size() );

        for( FOdysseyVectorSegment* segment : segmentList )
        {
            if( segment->GetClass() == FOdysseyVectorSegmentCubic::StaticClass() )
            {
                FOdysseyVectorSegmentCubic* cubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(segment);

                mCubicSegmentSnapshotArray.push_back( FSnapshotSegmentCubic( cubicSegment, FSnapshotFlags::ALL ) );
            }
        }
    }

    if( iSnapshotFlags & FSnapshotFlags::Object::Path::SELECTED_VERTICES )
    {
        mSelectedVertexList = iPath->GetSelectedVertexList();
    }

    if( iSnapshotFlags & FSnapshotFlags::Object::Path::JOINTTYPE )
    {
        mJointType = iPath->GetJointType();
    }

    if( iSnapshotFlags & FSnapshotFlags::Object::Path::MITERLIMIT )
    {
        mMiterLimit = iPath->GetMiterLimit();
    }

    if( iSnapshotFlags & FSnapshotFlags::Object::Path::BRUSH )
    {
        mBrush = iPath->GetBrush();
    }
}

bool
FSnapshotPath::Restore()
{
    FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(mObject);

    if( FSnapshotObject::Restore() )
    {
        if( mSnapshotFlags & FSnapshotFlags::Object::Path::TOPOLOGY )
        {
            std::list<FOdysseyVectorVertex*> swapTopologyVertexList = path->GetVertexList();
            std::list<FOdysseyVectorSegment*> swapTopologySegmentList = path->GetSegmentList();

            path->RemoveAllSegments();
            path->RemoveAllVertices();

            for( FOdysseyVectorVertex* vertex : mTopologyVertexList )
            {
                path->AddVertex( vertex );
            }

            for( FOdysseyVectorSegment* segment : mTopologySegmentList )
            {
                path->AddSegment( segment );
            }

            mTopologyVertexList = swapTopologyVertexList;
            mTopologySegmentList = swapTopologySegmentList;
        }

        if( mSnapshotFlags & FSnapshotFlags::Object::Path::VERTICES )
        {
            for( int i = 0; i < mVertexSnapshotArray.size(); i++ )
            {
                mVertexSnapshotArray[i].Restore();
            }
        }

        if( mSnapshotFlags & FSnapshotFlags::Object::Path::SEGMENTS )
        {
            for( int i = 0; i < mCubicSegmentSnapshotArray.size(); i++ )
            {
                mCubicSegmentSnapshotArray[i].Restore();
            }
        }

        if( mSnapshotFlags & FSnapshotFlags::Object::Path::SELECTED_VERTICES )
        {
            std::list<FOdysseyVectorVertex*> currentVertexList = path->GetSelectedVertexList();

            path->UnselectAllVertices();

            for( FOdysseyVectorVertex* vertex : mSelectedVertexList )
            {
                path->SelectVertex( vertex );
            }

            mSelectedVertexList = currentVertexList;
        }

        if( mSnapshotFlags & FSnapshotFlags::Object::Path::JOINTTYPE )
        {
            eJointType currentJointType = path->GetJointType();

            path->SetJointType( mJointType, true );
            // swap
            mJointType = currentJointType;
        }

        if( mSnapshotFlags & FSnapshotFlags::Object::Path::MITERLIMIT )
        {
            double currentMiterLimit = path->GetMiterLimit();

            path->SetMiterLimit( mMiterLimit, true );

            mMiterLimit = currentMiterLimit;
        }

        if( mSnapshotFlags & FSnapshotFlags::Object::Path::BRUSH )
        {
            FOdysseyVectorBrush currentBrush = path->GetBrush();

            path->SetBrush( mBrush );
            // swap
            mBrush = currentBrush;
        }

        return true;
    }

    return false;
}

FSnapshotGroup::~FSnapshotGroup()
{
}

FSnapshotGroup::FSnapshotGroup( FOdysseyVectorGroup* iGroup
                              , uint64 iSnapshotFlags )
    : FSnapshotObject( iGroup, iSnapshotFlags )
{
    if( iSnapshotFlags & FSnapshotFlags::Object::Group::HUDCOLOR )
    {
        mHUDColor = iGroup->GetHUDColor();
    }
}

bool
FSnapshotGroup::Restore()
{
    if( FSnapshotObject::Restore() )
    {
        FOdysseyVectorGroup* group = static_cast<FOdysseyVectorGroup*>(mObject);

        if( mSnapshotFlags & FSnapshotFlags::Object::Group::HUDCOLOR )
        {
            FColor currentHUDColor = group->GetHUDColor();

            group->SetHUDColor( mHUDColor );

            mHUDColor = currentHUDColor;
        }

        return true;
    }

    return false;
}

FSnapshotGroupPaint::~FSnapshotGroupPaint()
{
}

FSnapshotGroupPaint::FSnapshotGroupPaint( FOdysseyVectorGroupPaint* iPaintGroup
                                        , uint64 iSnapshotFlags )
    : FSnapshotGroup( iPaintGroup, iSnapshotFlags )
{
    if( iSnapshotFlags & FSnapshotFlags::Object::Group::Paint::BUCKETS )
    {
        std::list<FOdysseyVectorBucket*>& bucketList = iPaintGroup->GetBucketList();

        mBucketSnapshotArray.reserve( bucketList.size() );

        for( FOdysseyVectorBucket* bucket : bucketList )
        {
            mBucketSnapshotArray.push_back( FSnapshotBucket( bucket, FSnapshotFlags::ALL ) );
        }
    }

    if( iSnapshotFlags & FSnapshotFlags::Object::Group::Paint::SELECTED_BUCKETS )
    {
        mSelectedBucketList = iPaintGroup->GetSelectedBucketList();
    }

    if( mSnapshotFlags & FSnapshotFlags::Object::Group::Paint::PAINTED )
    {
        bPainted = iPaintGroup->IsPainted();
    }

    if( mSnapshotFlags & FSnapshotFlags::Object::Group::Paint::MONOCHROME )
    {
        bMonochrome = iPaintGroup->IsMonochrome();
    }

    if( mSnapshotFlags & FSnapshotFlags::Object::Group::Paint::MONOCHROMECOLOR )
    {
        mMonochromeColor = iPaintGroup->GetMonochromeColor();
    }
    if( mSnapshotFlags & FSnapshotFlags::Object::Group::Paint::REALTIME )
    {
        bRealtime = iPaintGroup->IsRealtime();
    }

    if( mSnapshotFlags & FSnapshotFlags::Object::Group::Paint::GAPTOLERANCE )
    {
        mGapTolerance = iPaintGroup->GetGapTolerance();
    }

    if( mSnapshotFlags & FSnapshotFlags::Object::Group::Paint::WIREFRAME )
    {
        bWireframe = iPaintGroup->IsWireframe();
    }

    if( mSnapshotFlags & FSnapshotFlags::Object::Group::Paint::WIREFRAMECOLOR )
    {
        mWireframeColor = iPaintGroup->GetWireframeColor();
    }
}

bool
FSnapshotGroupPaint::Restore()
{
    if( FSnapshotGroup::Restore() )
    {
        FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>(mObject);

        if( mSnapshotFlags & FSnapshotFlags::Object::Group::Paint::BUCKETS )
        {
            for( int i = 0; i < mBucketSnapshotArray.size(); i++ )
            {
                mBucketSnapshotArray[i].Restore();
            }
        }

        if( mSnapshotFlags & FSnapshotFlags::Object::Group::Paint::SELECTED_BUCKETS )
        {
            std::list<FOdysseyVectorBucket*> currentBucketList = paintGroup->GetSelectedBucketList();

            paintGroup->UnselectAllBuckets();

            for( FOdysseyVectorBucket* bucket : mSelectedBucketList )
            {
                paintGroup->SelectBucket( bucket );
            }
            // swap
            mSelectedBucketList = currentBucketList;
        }

        if( mSnapshotFlags & FSnapshotFlags::Object::Group::Paint::PAINTED )
        {
            bool currentPainted = paintGroup->IsPainted();

            paintGroup->SetPainted( bPainted );
            // swap
            bPainted = currentPainted;
        }

        if( mSnapshotFlags & FSnapshotFlags::Object::Group::Paint::MONOCHROME )
        {
            bool currentMonochrome = paintGroup->IsMonochrome();

            paintGroup->SetMonochrome( bMonochrome );
            // swap
            bMonochrome = currentMonochrome;
        }

        if( mSnapshotFlags & FSnapshotFlags::Object::Group::Paint::MONOCHROMECOLOR )
        {
            FColor currentMonochromeColor = paintGroup->GetMonochromeColor();

            paintGroup->SetMonochromeColor( mMonochromeColor );
            // swap
            mMonochromeColor = currentMonochromeColor;
        }
        if( mSnapshotFlags & FSnapshotFlags::Object::Group::Paint::REALTIME )
        {
            bool currentRealtime = paintGroup->IsRealtime();

            paintGroup->SetRealtime( bRealtime );
            // swap
            bRealtime = currentRealtime;
        }

        if( mSnapshotFlags & FSnapshotFlags::Object::Group::Paint::GAPTOLERANCE )
        {
            double currentGapTolerance = paintGroup->GetGapTolerance();

            paintGroup->SetGapTolerance( mGapTolerance );
            // swap
            mGapTolerance = currentGapTolerance;
        }

        if( mSnapshotFlags & FSnapshotFlags::Object::Group::Paint::WIREFRAME )
        {
            bool currentWireframe = paintGroup->IsWireframe();

            paintGroup->SetWireframe( bWireframe );
            // swap
            bWireframe = currentWireframe;
        }

        if( mSnapshotFlags & FSnapshotFlags::Object::Group::Paint::WIREFRAMECOLOR )
        {
            FColor currentWireframeColor = paintGroup->GetWireframeColor();

            paintGroup->SetWireframeColor( mWireframeColor );
            // swap
            mWireframeColor = currentWireframeColor;
        }

        return true;
    }

    return false;
}

void
FSnapshotCell::RecordState( eSnapshotState iStateType )
{
    FOdysseyVectorCell* cell = static_cast<FOdysseyVectorCell*>(mObject);
    FSnapshotCell::State *requestedState = nullptr;

    switch( iStateType )
    {
        case eSnapshotState::Initial :
            requestedState = &mInitialState;
        break;

        case eSnapshotState::Altered :
            requestedState = &mAlteredState;
        break;

        default :
        break;
    }

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

bool
FSnapshotCell::LoadState( eSnapshotState iStateType )
{
    FOdysseyVectorCell* cell = static_cast<FOdysseyVectorCell*>(mObject);
    FSnapshotCell::State *requestedState = nullptr;

    switch( iStateType )
    {
        case eSnapshotState::Initial :
            requestedState = &mInitialState;
        break;

        case eSnapshotState::Altered :
            requestedState = &mAlteredState;
        break;

        default :
        break;
    }

    cell->ClearObjectSelection();

    for( FOdysseyVectorObject* selectedObject : requestedState->selectedObjectArray )
    {
        cell->SelectObject( selectedObject );
    }

    return true; // restore succeeded
}

FSnapshotCell::~FSnapshotCell()
{
}

FSnapshotCell::FSnapshotCell( FOdysseyVectorCell* iCell )
    : FSnapshotObject( iCell, 0 )
{
}

FSnapshotCell::FSnapshotCell( FOdysseyVectorCell* iCell
                            , uint64 iSnapshotFlags
                            , eSnapshotState iStateType )
    : FSnapshotObject ( iCell, iSnapshotFlags )
    , mSnapshotFlags( iSnapshotFlags )
{
    RecordState( iStateType );
}

FOdysseyVectorCell*
FSnapshotCell::GetCell()
{
    return static_cast<FOdysseyVectorCell*>(mObject);
}
