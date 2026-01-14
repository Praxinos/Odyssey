// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Undo/OdysseyVectorUndoTagInbetweenerTrajectoryAlter.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorTag.h"
#include "OdysseyVectorLayer.h"

FOdysseyVectorUndoTagInbetweenerTrajectoryAlter::~FOdysseyVectorUndoTagInbetweenerTrajectoryAlter()
{
    mTrajectorySnapshot.Clean( mApplied ? eSnapshotState::Altered : eSnapshotState::Initial );
}

FOdysseyVectorUndoTagInbetweenerTrajectoryAlter::FOdysseyVectorUndoTagInbetweenerTrajectoryAlter( FOdysseyVectorGroupPaint* iScene
                                                                                                , FInbetweenerTrajectory* iTrajectory )
    : FOdysseyVectorUndo( iScene->GetLayer() )
    , mTrajectorySnapshot( iTrajectory, FSnapshotFlags::Trajectory::BEZIER )
{
    mTrajectorySnapshot.RecordState( eSnapshotState::Initial );
}

void
FOdysseyVectorUndoTagInbetweenerTrajectoryAlter::Apply( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    mTrajectorySnapshot.LoadState( eSnapshotState::Altered );

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
}

void
FOdysseyVectorUndoTagInbetweenerTrajectoryAlter::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    mTrajectorySnapshot.RecordState( eSnapshotState::Altered );
    mTrajectorySnapshot.LoadState( eSnapshotState::Initial );

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoTagInbetweenerTrajectoryAlter::ToString() const
{
    return FString("FOdysseyVectorUndoTagInbetweenerTrajectoryAlter");
}
