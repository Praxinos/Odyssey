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

    if( mSnapshotFlags & FSnapshotFlags::Trajectory::BEZIER )
    {
        mHandleDirection[0] = iTrajectory->GetHandle(0)->GetDirection();
        mHandleDirection[1] = iTrajectory->GetHandle(1)->GetDirection();

        mHandleLengthRatio[0] = iTrajectory->GetHandle(0)->GetLengthRatio();
        mHandleLengthRatio[1] = iTrajectory->GetHandle(1)->GetLengthRatio();
    }

    if( mSnapshotFlags & FSnapshotFlags::Trajectory::WAYPOINTS )
    {
        WaypointSpacingToArray( iTrajectory, mWaypointSpacingBuffer );
    }
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
FSnapshotTrajectory::Preswap()
{
    FInbetweenerTrajectory* trajectory = &mRoute->GetTrajectoryBuffer()[mIndex];

    if( mSnapshotFlags & FSnapshotFlags::Trajectory::BEZIER )
    {
        mPreswapHandleDirection[0] = trajectory->GetHandle(0)->GetDirection();
        mPreswapHandleDirection[1] = trajectory->GetHandle(1)->GetDirection();
        mPreswapHandleLengthRatio[0] = trajectory->GetHandle(0)->GetLengthRatio();
        mPreswapHandleLengthRatio[1] = trajectory->GetHandle(1)->GetLengthRatio();
    }

    if( mSnapshotFlags & FSnapshotFlags::Trajectory::WAYPOINTS )
    {
        WaypointSpacingToArray( trajectory, mPreswapWaypointSpacingBuffer );
    }
}

void
FSnapshotTrajectory::Restore()
{
    FInbetweenerTrajectory* trajectory = &mRoute->GetTrajectoryBuffer()[mIndex];

    if( mSnapshotFlags & FSnapshotFlags::Trajectory::BEZIER )
    {
        trajectory->GetHandle(0)->Set( mHandleDirection[0], mHandleLengthRatio[0] );
        trajectory->GetHandle(1)->Set( mHandleDirection[1], mHandleLengthRatio[1] );

        mHandleDirection[0] = mPreswapHandleDirection[0];
        mHandleDirection[1] = mPreswapHandleDirection[1];

        mHandleLengthRatio[0] = mPreswapHandleLengthRatio[0];
        mHandleLengthRatio[1] = mPreswapHandleLengthRatio[1];
    }

    if( mSnapshotFlags & FSnapshotFlags::Trajectory::WAYPOINTS )
    {
        std::vector<FInbetweenerWaypoint>& waypointBuffer = trajectory->GetWaypointBuffer();

        waypointBuffer.clear();
        waypointBuffer.reserve( mWaypointSpacingBuffer.size() );

        for( int i = 0; i < mWaypointSpacingBuffer.size(); i++ )
        {
            FInbetweenerWaypoint& waypoint = waypointBuffer.emplace_back( trajectory );

            waypoint.SetT( mWaypointSpacingBuffer[i] );
        }

        mWaypointSpacingBuffer = mPreswapWaypointSpacingBuffer;
    }
}

FSnapshotStep::~FSnapshotStep()
{
}

FSnapshotStep::FSnapshotStep( FInbetweenerStep* iStep )
    : mRoute( iStep->GetRoute() )
    , mIndex( iStep - &iStep->GetRoute()->GetStepBuffer()[0] )
    , bIsAligned( iStep->IsAligned() )
{
}

bool
FSnapshotStep::Restore()
{
    FInbetweenerStep* step = &mRoute->GetStepBuffer()[mIndex];
    bool swapAligned = step->IsAligned();

    step->SetAligned( bIsAligned );

    bIsAligned = swapAligned;

    return true;
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
FSnapshotRoute::Preswap()
{
    if( mSnapshotFlags & FSnapshotFlags::Route::TRAJECTORIES )
    {
        mPreswapTrajectorySnapshotBuffer.clear();
        mPreswapTrajectorySnapshotBuffer.reserve( mRoute->GetInbetweenerTag()->GetBreakdownCount() );

        for( FInbetweenerTrajectory& trajectory : mRoute->GetTrajectoryBuffer() )
        {
            mPreswapTrajectorySnapshotBuffer.emplace_back( &trajectory, mTrajectorySnapshotFlags );
        }
    }

    if( mSnapshotFlags & FSnapshotFlags::Route::STEPS )
    {
        mPreswapStepSnapshotBuffer.clear();
        mPreswapStepSnapshotBuffer.reserve( mRoute->GetInbetweenerTag()->GetBreakdownCount() );

        for( FInbetweenerStep& step : mRoute->GetStepBuffer() )
        {
            mPreswapStepSnapshotBuffer.emplace_back( &step );
        }
    }
}

bool
FSnapshotRoute::Restore()
{
    if( mSnapshotFlags & FSnapshotFlags::Route::TRAJECTORIES )
    {
        for( FSnapshotTrajectory& trajectorySnapshot : mTrajectorySnapshotBuffer )
        {
            trajectorySnapshot.Restore();
        }

        mTrajectorySnapshotBuffer = mPreswapTrajectorySnapshotBuffer;
    }

    if( mSnapshotFlags & FSnapshotFlags::Route::STEPS )
    {
        for( FSnapshotStep& stepSnapshot : mStepSnapshotBuffer )
        {
            stepSnapshot.Restore();
        }

        mStepSnapshotBuffer = mPreswapStepSnapshotBuffer;
    }

    return true;
}

FSnapshotInbetweenerBreakdown::~FSnapshotInbetweenerBreakdown()
{
}

FSnapshotInbetweenerBreakdown::FSnapshotInbetweenerBreakdown( FInbetweenerBreakdown* iBreakdown
                                                            , uint64 iSnapshotFlags )
    : mBreakdown( iBreakdown )
    , mSnapshotFlags( iSnapshotFlags )
{
    if( mSnapshotFlags & FSnapshotFlags::Breakdown::GRIDGEOMETRY )
    {
        mBreakdown->GetGrid()->GetGeometry( mGridGeometry
                                          , eInbetweenerPointPositionType::TargetPosition );
    }

    if( mSnapshotFlags & FSnapshotFlags::Breakdown::TRANSFORMATIONS )
    {
        mBreakdown->GetTargetTransform( mTranslationX
                                      , mTranslationY
                                      , mRotation
                                      , mScalingX
                                      , mScalingY );
    }
}

void
FSnapshotInbetweenerBreakdown::Preswap()
{
    if( mSnapshotFlags & FSnapshotFlags::Breakdown::GRIDGEOMETRY )
    {
        mBreakdown->GetGrid()->GetGeometry( mPreswapGridGeometry
                                          , eInbetweenerPointPositionType::TargetPosition );
    }

    if( mSnapshotFlags & FSnapshotFlags::Breakdown::TRANSFORMATIONS )
    {
        mBreakdown->GetTargetTransform( mPreswapTranslationX
                                      , mPreswapTranslationY
                                      , mPreswapRotation
                                      , mPreswapScalingX
                                      , mPreswapScalingY );
    }
}

bool
FSnapshotInbetweenerBreakdown::Restore()
{
    if( mSnapshotFlags & FSnapshotFlags::Breakdown::GRIDGEOMETRY )
    {
        mBreakdown->GetGrid()->SetGeometry( mGridGeometry   , eInbetweenerPointPositionType::TargetPosition );

        mGridGeometry = mPreswapGridGeometry;
    }

    if( mSnapshotFlags & FSnapshotFlags::Breakdown::TRANSFORMATIONS )
    {
        mBreakdown->SetTargetTransform( mTranslationX
                                      , mTranslationY
                                      , mRotation
                                      , mScalingX
                                      , mScalingY );

        mBreakdown->UpdateMatrix();

        mTranslationX = mPreswapTranslationX;
        mTranslationY = mPreswapTranslationY;
        mRotation     = mPreswapRotation;
        mScalingX     = mPreswapScalingX;
        mScalingY     = mPreswapScalingY;
    }

    return true; // restore succeeded
}

FSnapshotLayout::~FSnapshotLayout()
{
}

FSnapshotLayout::FSnapshotLayout()
{
}

FSnapshotLayout::FSnapshotLayout( FOdysseyVectorTagInbetweener* iInbetweenerTag )
    : mInbetweenerTag ( iInbetweenerTag )
{
    uint32 breakdownCount = mInbetweenerTag->GetBreakdownCount();

    mBreakdownArray.reserve( breakdownCount );
    mTargetBuffer.reserve( breakdownCount );

    for( FInbetweenerBreakdown* breakdown : mInbetweenerTag->GetBreakdownList() )
    {
        mBreakdownArray.emplace_back( breakdown );
        mTargetBuffer.emplace_back( breakdown->GetTargetDrawingIndex() );
    }
}

void
FSnapshotLayout::Preswap()
{
    uint32 breakdownCount = mInbetweenerTag->GetBreakdownCount();

    mPreswapBreakdownArray.clear();
    mPreswapBreakdownArray.reserve( breakdownCount );

    mPreswapTargetBuffer.clear();
    mPreswapTargetBuffer.reserve( breakdownCount );

    for( FInbetweenerBreakdown* breakdown : mInbetweenerTag->GetBreakdownList() )
    {
        mPreswapBreakdownArray.emplace_back( breakdown );
        mPreswapTargetBuffer.emplace_back( breakdown->GetTargetDrawingIndex() );
    }
}

bool
FSnapshotLayout::Restore()
{
    mInbetweenerTag->ResetLayout( false );

    for( uint32 i = 0; i < mBreakdownArray.size(); i++ )
    {
        FInbetweenerBreakdown* breakdown = mBreakdownArray[i];

        if( breakdown != mInbetweenerTag->GetMasterBreakdown() )
        {
            mInbetweenerTag->AddBreakdown( mBreakdownArray[i], mTargetBuffer[i], false );
        }
        else
        {
            breakdown->SetTargetDrawingIndex( mTargetBuffer[i] );
        }
    }

    mBreakdownArray = mPreswapBreakdownArray;
    mTargetBuffer = mPreswapTargetBuffer;

    return true;
}

FSnapshotDynamics::~FSnapshotDynamics()
{
}

FSnapshotDynamics::FSnapshotDynamics()
{
}

FSnapshotDynamics::FSnapshotDynamics( FOdysseyVectorTagInbetweener* iInbetweenerTag )
    : mInbetweenerTag( iInbetweenerTag )
{
    mRouteArray.reserve( mInbetweenerTag->GetRouteList().size() );

    for( FInbetweenerRoute* route : mInbetweenerTag->GetRouteList() )
    {
        mRouteArray.push_back( route );
    }
}

void
FSnapshotDynamics::Preswap()
{
    mPreswapRouteArray.clear();
    mPreswapRouteArray.reserve( mInbetweenerTag->GetRouteList().size() );

    for( FInbetweenerRoute* route : mInbetweenerTag->GetRouteList() )
    {
        mPreswapRouteArray.push_back( route );
    }
}

bool
FSnapshotDynamics::Restore()
{
    mInbetweenerTag->RemoveAllRoutes();

    for( FInbetweenerRoute* route : mRouteArray )
    {
        mInbetweenerTag->AddRoute( route );
    }

    return true;
}

FSnapshotTagInbetweener::~FSnapshotTagInbetweener()
{
}

static void
BuildBreakdownSnapshotBuffer( FOdysseyVectorTagInbetweener* iInbetweenerTag
                            , std::vector<FSnapshotInbetweenerBreakdown>& oBreakdownSnapshotBuffer
                            , uint64 iBreakdownSnapshotFlags )
{
    // Adding or Removing a breakdown will affect routes. Backup them.
    oBreakdownSnapshotBuffer.clear();
    oBreakdownSnapshotBuffer.reserve( iInbetweenerTag->GetBreakdownCount() );

    for( FInbetweenerBreakdown* breakdown : iInbetweenerTag->GetBreakdownList() )
    {
        oBreakdownSnapshotBuffer.emplace_back( breakdown, iBreakdownSnapshotFlags );
    }
}

FSnapshotTagInbetweener::FSnapshotTagInbetweener( FOdysseyVectorTagInbetweener* iInbetweenerTag
                                                , uint64 iSnapshotFlags
                                                , uint64 iBreakdownSnapshotFlags
                                                , uint64 iRouteSnapshotFlags )
    : mSnapshotFlags( iSnapshotFlags )
    , mInbetweenerTag( iInbetweenerTag )
    , mBreakdownSnapshotFlags( iBreakdownSnapshotFlags )
    , mRouteSnapshotFlags( iRouteSnapshotFlags )
    , mLayout()
    , mDynamics()
    , mChart( iInbetweenerTag->GetMasterBreakdown() )
    , mPreswapChart( iInbetweenerTag->GetMasterBreakdown() )
{
    /*-------------------------  Backup Mapping Type ----------------------- */
/*
    if( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::DRAWINGCOUNT )
    {
        mDrawingCount = mInbetweenerTag->GetDrawingCount();
    }
*/

    /*-------------------------  Backup Mapping Type ----------------------- */
    if( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::MAPASPOLYLINE )
    {
        bMapAsPolyline = mInbetweenerTag->GetMapAsPolyline();
    }

    /*----------------------------  Backup Grid Size ----------------------- */
    if( ( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::GRIDSIZE )
     || ( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::GRIDTYPE ) )
    {
        mGridSizeX = mInbetweenerTag->GetGridNumQuadX();
        mGridSizeY = mInbetweenerTag->GetGridNumQuadY();
    }

    /*----------------------------  Backup Grid Type ----------------------- */
    if( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::GRIDTYPE )
    {
        mGridType = mInbetweenerTag->GetGridType();
    }

    if( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::ARAPRIGIDITY )
    {
/*--------
        FInbetweenerGridARAP* arapGrid = static_cast<FInbetweenerGridARAP*>(mInbetweenerTag->GetGrid());

        mARAPRigidity = arapGrid->GetRigidity();
*/
    }

    /* ---------------------- Backup Interpolation Type -------------------- */
    if( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::INTERPOLATIONTYPE )
    {
        mInterpolationType = mInbetweenerTag->GetInterpolationType();
    }

    /* ---------------------------- Backup Color --------------------------- */
    if( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::COLOR )
    {
        mColor = mInbetweenerTag->GetColor();
    }

    /* ---------------------- Backup Breakdown Layout ---------------------- */
    if( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::BREAKDOWNS )
    {
        mLayout = FSnapshotLayout( mInbetweenerTag );
    }

    /* ----------------------- Backup Breakdowns --------------------------- */
    if( mBreakdownSnapshotFlags )
    {
        for( FInbetweenerBreakdown* breakdown : iInbetweenerTag->GetBreakdownList() )
        {
            mInbetweenerBreakdownSnapshotBuffer.emplace_back( breakdown, mBreakdownSnapshotFlags );
        }
    }

    /* ---------------------- Backup Route Layout ---------------------- */
    if( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::ROUTES )
    {
        mDynamics = FSnapshotDynamics( mInbetweenerTag );
    }

    /* ----------------------- Backup Chart -------------------------------- */
    if( ( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::BREAKDOWNS )
     || ( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::CHART ) )
    {
        //mChart = mInbetweenerTag->GetChart();
    }
}

void
FSnapshotTagInbetweener::Preswap()
{
    if( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::MAPASPOLYLINE )
    {
        bPreswapMapAsPolyline = mInbetweenerTag->GetMapAsPolyline();
    }

    if( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::GRIDSIZE )
    {
        mPreswapGridSizeX = mInbetweenerTag->GetGridNumQuadX();
        mPreswapGridSizeY = mInbetweenerTag->GetGridNumQuadY();
    }

    if( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::GRIDTYPE )
    {
        mPreswapGridType = mInbetweenerTag->GetGridType();
    }

    if( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::INTERPOLATIONTYPE )
    {
        mPreswapInterpolationType = mInbetweenerTag->GetInterpolationType();
    }

    if( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::COLOR )
    {
        mPreswapColor = mInbetweenerTag->GetColor();
    }

    if( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::BREAKDOWNS )
    {
        mLayout.Preswap(); // will prepare values to be swapped
    }

    if( mBreakdownSnapshotFlags )
    {
        for( FSnapshotInbetweenerBreakdown& inbetweenerBreakdownSnapshot : mInbetweenerBreakdownSnapshotBuffer )
        {
            inbetweenerBreakdownSnapshot.Preswap();// will prepare values to be swapped
        }
    }

    if( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::ROUTES )
    {
        mDynamics.Preswap(); // will prepare values to be swapped
    }

    if( mRouteSnapshotFlags )
    {
        // Adding or Removing a breakdown will affect routes. Restore them.
        for( FSnapshotRoute& routeSnapshot : mRouteSnapshotBuffer )
        {
            routeSnapshot.Preswap();
        }
    }

    if( ( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::BREAKDOWNS )
    ||  ( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::CHART          ) )
    {
        //mPreswapChart = mInbetweenerTag->GetChart();
    }
}

bool
FSnapshotTagInbetweener::Restore()
{
    if( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::MAPASPOLYLINE )
    {
        mInbetweenerTag->SetMapAsPolyline( bMapAsPolyline );

        bMapAsPolyline = bPreswapMapAsPolyline;
    }

    if( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::GRIDSIZE )
    {
        mInbetweenerTag->SetGridNumQuad( mGridSizeX, mGridSizeY );

        mGridSizeX = mPreswapGridSizeX;
        mGridSizeY = mPreswapGridSizeY;
    }

    if( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::GRIDTYPE )
    {
        mInbetweenerTag->SetGrid( mGridType, mGridSizeX, mGridSizeY );

        mGridType = mPreswapGridType;
    }

    if( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::INTERPOLATIONTYPE )
    {
        mInbetweenerTag->SetInterpolationType( mInterpolationType );

        mInterpolationType = mPreswapInterpolationType;
    }

    if( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::ARAPRIGIDITY )
    {
/*---------------
        FInbetweenerGridARAP* arapGrid = static_cast<FInbetweenerGridARAP*>(mInbetweenerTag->GetGrid());
        uint32 swapARAPRigidity = arapGrid->GetRigidity();

        arapGrid->SetRigidity( mARAPRigidity );

        mARAPRigidity = swapARAPRigidity;
*/
    }

    if( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::COLOR )
    {
        mInbetweenerTag->SetColor( mColor );

        mColor = mPreswapColor;
    }

    // restore layout (breakdown range)
    if( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::BREAKDOWNS )
    {
        mLayout.Restore(); // will swap values with preswap ones
    }

    if( mBreakdownSnapshotFlags )
    {
        std::vector<FSnapshotInbetweenerBreakdown> swapBreakdownSnapshotBuffer;

        for( FSnapshotInbetweenerBreakdown& inbetweenerBreakdownSnapshot : mInbetweenerBreakdownSnapshotBuffer )
        {
            inbetweenerBreakdownSnapshot.Restore(); // will swap values with preswap ones
        }
    }

    // restore layout (breakdown range)
    if( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::ROUTES )
    {
        mDynamics.Restore(); // will swap values with preswap ones
    }

    if( mRouteSnapshotFlags )
    {
        // Adding or Removing a breakdown will affect routes. Restore them.
        for( FSnapshotRoute& routeSnapshot : mRouteSnapshotBuffer )
        {
            routeSnapshot.Restore();
        }
    }

    // restore chart after params have been set
    if( ( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::BREAKDOWNS )
    ||  ( mSnapshotFlags & FSnapshotFlags::Tag::Inbetweener::CHART      ) )
    {
        //mInbetweenerTag->SetChart( mChart );

        //mChart = mPreswapChart;
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
