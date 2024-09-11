#include "Undo/OdysseyVectorUndoTagInbetweenerTrajectoryAlter.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorTag.h"

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
    : FOdysseyVectorUndo( iScene, iReturnFlags )
    , mTrajectorySnapshot( iTrajectory, FSnapshotFlags::Trajectory::BEZIER )
{
}

void
FOdysseyVectorUndoTagInbetweenerTrajectoryAlter::Apply( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    mTrajectorySnapshot.Preswap();
    mTrajectorySnapshot.Restore();

    // update invalidated objects
    mScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Invalidate( 0 );
    FOdysseyVectorEngine::Notify( mScene, mReturnFlags );
}

void
FOdysseyVectorUndoTagInbetweenerTrajectoryAlter::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    mTrajectorySnapshot.Preswap();
    mTrajectorySnapshot.Restore();

    // update invalidated objects
    mScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Invalidate( 0 );
    FOdysseyVectorEngine::Notify( mScene, mReturnFlags );
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoTagInbetweenerTrajectoryAlter::ToString() const
{
    return FString("FOdysseyVectorUndoTagInbetweenerTrajectoryAlter");
}
