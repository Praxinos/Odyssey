#include "Undo/OdysseyVectorUndoTagInbetweenerTrajectoryShiftWaypoint.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorTag.h"

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
    : FOdysseyVectorUndo( iScene, iReturnFlags )
{
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
    mScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // request redraw
    mScene->GetEngine()->Invalidate( 0 );
    // call callbacks if any (for refreshing GUI e.g)
    FOdysseyVectorEngine::Notify( mScene, mReturnFlags );
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
    mScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // request redraw
    mScene->GetEngine()->Invalidate( 0 );
    // call callbacks if any (for refreshing GUI e.g)
    FOdysseyVectorEngine::Notify( mScene, mReturnFlags );
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoTagInbetweenerTrajectoryShiftWaypoint::ToString() const
{
    return FString("FOdysseyVectorUndoTagInbetweenerTrajectoryShiftWaypoint");
}
