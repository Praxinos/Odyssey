#include "Undo/OdysseyVectorUndoTagInbetweenerBreakdownAlter.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorTag.h"

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
                                                                                              , FOdysseyVectorTagInbetweener* iInbetweenerTag )
    : FOdysseyVectorUndo( iScene )
    , mInbetweenerTagSnapshot( iInbetweenerTag
                             , FSnapshotFlags::Tag::Inbetweener::BREAKDOWNS
                             , FSnapshotFlags::Breakdown::GRIDGEOMETRY
                             , FSnapshotFlags::Route::TRAJECTORIES
                             | FSnapshotFlags::Route::STEPS )
{
}

void
FOdysseyVectorUndoTagInbetweenerBreakdownAlter::Apply( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    mInbetweenerTagSnapshot.Preswap();
    mInbetweenerTagSnapshot.Restore();

    // update invalidated objects
    mScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                               | FOdysseyVectorEngine::SIGNAL_OBJECT_MODIFIED );
}

void
FOdysseyVectorUndoTagInbetweenerBreakdownAlter::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    mInbetweenerTagSnapshot.Preswap();
    mInbetweenerTagSnapshot.Restore();

    // update invalidated objects
    mScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                               | FOdysseyVectorEngine::SIGNAL_OBJECT_MODIFIED );
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoTagInbetweenerBreakdownAlter::ToString() const
{
    return FString("FOdysseyVectorUndoTagInbetweenerBreakdownAlter");
}
