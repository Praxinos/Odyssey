#include "Undo/OdysseyVectorUndo.h"
#include "OdysseyVectorTagInbetweener.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorVertex.h"
#include "OdysseyVectorSegmentCubic.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorPath.h"

FOdysseyVectorUndo::~FOdysseyVectorUndo()
{
}

FOdysseyVectorUndo::FOdysseyVectorUndo( FOdysseyVectorGroupPaint *iScene, uint64 iReturnFlags )
    : mApplied( true )
    , mScene( iScene )
    , mSharedEnv( iScene->GetSharedEnv() )
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

FSnapshotTrajectory::~FSnapshotTrajectory()
{
}

FSnapshotTrajectory::FSnapshotTrajectory( FInbetweenerTrajectory* iTrajectory, uint64 iSnapshotFlags )
    : mSnapshotFlags( iSnapshotFlags )
    , mRoute( iTrajectory->GetRoute() )
    , mIndex( iTrajectory - &iTrajectory->GetRoute()->GetTrajectoryBuffer()[0] )
{
    RecordLocalState( &mInitialState );
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
FSnapshotTrajectory::RecordLocalState( FSnapshotTrajectory::State* iState )
{
    if( iState->inited == false )
    {
        FInbetweenerTrajectory* trajectory = &mRoute->GetTrajectoryBuffer()[mIndex];

        if( mSnapshotFlags & FSnapshotFlags::Trajectory::BEZIER )
        {
            iState->handleDirection[0] = trajectory->GetHandle(0)->GetDirection();
            iState->handleDirection[1] = trajectory->GetHandle(1)->GetDirection();
            iState->handleLengthRatio[0] = trajectory->GetHandle(0)->GetLengthRatio();
            iState->handleLengthRatio[1] = trajectory->GetHandle(1)->GetLengthRatio();
        }

        if( mSnapshotFlags & FSnapshotFlags::Trajectory::WAYPOINTS )
        {
            WaypointSpacingToArray( trajectory, iState->waypointSpacingBuffer );
        }

        iState->inited = true;
    }
}

void
FSnapshotTrajectory::RecordAlteredState()
{
    RecordLocalState( &mAlteredState );
}

bool
FSnapshotTrajectory::LoadState( FSnapshotTrajectory::State* iState )
{
    FInbetweenerTrajectory* trajectory = &mRoute->GetTrajectoryBuffer()[mIndex];

    if( mSnapshotFlags & FSnapshotFlags::Trajectory::BEZIER )
    {
        trajectory->GetHandle(0)->Set( iState->handleDirection[0], iState->handleLengthRatio[0] );
        trajectory->GetHandle(1)->Set( iState->handleDirection[1], iState->handleLengthRatio[1] );
    }

    if( mSnapshotFlags & FSnapshotFlags::Trajectory::WAYPOINTS )
    {
        std::vector<FInbetweenerWaypoint>& waypointBuffer = trajectory->GetWaypointBuffer();

        waypointBuffer.clear();
        waypointBuffer.reserve( iState->waypointSpacingBuffer.size() );

        for( int i = 0; i < iState->waypointSpacingBuffer.size(); i++ )
        {
            FInbetweenerWaypoint& waypoint = waypointBuffer.emplace_back( trajectory );

            waypoint.SetT( iState->waypointSpacingBuffer[i] );
        }
    }

    return true; // loading succeded
}

bool
FSnapshotTrajectory::LoadInitialState()
{
    return LoadState( &mInitialState );
}

bool
FSnapshotTrajectory::LoadAlteredState()
{
    return LoadState( &mAlteredState );
}

FSnapshotStep::~FSnapshotStep()
{
}

FSnapshotStep::FSnapshotStep( FInbetweenerStep* iStep )
    : mRoute( iStep->GetRoute() )
    , mIndex( iStep - &iStep->GetRoute()->GetStepBuffer()[0] )
{
    RecordLocalState( &mInitialState );
}

void
FSnapshotStep::RecordLocalState( FSnapshotStep::State* iState )
{
    if( iState->inited == false )
    {
        iState->aligned = mRoute->GetStepBuffer()[mIndex].IsAligned();

        iState->inited = true;
    }
}

bool
FSnapshotStep::LoadState( FSnapshotStep::State* iState )
{
    FInbetweenerStep* step = &mRoute->GetStepBuffer()[mIndex];

    step->SetAligned( iState->aligned );

    return true;
}

void
FSnapshotStep::RecordAlteredState()
{
    RecordLocalState( &mAlteredState );
}

bool
FSnapshotStep::LoadInitialState()
{
    return LoadState( &mInitialState );
}

bool
FSnapshotStep::LoadAlteredState()
{
    return LoadState( &mAlteredState );
}

FSnapshotRoute::~FSnapshotRoute()
{
}

FSnapshotRoute::FSnapshotRoute( FInbetweenerRoute* iRoute
                              , uint64 iSnapshotflags
                              , uint64 iTrajectorySnapshotFlags )
    : mRoute( iRoute )
    , mSnapshotFlags( iSnapshotflags ) // unused
{
    if( mSnapshotFlags & FSnapshotFlags::Route::TRAJECTORIES )
    {
        mTrajectorySnapshotBuffer.reserve( iRoute->GetInbetweenerTag()->GetBreakdownCount() );

        for( FInbetweenerTrajectory& trajectory : iRoute->GetTrajectoryBuffer() )
        {
            mTrajectorySnapshotBuffer.emplace_back( &trajectory, iTrajectorySnapshotFlags );
        }
    }

    if( mSnapshotFlags & FSnapshotFlags::Route::STEPS )
    {
        mStepSnapshotBuffer.reserve( iRoute->GetInbetweenerTag()->GetBreakdownCount() );

        for( FInbetweenerStep& step : iRoute->GetStepBuffer() )
        {
            mStepSnapshotBuffer.emplace_back( &step );
        }
    }
}

void
FSnapshotRoute::RecordAlteredState()
{
    if( mSnapshotFlags & FSnapshotFlags::Route::TRAJECTORIES )
    {
        for( FSnapshotTrajectory& trajectorySnapshot : mTrajectorySnapshotBuffer )
        {
            trajectorySnapshot.RecordAlteredState();
        }
    }

    if( mSnapshotFlags & FSnapshotFlags::Route::STEPS )
    {
        for( FSnapshotStep& stepSnapshot : mStepSnapshotBuffer )
        {
            stepSnapshot.RecordAlteredState();
        }
    }
}

bool
FSnapshotRoute::LoadInitialState()
{
    if( mSnapshotFlags & FSnapshotFlags::Route::TRAJECTORIES )
    {
        for( FSnapshotTrajectory& trajectorySnapshot : mTrajectorySnapshotBuffer )
        {
            trajectorySnapshot.LoadInitialState();
        }
    }

    if( mSnapshotFlags & FSnapshotFlags::Route::STEPS )
    {
        for( FSnapshotStep& stepSnapshot : mStepSnapshotBuffer )
        {
            stepSnapshot.LoadInitialState();
        }
    }

    return true;
}

bool
FSnapshotRoute::LoadAlteredState()
{
    if( mSnapshotFlags & FSnapshotFlags::Route::TRAJECTORIES )
    {
        for( FSnapshotTrajectory& trajectorySnapshot : mTrajectorySnapshotBuffer )
        {
            trajectorySnapshot.LoadAlteredState();
        }
    }

    if( mSnapshotFlags & FSnapshotFlags::Route::STEPS )
    {
        for( FSnapshotStep& stepSnapshot : mStepSnapshotBuffer )
        {
            stepSnapshot.LoadAlteredState();
        }
    }

    return true;
}

FSnapshotLayout::~FSnapshotLayout()
{
}

FSnapshotLayout::FSnapshotLayout()
{
}

void
FSnapshotLayout::RecordLocalState( FSnapshotLayout::State* iState )
{
    if( iState->inited == false )
    {
        uint32 breakdownCount = mInbetweenerTag->GetBreakdownCount();

        iState->breakdownArray.clear();
        iState->breakdownArray.reserve( breakdownCount );

        iState->targetBuffer.clear();
        iState->targetBuffer.reserve( breakdownCount );

        for( FInbetweenerBreakdown* breakdown : mInbetweenerTag->GetBreakdownList() )
        {
            iState->breakdownArray.emplace_back( breakdown );
            iState->targetBuffer.emplace_back( breakdown->GetTargetDrawingIndex() );
        }

        iState->inited = true;
    }
}

FSnapshotLayout::FSnapshotLayout( FOdysseyVectorTagInbetweener* iInbetweenerTag )
    : mInbetweenerTag ( iInbetweenerTag )
{
    RecordLocalState( &mInitialState );
}

void
FSnapshotLayout::RecordAlteredState()
{
    RecordLocalState( &mAlteredState );
}

bool
FSnapshotLayout::LoadState( FSnapshotLayout::State* iState )
{
    // erase all breakdowns except the default one
    mInbetweenerTag->ResetLayout( false );

    for( uint32 i = 0; i < iState->breakdownArray.size(); i++ )
    {
        FInbetweenerBreakdown* breakdown =  iState->breakdownArray[i];

        if( breakdown != mInbetweenerTag->GetMasterBreakdown() )
        {
            mInbetweenerTag->AddBreakdown( iState->breakdownArray[i]
                                         , iState->targetBuffer[i]
                                         , false );
        }
        else
        {
            breakdown->SetTargetDrawingIndex(  iState->targetBuffer[i] );
        }
    }

    return true;
}

bool
FSnapshotLayout::LoadInitialState()
{
    return LoadState( &mInitialState );
}

bool
FSnapshotLayout::LoadAlteredState()
{
    return LoadState( &mAlteredState );
}

FSnapshotDynamics::~FSnapshotDynamics()
{
    if( bApplied )
    {
        for( FInbetweenerRoute* route : mInitialState.routeArray )
        {
            if( route->GetInbetweenerTag() == nullptr )
            {
                delete route;
            }
        }
    }
    else
    {
        for( FInbetweenerRoute* route : mAlteredState.routeArray )
        {
            if( route->GetInbetweenerTag() == nullptr )
            {
                delete route;
            }
        }
    }
}

FSnapshotDynamics::FSnapshotDynamics()
{
}

FSnapshotDynamics::FSnapshotDynamics( FOdysseyVectorTagInbetweener* iInbetweenerTag )
    : mInbetweenerTag( iInbetweenerTag )
    , bApplied ( true )
{
    RecordLocalState( &mInitialState );
}

void
FSnapshotDynamics::RecordLocalState( FSnapshotDynamics::State* iState )
{
    if( iState->inited == false )
    {
        // Do not forget to clear as it could be called several times when calling Preswap()
        iState->routeArray.clear();
        iState->routeArray.reserve( mInbetweenerTag->GetRouteList().size() );

        for( FInbetweenerRoute* route : mInbetweenerTag->GetRouteList() )
        {
            iState->routeArray.push_back( route );
        }

        iState->inited = true;
    }
}

void
FSnapshotDynamics::RecordAlteredState()
{
    RecordLocalState( &mAlteredState );
}

bool
FSnapshotDynamics::LoadState( FSnapshotDynamics::State* iState )
{
    mInbetweenerTag->RemoveAllRoutes();

    for( FInbetweenerRoute* route : iState->routeArray )
    {
        mInbetweenerTag->AddRoute( route );
    }

    return true;
}

bool
FSnapshotDynamics::LoadInitialState()
{
    bApplied = false;

    return LoadState( &mInitialState );
}

bool
FSnapshotDynamics::LoadAlteredState()
{
    bApplied = true;

    return LoadState( &mAlteredState );
}

FSnapshotInbetweenerChart::~FSnapshotInbetweenerChart()
{
}

FSnapshotInbetweenerChart::FSnapshotInbetweenerChart( FInbetweenerChart* iChart
                                                    , uint64 iSnapshotFlags )
    : mChart( iChart )
    , mSnapshotFlags( iSnapshotFlags )
{
    RecordLocalState( &mInitialState );
}

void
FSnapshotInbetweenerChart::RecordLocalState( FSnapshotInbetweenerChart::State* iState )
{
    if( iState->inited == false )
    {
        uint32 drawingCount = mChart->GetBreakdown()->GetDrawingCount();

        // save bezier
        if( mSnapshotFlags & FSnapshotFlags::Chart::BEZIER )
        {
            iState->HUDBezier[0] = mChart->GetHUDBezier()[0];
            iState->HUDBezier[1] = mChart->GetHUDBezier()[1];
            iState->HUDBezier[2] = mChart->GetHUDBezier()[2];
        }

        // save spacing
        if( mSnapshotFlags & FSnapshotFlags::Chart::SPACING )
        {
            iState->spacing.resize( drawingCount );

            for( uint32 i = 0; i < drawingCount; i++ )
            {
                 iState->spacing[i] = mChart->GetDivisionArray()[i].spacing;
            }
        }

        iState->inited = true;
    }
}

void
FSnapshotInbetweenerChart::RecordAlteredState()
{
    RecordLocalState( &mAlteredState );
}

bool
FSnapshotInbetweenerChart::LoadState( FSnapshotInbetweenerChart::State* iState )
{
    // restore bezier
    if( mSnapshotFlags & FSnapshotFlags::Chart::BEZIER )
    {
        mChart->GetHUDBezier()[0] = iState->HUDBezier[0];
        mChart->GetHUDBezier()[1] = iState->HUDBezier[1];
        mChart->GetHUDBezier()[2] = iState->HUDBezier[2];
    }

    // restore spacing
    if( mSnapshotFlags & FSnapshotFlags::Chart::SPACING )
    {
        for( uint32 i = 0; i < iState->spacing.size(); i++ )
        {
            mChart->GetDivisionArray()[i].SetSpacing( iState->spacing[i] );
        }
    }

    return true;
}

bool
FSnapshotInbetweenerChart::LoadInitialState()
{
    return LoadState( &mInitialState );
}

bool
FSnapshotInbetweenerChart::LoadAlteredState()
{
    return LoadState( &mAlteredState );
}

FSnapshotInbetweenerBreakdown::~FSnapshotInbetweenerBreakdown()
{
}

void
FSnapshotInbetweenerBreakdown::RecordLocalState( FSnapshotInbetweenerBreakdown::State* iState )
{
    if( iState->inited == false )
    {
        if( mSnapshotFlags & FSnapshotFlags::Breakdown::GRIDGEOMETRY )
        {
            mBreakdown->GetGrid()->GetGeometry( iState->gridGeometry
                                              , eInbetweenerPointPositionType::TargetPosition );
        }

        if( mSnapshotFlags & FSnapshotFlags::Breakdown::TRANSFORMATIONS )
        {
            mBreakdown->GetTargetTransform( iState->translationX
                                          , iState->translationY
                                          , iState->rotation
                                          , iState->scalingX
                                          , iState->scalingY );
        }
    }
}

FSnapshotInbetweenerBreakdown::FSnapshotInbetweenerBreakdown( FInbetweenerBreakdown* iBreakdown
                                                            , uint64 iSnapshotFlags )
    : mBreakdown( iBreakdown )
    , mSnapshotFlags( iSnapshotFlags )
    , mChartSnapshot( iBreakdown->GetChart()
                    , ( mSnapshotFlags & FSnapshotFlags::Breakdown::CHART ) ? FSnapshotFlags::ALL 
                                                                            : 0  )
{
    RecordLocalState( &mInitialState );
}

void
FSnapshotInbetweenerBreakdown::RecordAlteredState()
{
    RecordLocalState( &mAlteredState );

    mChartSnapshot.RecordAlteredState();
}

bool
FSnapshotInbetweenerBreakdown::LoadState( FSnapshotInbetweenerBreakdown::State* iState )
{
    if( mSnapshotFlags & FSnapshotFlags::Breakdown::GRIDGEOMETRY )
    {
        mBreakdown->GetGrid()->SetGeometry( iState->gridGeometry
                                          , eInbetweenerPointPositionType::TargetPosition );
    }

    if( mSnapshotFlags & FSnapshotFlags::Breakdown::TRANSFORMATIONS )
    {
        mBreakdown->SetTargetTransform( iState->translationX
                                      , iState->translationY
                                      , iState->rotation
                                      , iState->scalingX
                                      , iState->scalingY );

        mBreakdown->UpdateMatrix();
    }

    return true; // restore succeeded
}

bool
FSnapshotInbetweenerBreakdown::LoadInitialState()
{
    LoadState( &mInitialState );

    if( mSnapshotFlags & FSnapshotFlags::Breakdown::CHART )
    {
        mChartSnapshot.LoadInitialState();
    }

    return true; // restore succeeded
}

bool
FSnapshotInbetweenerBreakdown::LoadAlteredState()
{
    LoadState( &mAlteredState );

    if( mSnapshotFlags & FSnapshotFlags::Breakdown::CHART )
    {
        mChartSnapshot.LoadAlteredState();
    }

    return true; // restore succeeded
}

FSnapshotTagInbetweener::~FSnapshotTagInbetweener()
{
}

void
FSnapshotTagInbetweener::RecordLocalState( FSnapshotTagInbetweener::State* iState )
{
    if( iState->inited == false )
    {
        iState->interpolationDirection = mInbetweenerTag->GetInterpolationDirection();

        if( ( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::GRIDSIZE )
         || ( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::GRIDTYPE ) )
        {
            iState->gridSizeX = mInbetweenerTag->GetGridNumQuadX();
            iState->gridSizeY = mInbetweenerTag->GetGridNumQuadY();
        }

        if( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::GRIDTYPE )
        {
            iState->gridType = mInbetweenerTag->GetGridType();
        }

        if( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::INTERPOLATIONTYPE )
        {
            iState->interpolationType = mInbetweenerTag->GetInterpolationType();
        }

        if( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::COLOR )
        {
            iState->color = mInbetweenerTag->GetColor();
        }

        if( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::MAPASPOLYLINE )
        {
            iState->mapAsPolyline = mInbetweenerTag->GetMapAsPolyline();
        }
    }
}

FSnapshotTagInbetweener::FSnapshotTagInbetweener( FOdysseyVectorTagInbetweener* iInbetweenerTag
                                                , uint64 iSnapshotFlags
                                                , uint64 iBreakdownSnapshotFlags
                                                , uint64 iRouteSnapshotFlags
                                                , uint64 iTrajectorySnapshotFlags )
    : mSnapshotFlags( iSnapshotFlags )
    , mInbetweenerTag( iInbetweenerTag )
    , mLayoutSnapshot() // Breakdown Layout
    , mDynamicsSnapshot() // Route Layout
{
    RecordLocalState( &mInitialState );

    // Snapshot Breakdown Layout
    if( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::BREAKDOWNS )
    {
        mLayoutSnapshot = FSnapshotLayout( mInbetweenerTag );
    }

    // Snapshot Breakdowns
    if( iBreakdownSnapshotFlags )
    {
        mInbetweenerBreakdownSnapshotBuffer.reserve( iInbetweenerTag->GetBreakdownCount() );

        for( FInbetweenerBreakdown* breakdown : iInbetweenerTag->GetBreakdownList() )
        {
            mInbetweenerBreakdownSnapshotBuffer.emplace_back( breakdown, iBreakdownSnapshotFlags );
        }
    }

    // Snapshot Route Layout
    if( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::ROUTES )
    {
        mDynamicsSnapshot = FSnapshotDynamics( mInbetweenerTag );
    }

    // Snapshot Routes
    if( iRouteSnapshotFlags )
    {
        mRouteSnapshotBuffer.reserve( mInbetweenerTag->GetRouteList().size() );

        // Adding or Removing a breakdown will affect routes. Restore them.
        for( FInbetweenerRoute* route : mInbetweenerTag->GetRouteList() )
        {
            mRouteSnapshotBuffer.emplace_back( route, iRouteSnapshotFlags, iTrajectorySnapshotFlags );
        }
    }
}

void
FSnapshotTagInbetweener::RecordAlteredState()
{
    RecordLocalState( &mAlteredState );

    // restore breakdown layout
    if( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::BREAKDOWNS )
    {
        mLayoutSnapshot.RecordAlteredState();
    }

    // restore breakdowns if any
    for( FSnapshotInbetweenerBreakdown& inbetweenerBreakdownSnapshot : mInbetweenerBreakdownSnapshotBuffer )
    {
        inbetweenerBreakdownSnapshot.RecordAlteredState();
    }

    // restore route layout
    if( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::ROUTES )
    {
        mDynamicsSnapshot.RecordAlteredState();
    }

    // restore routes if any
    for( FSnapshotRoute& routeSnapshot : mRouteSnapshotBuffer )
    {
        routeSnapshot.RecordAlteredState();
    }
}

bool
FSnapshotTagInbetweener::LoadLocalState( FSnapshotTagInbetweener::State* iState )
{
    mInbetweenerTag->SetInterpolationDirection( iState->interpolationDirection );

    if( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::MAPASPOLYLINE )
    {
        mInbetweenerTag->SetMapAsPolyline( iState->mapAsPolyline );
    }

    if( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::GRIDSIZE )
    {
        mInbetweenerTag->SetGridNumQuad( iState->gridSizeX
                                       , iState->gridSizeY );
    }

    if( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::GRIDTYPE )
    {
        mInbetweenerTag->SetGrid( iState->gridType
                                , iState->gridSizeX
                                , iState->gridSizeY );
    }

    if( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::INTERPOLATIONTYPE )
    {
        mInbetweenerTag->SetInterpolationType( iState->interpolationType );
    }

    if( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::COLOR )
    {
        mInbetweenerTag->SetColor( iState->color );
    }

    return true; // restore succeeded
}

bool
FSnapshotTagInbetweener::LoadInitialState()
{
    LoadLocalState( &mInitialState );

    // restore breakdown layout
    if( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::BREAKDOWNS )
    {
        mLayoutSnapshot.LoadInitialState();
    }

    // restore breakdowns if any
    for( FSnapshotInbetweenerBreakdown& inbetweenerBreakdownSnapshot : mInbetweenerBreakdownSnapshotBuffer )
    {
        inbetweenerBreakdownSnapshot.LoadInitialState();
    }

    // restore route layout
    if( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::ROUTES )
    {
        mDynamicsSnapshot.LoadInitialState();
    }

    // restore routes if any
    for( FSnapshotRoute& routeSnapshot : mRouteSnapshotBuffer )
    {
        routeSnapshot.LoadInitialState();
    }

    return true; // loading succeeded
}

bool
FSnapshotTagInbetweener::LoadAlteredState()
{
    LoadLocalState( &mAlteredState );

    // restore breakdown layout
    if( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::BREAKDOWNS )
    {
        mLayoutSnapshot.LoadAlteredState();
    }

    // restore breakdowns if any
    for( FSnapshotInbetweenerBreakdown& inbetweenerBreakdownSnapshot : mInbetweenerBreakdownSnapshotBuffer )
    {
        inbetweenerBreakdownSnapshot.LoadAlteredState();
    }

    // restore route layout
    if( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::ROUTES )
    {
        mDynamicsSnapshot.LoadAlteredState();
    }

    // restore routes if any
    for( FSnapshotRoute& routeSnapshot : mRouteSnapshotBuffer )
    {
        routeSnapshot.LoadAlteredState();
    }

    return true; // loading succeeded
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

        mPoint->Set( mCoords.x, mCoords.y );

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

    if( iSnapshotFlags & FSnapshotFlags::Point::Bucket::SPREADINGPOLICY )
    {
        mSpreadingPolicy = iBucket->GetSpreadingPolicy();
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

    if( iSnapshotFlags & FSnapshotFlags::Point::Bucket::PALETTEENTRY )
    {
        mPaletteEntry = iBucket->GetPaletteEntry();
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

    if( mSnapshotFlags & FSnapshotFlags::Point::Bucket::SPREADINGPOLICY )
    {
        eBucketSpreadingPolicy currentSpreadingPolicy = bucket->GetSpreadingPolicy();

        bucket->SetSpreadingPolicy( mSpreadingPolicy );
        // swap
        mSpreadingPolicy = currentSpreadingPolicy;
    }

    if( mSnapshotFlags & FSnapshotFlags::Point::Bucket::SOLIDCOLOR )
    {
        FColor currentSolidColor = bucket->GetSolidColor();

        bucket->SetSolidColor( mSolidColor );

        mSolidColor = currentSolidColor;
    }

    if( mSnapshotFlags & FSnapshotFlags::Point::Bucket::ROTATION )
    {
        double currentRotation = bucket->GetRotation();

        bucket->SetRotation( mRotation );
        // swap
        mRotation = currentRotation;
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

FSnapshotGroupPaint::~FSnapshotGroupPaint()
{
}

FSnapshotGroupPaint::FSnapshotGroupPaint( FOdysseyVectorGroupPaint* iPaintGroup
                                        , uint64 iSnapshotFlags )
    : FSnapshotObject( iPaintGroup, iSnapshotFlags )
{
    if( iSnapshotFlags & FSnapshotFlags::Object::GroupPaint::BUCKETS )
    {
        std::list<FOdysseyVectorBucket*>& bucketList = iPaintGroup->GetBucketList();

        mBucketSnapshotArray.reserve( bucketList.size() );

        for( FOdysseyVectorBucket* bucket : bucketList )
        {
            mBucketSnapshotArray.push_back( FSnapshotBucket( bucket, FSnapshotFlags::ALL ) );
        }
    }

    if( iSnapshotFlags & FSnapshotFlags::Object::GroupPaint::SELECTED_BUCKETS )
    {
        mSelectedBucketList = iPaintGroup->GetSelectedBucketList();
    }

    if( mSnapshotFlags & FSnapshotFlags::Object::GroupPaint::PAINTED )
    {
        bPainted = iPaintGroup->IsPainted();
    }

    if( mSnapshotFlags & FSnapshotFlags::Object::GroupPaint::MONOCHROME )
    {
        bMonochrome = iPaintGroup->IsMonochrome();
    }

    if( mSnapshotFlags & FSnapshotFlags::Object::GroupPaint::MONOCHROMECOLOR )
    {
        mMonochromeColor = iPaintGroup->GetMonochromeColor();
    }
    if( mSnapshotFlags & FSnapshotFlags::Object::GroupPaint::REALTIME )
    {
        bRealtime = iPaintGroup->IsRealtime();
    }

    if( mSnapshotFlags & FSnapshotFlags::Object::GroupPaint::GAPTOLERANCE )
    {
        mGapTolerance = iPaintGroup->GetGapTolerance();
    }

    if( mSnapshotFlags & FSnapshotFlags::Object::GroupPaint::WIREFRAME )
    {
        bWireframe = iPaintGroup->IsWireframe();
    }

    if( mSnapshotFlags & FSnapshotFlags::Object::GroupPaint::WIREFRAMECOLOR )
    {
        mWireframeColor = iPaintGroup->GetWireframeColor();
    }
}

bool
FSnapshotGroupPaint::Restore()
{
    if( FSnapshotObject::Restore() )
    {
        FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>(mObject);

        if( mSnapshotFlags & FSnapshotFlags::Object::GroupPaint::BUCKETS )
        {
            for( int i = 0; i < mBucketSnapshotArray.size(); i++ )
            {
                mBucketSnapshotArray[i].Restore();
            }
        }

        if( mSnapshotFlags & FSnapshotFlags::Object::GroupPaint::SELECTED_BUCKETS )
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

        if( mSnapshotFlags & FSnapshotFlags::Object::GroupPaint::PAINTED )
        {
            bool currentPainted = paintGroup->IsPainted();

            paintGroup->SetPainted( bPainted );
            // swap
            bPainted = currentPainted;
        }

        if( mSnapshotFlags & FSnapshotFlags::Object::GroupPaint::MONOCHROME )
        {
            bool currentMonochrome = paintGroup->IsMonochrome();

            paintGroup->SetMonochrome( bMonochrome );
            // swap
            bMonochrome = currentMonochrome;
        }

        if( mSnapshotFlags & FSnapshotFlags::Object::GroupPaint::MONOCHROMECOLOR )
        {
            FColor currentMonochromeColor = paintGroup->GetMonochromeColor();

            paintGroup->SetMonochromeColor( mMonochromeColor );
            // swap
            mMonochromeColor = currentMonochromeColor;
        }
        if( mSnapshotFlags & FSnapshotFlags::Object::GroupPaint::REALTIME )
        {
            bool currentRealtime = paintGroup->IsRealtime();

            paintGroup->SetRealtime( bRealtime );
            // swap
            bRealtime = currentRealtime;
        }

        if( mSnapshotFlags & FSnapshotFlags::Object::GroupPaint::GAPTOLERANCE )
        {
            double currentGapTolerance = paintGroup->GetGapTolerance();

            paintGroup->SetGapTolerance( mGapTolerance );
            // swap
            mGapTolerance = currentGapTolerance;
        }

        if( mSnapshotFlags & FSnapshotFlags::Object::GroupPaint::WIREFRAME )
        {
            bool currentWireframe = paintGroup->IsWireframe();

            paintGroup->SetWireframe( bWireframe );
            // swap
            bWireframe = currentWireframe;
        }

        if( mSnapshotFlags & FSnapshotFlags::Object::GroupPaint::WIREFRAMECOLOR )
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
