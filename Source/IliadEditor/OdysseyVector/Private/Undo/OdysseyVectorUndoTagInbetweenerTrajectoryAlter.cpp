// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "Undo/OdysseyVectorUndoTagInbetweenerTrajectoryAlter.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorTag.h"
#include "OdysseyVectorLayer.h"

FOdysseyVectorUndoTagInbetweenerTrajectoryAlter::~FOdysseyVectorUndoTagInbetweenerTrajectoryAlter()
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

FOdysseyVectorUndoTagInbetweenerTrajectoryAlter::FOdysseyVectorUndoTagInbetweenerTrajectoryAlter( FOdysseyVectorGroupPaint* iScene
                                                                                                , FInbetweenerTrajectory* iTrajectory
                                                                                                , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iScene->GetLayer(), iReturnFlags )
    , mTrajectorySnapshot( iTrajectory, FSnapshotFlags::Trajectory::BEZIER )
{
}

void
FOdysseyVectorUndoTagInbetweenerTrajectoryAlter::Apply( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    mTrajectorySnapshot.LoadAlteredState();

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
}

void
FOdysseyVectorUndoTagInbetweenerTrajectoryAlter::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    mTrajectorySnapshot.RecordAlteredState();
    mTrajectorySnapshot.LoadInitialState();

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoTagInbetweenerTrajectoryAlter::ToString() const
{
    return FString("FOdysseyVectorUndoTagInbetweenerTrajectoryAlter");
}
