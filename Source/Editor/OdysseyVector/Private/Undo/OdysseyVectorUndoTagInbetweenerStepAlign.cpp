#include "Undo/OdysseyVectorUndoTagInbetweenerStepAlign.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorTag.h"

FOdysseyVectorUndoTagInbetweenerStepAlign::~FOdysseyVectorUndoTagInbetweenerStepAlign()
{
    if( mApplied )
    {
        // nothing to do
    }
    else
    {

    }
}

FOdysseyVectorUndoTagInbetweenerStepAlign::FOdysseyVectorUndoTagInbetweenerStepAlign( FOdysseyVectorGroupPaint* iScene
                                                                                    , FOdysseyVectorTagInbetweener* iInbetweenerTag
                                                                                    , FInbetweenerRoute* iRoute )
    : FOdysseyVectorUndo( iScene )
    , mInbetweenerTag( iInbetweenerTag )
    , mRouteSnapshot( iRoute
                    , FSnapshotFlags::Route::TRAJECTORIES | FSnapshotFlags::Route::STEPS
                    , FSnapshotFlags::Trajectory::BEZIER )
{
}

void
FOdysseyVectorUndoTagInbetweenerStepAlign::Apply( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    mRouteSnapshot.Preswap();
    mRouteSnapshot.Restore();

    // update invalidated objects
    mScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                               | FOdysseyVectorEngine::SIGNAL_OBJECT_MODIFIED );
}

void
FOdysseyVectorUndoTagInbetweenerStepAlign::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    mRouteSnapshot.Preswap();
    mRouteSnapshot.Restore();

    // update invalidated objects
    mScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                               | FOdysseyVectorEngine::SIGNAL_OBJECT_MODIFIED );
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoTagInbetweenerStepAlign::ToString() const
{
    return FString("FOdysseyVectorUndoTagInbetweenerStepAlign");
}
