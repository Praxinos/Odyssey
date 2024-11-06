#include "Undo/OdysseyVectorUndoTagInbetweenerTrajectoryShiftWaypoint.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorTag.h"
#include "OdysseyVectorSharedEnv.h"

FOdysseyVectorUndoTagInbetweenerTrajectoryShiftWaypoint::~FOdysseyVectorUndoTagInbetweenerTrajectoryShiftWaypoint()
{
    if( mApplied )
    {
        // nothing to do
    }
    else
    {
        // nothing to do
    }
}

FOdysseyVectorUndoTagInbetweenerTrajectoryShiftWaypoint::FOdysseyVectorUndoTagInbetweenerTrajectoryShiftWaypoint( FOdysseyVectorGroupPaint* iScene
                                                                                                                , FInbetweenerTrajectory* iTrajectory
                                                                                                                , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iScene->GetSharedEnv(), iReturnFlags )
{
    GetEngineListFromObjectList( { iScene }, mEngineList );

    mTrajectorySnapshotBuffer.emplace_back( iTrajectory, FSnapshotFlags::Trajectory::WAYPOINTS );
}

void
FOdysseyVectorUndoTagInbetweenerTrajectoryShiftWaypoint::Apply( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    for( FSnapshotTrajectory& trajectorySnapshot : mTrajectorySnapshotBuffer )
    {
        trajectorySnapshot.LoadAlteredState();
    }

    // update invalidated objects
    mSharedEnv->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    // request redraw
    InvalidateEngineList( 0 );

    // call callbacks if any (for refreshing GUI e.g)
    FOdysseyVectorEngine::Notify( nullptr, mReturnFlags );
}

void
FOdysseyVectorUndoTagInbetweenerTrajectoryShiftWaypoint::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    for( FSnapshotTrajectory& trajectorySnapshot : mTrajectorySnapshotBuffer )
    {
        trajectorySnapshot.RecordAlteredState();
        trajectorySnapshot.LoadInitialState();
    }

    // update invalidated objects
    mSharedEnv->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    // request redraw
    InvalidateEngineList( 0 );

    // call callbacks if any (for refreshing GUI e.g)
    FOdysseyVectorEngine::Notify( nullptr, mReturnFlags );
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoTagInbetweenerTrajectoryShiftWaypoint::ToString() const
{
    return FString("FOdysseyVectorUndoTagInbetweenerTrajectoryShiftWaypoint");
}
