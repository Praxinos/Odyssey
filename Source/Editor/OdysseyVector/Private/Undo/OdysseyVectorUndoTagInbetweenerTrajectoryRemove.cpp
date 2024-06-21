#include "Undo/OdysseyVectorUndoTagInbetweenerTrajectoryRemove.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorTag.h"

FOdysseyVectorUndoTagInbetweenerTrajectoryRemove::~FOdysseyVectorUndoTagInbetweenerTrajectoryRemove()
{
    if( mApplied )
    {
        delete mTrajectory;
    }
    else
    {
        // nothing to do
    }
}

FOdysseyVectorUndoTagInbetweenerTrajectoryRemove::FOdysseyVectorUndoTagInbetweenerTrajectoryRemove( FOdysseyVectorGroupPaint* iScene
                                                                                                  , FOdysseyVectorTagInbetweener* iInbetweenerTag
                                                                                                  , FInbetweenerTrajectory* iTrajectory )
    : FOdysseyVectorUndo( iScene )
    , mInbetweenerTag( iInbetweenerTag )
    , mTrajectory( iTrajectory )
{
}

void
FOdysseyVectorUndoTagInbetweenerTrajectoryRemove::Apply( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    mInbetweenerTag->GetGrid()->RemoveTrajectory( mTrajectory );

    // update invalidated objects
    mScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                               | FOdysseyVectorEngine::SIGNAL_OBJECT_MODIFIED );
}

void
FOdysseyVectorUndoTagInbetweenerTrajectoryRemove::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    mInbetweenerTag->GetGrid()->AddTrajectory( mTrajectory );

    // update invalidated objects
    mScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                               | FOdysseyVectorEngine::SIGNAL_OBJECT_MODIFIED );
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoTagInbetweenerTrajectoryRemove::ToString() const
{
    return FString("FOdysseyVectorUndoTagInbetweenerTrajectoryRemove");
}
