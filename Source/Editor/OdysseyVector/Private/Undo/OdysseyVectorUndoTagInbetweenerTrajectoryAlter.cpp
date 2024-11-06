#include "Undo/OdysseyVectorUndoTagInbetweenerTrajectoryAlter.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorTag.h"
#include "OdysseyVectorSharedEnv.h"

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
    : FOdysseyVectorUndo( iScene->GetSharedEnv(), iReturnFlags )
    , mTrajectorySnapshot( iTrajectory, FSnapshotFlags::Trajectory::BEZIER )
{
    GetEngineListFromObjectList( { iScene }, mEngineList );
}

void
FOdysseyVectorUndoTagInbetweenerTrajectoryAlter::Apply( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    mTrajectorySnapshot.LoadAlteredState();

    // update invalidated objects
    mSharedEnv->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    // request redraw
    InvalidateEngineList( 0 );

    // call callbacks if any (for refreshing GUI e.g)
    FOdysseyVectorEngine::Notify( nullptr, mReturnFlags );
}

void
FOdysseyVectorUndoTagInbetweenerTrajectoryAlter::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    mTrajectorySnapshot.RecordAlteredState();
    mTrajectorySnapshot.LoadInitialState();

    // update invalidated objects
    mSharedEnv->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    // request redraw
    InvalidateEngineList( 0 );

    // call callbacks if any (for refreshing GUI e.g)
    FOdysseyVectorEngine::Notify( nullptr, mReturnFlags );
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoTagInbetweenerTrajectoryAlter::ToString() const
{
    return FString("FOdysseyVectorUndoTagInbetweenerTrajectoryAlter");
}
