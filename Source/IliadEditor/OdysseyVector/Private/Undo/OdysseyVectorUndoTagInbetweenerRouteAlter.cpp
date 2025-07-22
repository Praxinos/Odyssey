// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Undo/OdysseyVectorUndoTagInbetweenerRouteAlter.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorTag.h"
#include "OdysseyVectorLayer.h"

FOdysseyVectorUndoTagInbetweenerRouteAlter::~FOdysseyVectorUndoTagInbetweenerRouteAlter()
{
    if( mApplied )
    {
        // nothing to do
    }
    else
    {

    }
}

FOdysseyVectorUndoTagInbetweenerRouteAlter::FOdysseyVectorUndoTagInbetweenerRouteAlter( FOdysseyVectorGroupPaint* iScene
                                                                                      , FOdysseyVectorTagInbetweener* iInbetweenerTag
                                                                                      , FInbetweenerRoute* iRoute
                                                                                      , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iScene->GetLayer(), iReturnFlags )
    , mInbetweenerTag( iInbetweenerTag )
    , mRouteSnapshot( iRoute
                    , ( FSnapshotFlags::Route::TRAJECTORIES
                      | FSnapshotFlags::Route::STEPS )
                    , ( FSnapshotFlags::Trajectory::BEZIER
                      | FSnapshotFlags::Trajectory::WAYPOINTS )
                    , eSnapshotState::Initial )
{
}

void
FOdysseyVectorUndoTagInbetweenerRouteAlter::Apply( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    mRouteSnapshot.LoadState( eSnapshotState::Altered );

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
}

void
FOdysseyVectorUndoTagInbetweenerRouteAlter::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    mRouteSnapshot.RecordState( eSnapshotState::Altered );
    mRouteSnapshot.LoadState( eSnapshotState::Initial );

    // Update vector scenes and call callbacks if any (for refreshing GUI e.g)
    Update();
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoTagInbetweenerRouteAlter::ToString() const
{
    return FString("FOdysseyVectorUndoTagInbetweenerRouteAlter");
}
