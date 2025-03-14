// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "Undo/OdysseyVectorUndoTagInbetweenerTrajectoryShiftWaypoint.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorTag.h"
#include "OdysseyVectorLayer.h"

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
    : FOdysseyVectorUndo( iScene->GetLayer(), iReturnFlags )
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

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
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

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoTagInbetweenerTrajectoryShiftWaypoint::ToString() const
{
    return FString("FOdysseyVectorUndoTagInbetweenerTrajectoryShiftWaypoint");
}
