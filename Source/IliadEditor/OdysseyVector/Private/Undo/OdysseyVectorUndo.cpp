// IDDN.FR.001.060015.015.S.X.2019.000.00000
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

FOdysseyVectorUndo::FOdysseyVectorUndo( FOdysseyVectorLayer* iSharedEnv )
    : mApplied( true )
    , mLayer( iSharedEnv )
    , bUpdateViaDelegation ( true )
{
}

void
FOdysseyVectorUndo::SetUpdateViaDelegation( bool iUpdateViaDelegation )
{
    bUpdateViaDelegation = iUpdateViaDelegation;
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
FOdysseyVectorUndo::UpdateLayer()
{
    // update invalidated objects
    mLayer->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    // prepare for full redraw
    for( FOdysseyVectorCell* cell : mLayer->GetInvalidateCellList() )
    {
        cell->InvalidateRect();
    }

    mLayer->RequestRedraw( nullptr, 0 );

    OnPostUndoRedoDelegate().Broadcast();
}

// staic
FOdysseyVectorUndo::FOnPostUndoRedo&
FOdysseyVectorUndo::OnPostUndoRedoDelegate()
{
    static FOnPostUndoRedo onPostUndoRedo;

    return onPostUndoRedo;
}

void
FOdysseyVectorUndo::Update()
{
    if( bUpdateViaDelegation )
    {
        // call callbacks if any (for refreshing GUI e.g)
        FOdysseyUndoDelegates::Get().OnAfterUndoRedo().AddLambda(
            [this]( bool iIsRedo )
            {
                UpdateLayer();
            }
        );
    }
    else
    {
        UpdateLayer();
    }
}

void
FOdysseyVectorUndo::GetInbetweenerTagList( const std::list<FOdysseyVectorObject*>& iObjectList
                                         , std::list<FOdysseyVectorTagInbetweener*>& oInbetweenerTagList )
{
    std::vector<FOdysseyVectorObject*> objectArray;

    objectArray.reserve( iObjectList.size() );

    for( FOdysseyVectorObject* object : iObjectList )
    {
        objectArray.push_back( object );
    }

    GetInbetweenerTagList( objectArray, oInbetweenerTagList );
}

void
FOdysseyVectorUndo::GetInbetweenerTagList( const std::vector<FOdysseyVectorObject*>& iObjectArray
                                         , std::list<FOdysseyVectorTagInbetweener*>& oInbetweenerTagList )
{
    uint32 inbetweenerClass = FOdysseyVectorTagInbetweener::StaticClass();

    for( FOdysseyVectorObject* removedObject : iObjectArray )
    {
        FOdysseyVectorObject* taggedAncestor = removedObject->GetAncestorByClassAndTag( FOdysseyVectorObject::StaticClass()
                                                                                      , true
                                                                                      , inbetweenerClass
                                                                                      , false );
        if( taggedAncestor )
        {
            FOdysseyVectorTagInbetweener* inbetweenerTag = static_cast<FOdysseyVectorTagInbetweener*>( taggedAncestor->GetTagByType( inbetweenerClass ) );

            if( std::find( oInbetweenerTagList.begin()
                         , oInbetweenerTagList.end()
                         , inbetweenerTag ) == oInbetweenerTagList.end() )
            {
                oInbetweenerTagList.push_back(  inbetweenerTag );
            }
        }
    }
}

FSnapshotTrajectory::~FSnapshotTrajectory()
{
    if( mInitialState )
        delete mInitialState;

    if( mAlteredState )
        delete mAlteredState;
}

FSnapshotTrajectory::FSnapshotTrajectory( FInbetweenerTrajectory* iTrajectory
                                        , uint64 iSnapshotFlags )
    : mRoute( iTrajectory->GetRoute() )
    , mIndex( iTrajectory - &iTrajectory->GetRoute()->GetTrajectoryBuffer()[0] )
    , mSnapshotFlags( iSnapshotFlags )
    , mInitialState ( nullptr )
    , mAlteredState ( nullptr )
{
}

void
FSnapshotTrajectory::Clean( eSnapshotState iKeepState )
{
    // nothing to delete
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

FSnapshotStep::FSnapshotStep( FInbetweenerStep* iStep )
    : mRoute( iStep->GetRoute() )
    , mIndex( iStep - &iStep->GetRoute()->GetStepBuffer()[0] )
    , mInitialState ( nullptr )
    , mAlteredState ( nullptr )
{
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
                              , uint64 iTrajectorySnapshotFlags )
    : mRoute( iRoute )
    , mSnapshotFlags( iSnapshotflags ) // unused
    , mTrajectorySnapshotFlags( iTrajectorySnapshotFlags )
    , mInitialState ( nullptr )
    , mAlteredState ( nullptr )
{
}

void
FSnapshotRoute::Clean( eSnapshotState iKeepState )
{
    // if the route does not have a snapshot of the requetsed state,
    // it means it was non-existent when the snapshot was taken
    if( GetState( iKeepState ) == nullptr )
    {
        delete mRoute;
    }
}

FSnapshotRoute::State*
FSnapshotRoute::GetState( eSnapshotState iStateType )
{
    switch( iStateType )
    {
        case eSnapshotState::Initial :
        return mInitialState;

        case eSnapshotState::Altered :
        return mAlteredState;

        default :
        break;
    }

    return nullptr;
}

FSnapshotTrajectory*
FSnapshotRoute::GetTrajectorySnapshot( uint32 iTrajectoryIndex )
{
    for( FSnapshotTrajectory& trajectorySnapshot : mTrajectorySnapshotBuffer )
    {
        // theoritically we could use the index of the trajectory in the array, but I'll play it safe
        if( trajectorySnapshot.GetIndex() == iTrajectoryIndex )
        {
            return &trajectorySnapshot;
        }
    }

    return nullptr;
}

FSnapshotStep*
FSnapshotRoute::GetStepSnapshot( uint32 iStepIndex )
{
    for( FSnapshotStep& stepSnapshot : mStepSnapshotBuffer )
    {
        // theoritically we could use the index of the trajectory in the array, but I'll play it safe
        if( stepSnapshot.GetIndex() == iStepIndex )
        {
            return &stepSnapshot;
        }
    }

    return nullptr;
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

            requestedState->inited = true;
        }

        if( mSnapshotFlags & FSnapshotFlags::Route::TRAJECTORIES )
        {
            for( FInbetweenerTrajectory& trajectory : mRoute->GetTrajectoryBuffer() )
            {
                FSnapshotTrajectory* trajectorySnapshot = GetTrajectorySnapshot( trajectory.GetIndex() );

                if( trajectorySnapshot == nullptr )
                {
                    trajectorySnapshot = &mTrajectorySnapshotBuffer.emplace_back( &trajectory
                                                                                , mTrajectorySnapshotFlags );
                }

                trajectorySnapshot->RecordState( iStateType );
            }
        }

        if( mSnapshotFlags & FSnapshotFlags::Route::STEPS )
        {
            for( FInbetweenerStep& step : mRoute->GetStepBuffer() )
            {
                FSnapshotStep* stepSnapshot = GetStepSnapshot( step.GetIndex() );

                if( stepSnapshot == nullptr )
                {
                    stepSnapshot = &mStepSnapshotBuffer.emplace_back( &step );
                }

                stepSnapshot->RecordState( iStateType );
            }
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
            for( FSnapshotTrajectory& trajectorySnapshot : mTrajectorySnapshotBuffer )
            {
                trajectorySnapshot.LoadState( iStateType );
            }
        }

        if( mSnapshotFlags & FSnapshotFlags::Route::STEPS )
        {
            for( FSnapshotStep& stepSnapshot : mStepSnapshotBuffer )
            {
                stepSnapshot.LoadState( iStateType );
            }
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
                                                    , uint64 iSnapshotFlags )
    : mChart( iChart )
    , mSnapshotFlags( iSnapshotFlags )
    , mInitialState ( nullptr )
    , mAlteredState ( nullptr )
{
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

    if( requestedState )
    {
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
                                                            , uint64 iSnapshotFlags )
    : mBreakdown( iBreakdown )
    , mSnapshotFlags( iSnapshotFlags )
    , mInitialState ( nullptr )
    , mAlteredState ( nullptr )
    , mChartSnapshot( mBreakdown->GetChart()
                   , ( ( mSnapshotFlags & FSnapshotFlags::Breakdown::CHART ) ? FSnapshotFlags::ALL : 0 ) )
{
}

void
FSnapshotInbetweenerBreakdown::Clean( eSnapshotState iKeepState )
{
    // if the breakdown does not have a snapshot of the requetsed state,
    // it means it was non-existent when the snapshot was taken
    if( GetState( iKeepState ) == nullptr )
    {
        mBreakdown = nullptr;
    }
}

FSnapshotInbetweenerBreakdown::State*
FSnapshotInbetweenerBreakdown::GetState( eSnapshotState iStateType )
{
    switch( iStateType )
    {
        case eSnapshotState::Initial :
        return mInitialState;

        case eSnapshotState::Altered :
        return mAlteredState;

        default :
        break;
    }

    return nullptr;
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
            requestedState->target = mBreakdown->GetTargetDrawingIndex();

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
                                              , requestedState->scalingY
                                              , requestedState->skewX
                                              , requestedState->skewY );
            }

            if( mSnapshotFlags & FSnapshotFlags::Breakdown::TARGETVISIBILITY )
            {
                requestedState->targetVisibility  = mBreakdown->IsTargetVisible();
            }

            requestedState->inited = true;
        }

        mChartSnapshot.RecordState( iStateType );
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
                                          , requestedState->scalingY
                                          , requestedState->skewX
                                          , requestedState->skewY );

            mBreakdown->UpdateMatrix();
        }

        if( mSnapshotFlags & FSnapshotFlags::Breakdown::TARGETVISIBILITY )
        {
            mBreakdown->SetTargetVisibility( requestedState->targetVisibility );
        }

        mChartSnapshot.LoadState( iStateType );

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
                                                , uint64 iTrajectorySnapshotFlags )
    : mInbetweenerTag( iInbetweenerTag )
    , mSnapshotFlags( iSnapshotFlags )
    , mBreakdownSnapshotFlags( iBreakdownSnapshotFlags )
    , mRouteSnapshotFlags( iRouteSnapshotFlags )
    , mTrajectorySnapshotFlags( iTrajectorySnapshotFlags )
    , mInitialState( nullptr )
    , mAlteredState( nullptr )
{
}

void
FSnapshotTagInbetweener::Clean( eSnapshotState iKeepState ) // Applied = true means keep the altered version
{
    for( FSnapshotInbetweenerBreakdown& breakdownSnapshot : mBreakdownSnapshotBuffer )
    {
        breakdownSnapshot.Clean( iKeepState );
    }

    for( FSnapshotRoute& routeSnapshot : mRouteSnapshotBuffer )
    {
        routeSnapshot.Clean( iKeepState );
    }
}

FSnapshotInbetweenerBreakdown*
FSnapshotTagInbetweener::GetBreakdownSnapshot( FInbetweenerBreakdown* iBreakdown )
{
    for( FSnapshotInbetweenerBreakdown& breakdownSnapshot : mBreakdownSnapshotBuffer )
    {
        if( breakdownSnapshot.GetBreakdown() == iBreakdown )
        {
            return &breakdownSnapshot;
        }
    }

    return nullptr;
}

FSnapshotRoute*
FSnapshotTagInbetweener::GetRouteSnapshot( FInbetweenerRoute* iRoute )
{
    for( FSnapshotRoute& routeSnapshot : mRouteSnapshotBuffer )
    {
        if( routeSnapshot.GetRoute() == iRoute )
        {
            return &routeSnapshot;
        }
    }

    return nullptr;
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

            requestedState->inited = true;
        }

        // Snapshot breakdown layout
        if( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::BREAKDOWNS )
        {
            for( FInbetweenerBreakdown* breakdown : mInbetweenerTag->GetBreakdownList() )
            {
                FSnapshotInbetweenerBreakdown* breakdownSnapshot = GetBreakdownSnapshot( breakdown );

                if( breakdownSnapshot == nullptr )
                {
                    breakdownSnapshot = &mBreakdownSnapshotBuffer.emplace_back( breakdown
                                                                             , mBreakdownSnapshotFlags );
                }

                breakdownSnapshot->RecordState( iStateType );
            }
        }

        // Snapshot route layout
        if( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::ROUTES )
        {
            for( FInbetweenerRoute* route : mInbetweenerTag->GetRouteList() )
            {
                FSnapshotRoute* routeSnapshot = GetRouteSnapshot( route );

                if( routeSnapshot == nullptr )
                {
                    routeSnapshot = &mRouteSnapshotBuffer.emplace_back( route
                                                                      , mRouteSnapshotFlags
                                                                      , mTrajectorySnapshotFlags );
                }

                routeSnapshot->RecordState( iStateType );
            }
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
            // reset all breakdowns first (leave the default one )
            mInbetweenerTag->ResetLayout( false );

            for( FSnapshotInbetweenerBreakdown& breakdownSnapshot : mBreakdownSnapshotBuffer )
            {
                // if a state was recorded that matches the requested state (either initial or altered)
                if( breakdownSnapshot.GetState( iStateType ) )
                {
                    // restore the breakdown
                    mInbetweenerTag->AddBreakdown( breakdownSnapshot.GetBreakdown()
                                                 , breakdownSnapshot.GetState( iStateType )->target
                                                 , false
                                                 , false );
                }
            }

            for( FSnapshotInbetweenerBreakdown& inbetweenerBreakdownSnapshot : mBreakdownSnapshotBuffer )
            {
                inbetweenerBreakdownSnapshot.LoadState( iStateType );
            }
        }

        // restore route layout
        if( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::ROUTES )
        {
            // reset all breakdowns first (leave the default one )
            mInbetweenerTag->RemoveAllRoutes();

            for( FSnapshotRoute& routeSnapshot : mRouteSnapshotBuffer )
            {
                // if a state was recorded that matches the requested state (either initial or altered)
                if( routeSnapshot.GetState( iStateType ) )
                {
                    // restore the route
                    mInbetweenerTag->AddRoute( routeSnapshot.GetRoute() );
                }
            }

            for( FSnapshotRoute& routeSnapshot : mRouteSnapshotBuffer )
            {
                routeSnapshot.LoadState( iStateType );
            }
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
    : mObject( iObject )
    , mSnapshotFlags( iSnapshotFlags )
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
                                     , requestedState->scalingY
                                     , requestedState->skewX
                                     , requestedState->skewY );
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
                                 , requestedState->scalingY
                                 , requestedState->skewX
                                 , requestedState->skewY );

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
    : mPoint( iPoint )
    , mSnapshotFlags( iSnapshotFlags )
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
                requestedState->alignment = GetVertex()->IsHandleAligned();
            }

            if( mSnapshotFlags & FSnapshotFlags::Point::Vertex::LOCK )
            {
                requestedState->locked = GetVertex()->IsLocked();
            }

            if( mSnapshotFlags & FSnapshotFlags::Point::Vertex::RADIUS )
            {
                requestedState->radius = GetVertex()->GetRadius();
            }

            requestedState->inited = true;
        }
    }
}

FOdysseyVectorVertex*
FSnapshotVertex::GetVertex()
{
    return static_cast<FOdysseyVectorVertex*>(mPoint);
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
            GetVertex()->SetHandleAligned( requestedState->alignment );
        }

        if( mSnapshotFlags & FSnapshotFlags::Point::Vertex::LOCK )
        {
            GetVertex()->SetLocked( requestedState->locked );
        }

        if( mSnapshotFlags & FSnapshotFlags::Point::Vertex::RADIUS )
        {
            GetVertex()->SetRadius( requestedState->radius );
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
                requestedState->colorMode = GetBucket()->GetColorMode();
            }

            if( mSnapshotFlags & FSnapshotFlags::Point::Bucket::SOLIDCOLOR )
            {
                requestedState->solidColor = GetBucket()->GetSolidColor();
            }

            if( mSnapshotFlags & FSnapshotFlags::Point::Bucket::ROTATION )
            {
                requestedState->rotation = GetBucket()->GetRotation();
            }

            if( mSnapshotFlags & FSnapshotFlags::Point::Bucket::PROPAGATION )
            {
                requestedState->propagated = GetBucket()->IsPropagated();
            }

            if( mSnapshotFlags & FSnapshotFlags::Point::Bucket::GRADIENTCOLOR0 )
            {
                requestedState->gradientColor0 = GetBucket()->GetGradientColor0();
            }

            if( mSnapshotFlags & FSnapshotFlags::Point::Bucket::GRADIENTCOLOR1 )
            {
                requestedState->gradientColor1 = GetBucket()->GetGradientColor1();
            }

            if( mSnapshotFlags & FSnapshotFlags::Point::Bucket::RADIALRADIUS )
            {
                requestedState->radialRadius = GetBucket()->GetRadialRadius();
            }

            if( mSnapshotFlags & FSnapshotFlags::Point::Bucket::RADIALOFFSET )
            {
                requestedState->radialOffset = GetBucket()->GetRadialOffset();
            }

            if( mSnapshotFlags & FSnapshotFlags::Point::Bucket::LINEARP0 )
            {
                requestedState->linearP0 = GetBucket()->GetLinearP0();
            }

            if( mSnapshotFlags & FSnapshotFlags::Point::Bucket::LINEARP1 )
            {
                requestedState->linearP1 = GetBucket()->GetLinearP1();
            }

            if( mSnapshotFlags & FSnapshotFlags::Point::Bucket::PALETTEENTRY )
            {
                requestedState->paletteEntry = GetBucket()->GetPaletteEntry();
            }

            requestedState->inited = true;
        }
    }
}

FOdysseyVectorBucket*
FSnapshotBucket::GetBucket()
{
    return static_cast<FOdysseyVectorBucket*>(mPoint);
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
            GetBucket()->SetColorMode( requestedState->colorMode );
        }

        if( mSnapshotFlags & FSnapshotFlags::Point::Bucket::SOLIDCOLOR )
        {
            GetBucket()->SetSolidColor( requestedState->solidColor );
        }

        if( mSnapshotFlags & FSnapshotFlags::Point::Bucket::LINEARP0 )
        {
            GetBucket()->SetLinearP0( requestedState->linearP0 );
        }

        if( mSnapshotFlags & FSnapshotFlags::Point::Bucket::LINEARP1 )
        {
            GetBucket()->SetLinearP1( requestedState->linearP1 );
        }

        if( mSnapshotFlags & FSnapshotFlags::Point::Bucket::PROPAGATION )
        {
            GetBucket()->SetPropagated( requestedState->propagated );
        }

        if( mSnapshotFlags & FSnapshotFlags::Point::Bucket::GRADIENTCOLOR0 )
        {
            GetBucket()->SetGradientColor0( requestedState->gradientColor0 );
        }

        if( mSnapshotFlags & FSnapshotFlags::Point::Bucket::GRADIENTCOLOR1 )
        {
            GetBucket()->SetGradientColor1( requestedState->gradientColor1 );
        }

        if( mSnapshotFlags & FSnapshotFlags::Point::Bucket::RADIALRADIUS )
        {
            GetBucket()->SetRadialRadius( requestedState->radialRadius );
        }

        if( mSnapshotFlags & FSnapshotFlags::Point::Bucket::RADIALOFFSET )
        {
            GetBucket()->SetRadialOffset( requestedState->radialOffset );
        }

        if( mSnapshotFlags & FSnapshotFlags::Point::Bucket::PALETTEENTRY )
        {
            GetBucket()->SetPaletteEntry( requestedState->paletteEntry );
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
    : mCubicSegment( iCubicSegment )
    , mSnapshotFlags( iSnapshotFlags )
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
        if( mPathInitialState )
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
        }

        if( mPathAlteredState )
        {
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
    }

    if( mPathInitialState )
        delete mPathInitialState;

    if( mPathAlteredState )
        delete mPathAlteredState;
}

FOdysseyVectorPath*
FSnapshotPath::GetPath()
{
    return static_cast<FOdysseyVectorPath*>(mObject);
}

FSnapshotPath::FSnapshotPath( FOdysseyVectorPath* iPath
                            , uint64 iSnapshotFlags
                            , eSnapshotState iStateType )
    : FSnapshotObject( iPath, iSnapshotFlags, eSnapshotState::None )
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
                requestedState->topologyVertexList = GetPath()->GetVertexList();
                requestedState->topologySegmentList = GetPath()->GetSegmentList();
            }

             // Snapshot vertices position & radius
            if( mSnapshotFlags & FSnapshotFlags::Object::Path::VERTICES )
            {
                std::list<FOdysseyVectorVertex*>& vertexList = GetPath()->GetVertexList();

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
                std::list<FOdysseyVectorSegment*>& segmentList = GetPath()->GetSegmentList();

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
                requestedState->selectedVertexList = GetPath()->GetSelectedVertexList();
            }

            if( mSnapshotFlags & FSnapshotFlags::Object::Path::JOINTTYPE )
            {
                requestedState->jointType = GetPath()->GetJointType();
            }

            if( mSnapshotFlags & FSnapshotFlags::Object::Path::MITERLIMIT )
            {
                requestedState->miterLimit = GetPath()->GetMiterLimit();
            }

            if( mSnapshotFlags & FSnapshotFlags::Object::Path::BRUSH )
            {
                requestedState->brush = GetPath()->GetBrush();
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
                GetPath()->RemoveAllSegments();
                GetPath()->RemoveAllVertices();

                for( FOdysseyVectorVertex* vertex : requestedState->topologyVertexList )
                {
                    GetPath()->AddVertex( vertex );
                }

                for( FOdysseyVectorSegment* segment : requestedState->topologySegmentList )
                {
                    GetPath()->AddSegment( segment );
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
                GetPath()->UnselectAllVertices();

                for( FOdysseyVectorVertex* vertex : requestedState->selectedVertexList )
                {
                    GetPath()->SelectVertex( vertex );
                }
            }

            if( mSnapshotFlags & FSnapshotFlags::Object::Path::JOINTTYPE )
            {
                GetPath()->SetJointType( requestedState->jointType, true );
            }

            if( mSnapshotFlags & FSnapshotFlags::Object::Path::MITERLIMIT )
            {
                GetPath()->SetMiterLimit( requestedState->miterLimit, true );
            }

            if( mSnapshotFlags & FSnapshotFlags::Object::Path::BRUSH )
            {
                GetPath()->SetBrush( requestedState->brush );
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
    , mGroupInitialState( nullptr )
    , mGroupAlteredState( nullptr )
{
    // Note: even though the ctor for the base class FSnapshotObject calls RecordState, it will not call the overriden
    // method because vtable don't exist at construction time. So we call it here, knowing that
    // FSnapshotObject::Recordstate will receive eSnapshotState::None so we don't do thing twice.
    RecordState( iStateType );
}

FOdysseyVectorGroup*
FSnapshotGroup::GetGroup()
{
    return static_cast<FOdysseyVectorGroup*>(mObject);
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
                requestedState->HUDColor = GetGroup()->GetHUDColor();
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
                GetGroup()->SetHUDColor( requestedState->HUDColor );
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
    , mPaintgroupInitialState( nullptr )
    , mPaintgroupAlteredState( nullptr )
{
    // Note: even though the ctor for the base class FSnapshotGroup calls RecordState, it will not call the overriden
    // method because vtable don't exist at construction time. So we call it here, knowing that
    // FSnapshotGroup::Recordstate will receive eSnapshotState::None so we don't do thing twice.
    RecordState( iStateType );
}

FOdysseyVectorGroupPaint*
FSnapshotGroupPaint::GetPaintGroup()
{
    return static_cast<FOdysseyVectorGroupPaint*>(mObject);
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
                std::list<FOdysseyVectorBucket*>& bucketList = GetPaintGroup()->GetBucketList();

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
                requestedState->selectedBucketList = GetPaintGroup()->GetSelectedBucketList();
            }

            if( mSnapshotFlags & FSnapshotFlags::Object::Group::Paint::PAINTED )
            {
                requestedState->painted = GetPaintGroup()->IsPainted();
            }

            if( mSnapshotFlags & FSnapshotFlags::Object::Group::Paint::INTERSECTSCANVAS )
            {
                requestedState->intersectsCanvas = GetPaintGroup()->IntersectsCanvas();
            }

            if( mSnapshotFlags & FSnapshotFlags::Object::Group::Paint::MONOCHROME )
            {
                requestedState->monochrome = GetPaintGroup()->IsMonochrome();
            }

            if( mSnapshotFlags & FSnapshotFlags::Object::Group::Paint::MONOCHROMECOLOR )
            {
                requestedState->monochromeColor = GetPaintGroup()->GetMonochromeColor();
            }
            if( mSnapshotFlags & FSnapshotFlags::Object::Group::Paint::REALTIME )
            {
                requestedState->realtime = GetPaintGroup()->IsRealtime();
            }

            if( mSnapshotFlags & FSnapshotFlags::Object::Group::Paint::GAPTOLERANCE )
            {
                requestedState->gapTolerance = GetPaintGroup()->GetGapTolerance();
            }

            if( mSnapshotFlags & FSnapshotFlags::Object::Group::Paint::SEGMENTEXTENSIONSCHEME )
            {
                requestedState->segmentExtensionScheme = GetPaintGroup()->GetSegmentExtensionScheme();
            }

            if( mSnapshotFlags & FSnapshotFlags::Object::Group::Paint::GAPDETECTIONSCHEME )
            {
                requestedState->gapDetectionScheme = GetPaintGroup()->GetGapDetectionScheme();
            }

            if( mSnapshotFlags & FSnapshotFlags::Object::Group::Paint::SEGMENTEXTENSIONSIMPLIFIED )
            {
                requestedState->segmentExtensionSimplified = GetPaintGroup()->IsSegmentExtensionSimplified();
            }

            if( mSnapshotFlags & FSnapshotFlags::Object::Group::Paint::WIREFRAME )
            {
                requestedState->wireframe = GetPaintGroup()->IsWireframe();
            }

            if( mSnapshotFlags & FSnapshotFlags::Object::Group::Paint::WIREFRAMECOLOR )
            {
                requestedState->wireframeColor = GetPaintGroup()->GetWireframeColor();
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
                GetPaintGroup()->UnselectAllBuckets();

                for( FOdysseyVectorBucket* bucket : requestedState->selectedBucketList )
                {
                    GetPaintGroup()->SelectBucket( bucket );
                }
            }

            if( mSnapshotFlags & FSnapshotFlags::Object::Group::Paint::PAINTED )
            {
                GetPaintGroup()->SetPainted( requestedState->painted );
            }

            if( mSnapshotFlags & FSnapshotFlags::Object::Group::Paint::INTERSECTSCANVAS )
            {
                GetPaintGroup()->SetIntersectsCanvas( requestedState->intersectsCanvas );
            }

            if( mSnapshotFlags & FSnapshotFlags::Object::Group::Paint::MONOCHROME )
            {
                GetPaintGroup()->SetMonochrome( requestedState->monochrome );
            }

            if( mSnapshotFlags & FSnapshotFlags::Object::Group::Paint::MONOCHROMECOLOR )
            {
                GetPaintGroup()->SetMonochromeColor( requestedState->monochromeColor );
            }
            if( mSnapshotFlags & FSnapshotFlags::Object::Group::Paint::REALTIME )
            {
                GetPaintGroup()->SetRealtime( requestedState->realtime );
            }

            if( mSnapshotFlags & FSnapshotFlags::Object::Group::Paint::GAPTOLERANCE )
            {
                GetPaintGroup()->SetGapTolerance( requestedState->gapTolerance );
            }

            if( mSnapshotFlags & FSnapshotFlags::Object::Group::Paint::SEGMENTEXTENSIONSCHEME )
            {
                GetPaintGroup()->SetSegmentExtensionScheme( requestedState->segmentExtensionScheme );
            }

            if( mSnapshotFlags & FSnapshotFlags::Object::Group::Paint::GAPDETECTIONSCHEME )
            {
                GetPaintGroup()->SetGapDetectionScheme( requestedState->gapDetectionScheme );
            }

            if( mSnapshotFlags & FSnapshotFlags::Object::Group::Paint::SEGMENTEXTENSIONSIMPLIFIED )
            {
                GetPaintGroup()->SetSegmentExtensionSimplified( requestedState->segmentExtensionSimplified );
            }

            if( mSnapshotFlags & FSnapshotFlags::Object::Group::Paint::WIREFRAME )
            {
                GetPaintGroup()->SetWireframe( requestedState->wireframe );
            }

            if( mSnapshotFlags & FSnapshotFlags::Object::Group::Paint::WIREFRAMECOLOR )
            {
                GetPaintGroup()->SetWireframeColor( requestedState->wireframeColor );
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
