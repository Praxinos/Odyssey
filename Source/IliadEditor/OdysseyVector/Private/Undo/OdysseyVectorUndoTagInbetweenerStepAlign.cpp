// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Undo/OdysseyVectorUndoTagInbetweenerStepAlign.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorTag.h"
#include "OdysseyVectorLayer.h"

FOdysseyVectorUndoTagInbetweenerStepAlign::~FOdysseyVectorUndoTagInbetweenerStepAlign()
{
    mRouteSnapshot.Clean( mApplied ? eSnapshotState::Altered : eSnapshotState::Initial );
}

FOdysseyVectorUndoTagInbetweenerStepAlign::FOdysseyVectorUndoTagInbetweenerStepAlign( FOdysseyVectorGroupPaint* iScene
                                                                                    , FOdysseyVectorTagInbetweener* iInbetweenerTag
                                                                                    , FInbetweenerRoute* iRoute )
    : FOdysseyVectorUndo( iScene->GetLayer() )
    , mInbetweenerTag( iInbetweenerTag )
    , mRouteSnapshot( iRoute
                    , FSnapshotFlags::Route::TRAJECTORIES | FSnapshotFlags::Route::STEPS
                    , FSnapshotFlags::Trajectory::BEZIER )
{
}

void
FOdysseyVectorUndoTagInbetweenerStepAlign::Begin()
{
    mRouteSnapshot.RecordState(  eSnapshotState::Initial );
}

void
FOdysseyVectorUndoTagInbetweenerStepAlign::End()
{
    mRouteSnapshot.RecordState(  eSnapshotState::Altered );
}

void
FOdysseyVectorUndoTagInbetweenerStepAlign::Apply( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    mRouteSnapshot.LoadState( eSnapshotState::Altered );

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
}

void
FOdysseyVectorUndoTagInbetweenerStepAlign::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    mRouteSnapshot.LoadState( eSnapshotState::Initial );

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoTagInbetweenerStepAlign::ToString() const
{
    return FString("FOdysseyVectorUndoTagInbetweenerStepAlign");
}
