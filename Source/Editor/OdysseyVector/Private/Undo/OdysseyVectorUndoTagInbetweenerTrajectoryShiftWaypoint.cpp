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
                                                                                                                , FInbetweenerTrajectory* iTrajectory )
    : FOdysseyVectorUndo( iScene )
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
        trajectorySnapshot.Restore();
    }

    // update invalidated objects
    mScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                               | FOdysseyVectorEngine::SIGNAL_OBJECT_MODIFIED );
}

void
FOdysseyVectorUndoTagInbetweenerTrajectoryShiftWaypoint::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    for( FSnapshotTrajectory& trajectorySnapshot : mTrajectorySnapshotBuffer )
    {
        trajectorySnapshot.Restore();
    }

    // update invalidated objects
    mScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                               | FOdysseyVectorEngine::SIGNAL_OBJECT_MODIFIED );
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoTagInbetweenerTrajectoryShiftWaypoint::ToString() const
{
    return FString("FOdysseyVectorUndoTagInbetweenerTrajectoryShiftWaypoint");
}
