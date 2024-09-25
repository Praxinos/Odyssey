#include "Undo/OdysseyVectorUndoTagInbetweenerBreakdownAlter.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorTag.h"
#include "OdysseyVectorSharedEnv.h"

FOdysseyVectorUndoTagInbetweenerBreakdownAlter::~FOdysseyVectorUndoTagInbetweenerBreakdownAlter()
{
    if( mApplied )
    {
        // nothing to do
    }
    else
    {

    }
}

FOdysseyVectorUndoTagInbetweenerBreakdownAlter::FOdysseyVectorUndoTagInbetweenerBreakdownAlter( FOdysseyVectorGroupPaint* iScene
                                                                                              , FOdysseyVectorTagInbetweener* iInbetweenerTag
                                                                                              , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iScene, iReturnFlags )
    , mInbetweenerTagSnapshot( iInbetweenerTag
                             , FSnapshotFlags::Tag::Inbetweener::BREAKDOWNS
                             , FSnapshotFlags::Breakdown::GRIDGEOMETRY
                             , FSnapshotFlags::Route::TRAJECTORIES
                             | FSnapshotFlags::Route::STEPS
                             , FSnapshotFlags::Trajectory::BEZIER
                             | FSnapshotFlags::Trajectory::WAYPOINTS )
{
}

void
FOdysseyVectorUndoTagInbetweenerBreakdownAlter::Apply( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    mInbetweenerTagSnapshot.LoadAlteredState();

    // update invalidated objects
    mScene->GetSharedEnv()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // request redraw
    mScene->GetEngine()->Invalidate( 0 );
    // call callbacks if any (for refreshing GUI e.g)
    FOdysseyVectorEngine::Notify( mScene, mReturnFlags );
}

void
FOdysseyVectorUndoTagInbetweenerBreakdownAlter::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    mInbetweenerTagSnapshot.RecordAlteredState();
    mInbetweenerTagSnapshot.LoadInitialState();

    // update invalidated objects
    mScene->GetSharedEnv()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // request redraw
    mScene->GetEngine()->Invalidate( 0 );
    // call callbacks if any (for refreshing GUI e.g)
    FOdysseyVectorEngine::Notify( mScene, mReturnFlags );
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoTagInbetweenerBreakdownAlter::ToString() const
{
    return FString("FOdysseyVectorUndoTagInbetweenerBreakdownAlter");
}
