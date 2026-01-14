// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Undo/OdysseyVectorUndoTagInbetweenerTrajectoryShiftWaypoint.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorTag.h"
#include "OdysseyVectorLayer.h"

FOdysseyVectorUndoTagInbetweenerTrajectoryShiftWaypoint::~FOdysseyVectorUndoTagInbetweenerTrajectoryShiftWaypoint()
{
    mTrajectorySnapshot.Clean( mApplied ? eSnapshotState::Altered : eSnapshotState::Initial );
}

FOdysseyVectorUndoTagInbetweenerTrajectoryShiftWaypoint::FOdysseyVectorUndoTagInbetweenerTrajectoryShiftWaypoint( FOdysseyVectorGroupPaint* iScene
                                                                                                                , FInbetweenerTrajectory* iTrajectory )
    : FOdysseyVectorUndo( iScene->GetLayer() )
    , mTrajectorySnapshot ( iTrajectory, FSnapshotFlags::Trajectory::WAYPOINTS )
{
}

void
FOdysseyVectorUndoTagInbetweenerTrajectoryShiftWaypoint::Begin()
{
    mTrajectorySnapshot.RecordState(  eSnapshotState::Initial );
}

void
FOdysseyVectorUndoTagInbetweenerTrajectoryShiftWaypoint::End()
{
    mTrajectorySnapshot.RecordState(  eSnapshotState::Altered );
}

void
FOdysseyVectorUndoTagInbetweenerTrajectoryShiftWaypoint::Apply( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    mTrajectorySnapshot.LoadState( eSnapshotState::Altered );

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
}

void
FOdysseyVectorUndoTagInbetweenerTrajectoryShiftWaypoint::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    mTrajectorySnapshot.LoadState( eSnapshotState::Initial );

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoTagInbetweenerTrajectoryShiftWaypoint::ToString() const
{
    return FString("FOdysseyVectorUndoTagInbetweenerTrajectoryShiftWaypoint");
}
