#include "Undo/OdysseyVectorUndoTagInbetweenerStepAlign.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorTag.h"
#include "OdysseyVectorSharedEnv.h"

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
                                                                                    , FInbetweenerRoute* iRoute
                                                                                    , uint64 iReturnFlags )
    : FOdysseyVectorUndo( iScene, iReturnFlags )
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

    mRouteSnapshot.LoadAlteredState();

    // update invalidated objects
    mScene->GetSharedEnv()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // request redraw
    mScene->GetEngine()->Invalidate( 0 );
    // call callbacks if any (for refreshing GUI e.g)
    FOdysseyVectorEngine::Notify( mScene, mReturnFlags );
}

void
FOdysseyVectorUndoTagInbetweenerStepAlign::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    mRouteSnapshot.RecordAlteredState();
    mRouteSnapshot.LoadInitialState();

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
FOdysseyVectorUndoTagInbetweenerStepAlign::ToString() const
{
    return FString("FOdysseyVectorUndoTagInbetweenerStepAlign");
}
