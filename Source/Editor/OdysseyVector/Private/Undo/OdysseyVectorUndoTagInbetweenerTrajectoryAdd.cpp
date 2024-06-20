#include "Undo/OdysseyVectorUndoTagInbetweenerTrajectoryAdd.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorTag.h"

FOdysseyVectorUndoTagInbetweenerTrajectoryAdd::~FOdysseyVectorUndoTagInbetweenerTrajectoryAdd()
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

FOdysseyVectorUndoTagInbetweenerTrajectoryAdd::FOdysseyVectorUndoTagInbetweenerTrajectoryAdd( FOdysseyVectorGroupPaint* iScene
                                                                                            , FOdysseyVectorTagInbetweener* iInbetweenerTag
                                                                                            , FInbetweenerTrajectory* iTrajectory )
    : FOdysseyVectorUndo( iScene )
    , mInbetweenerTag( iInbetweenerTag )
    , mTrajectory( iTrajectory )
{
}

void
FOdysseyVectorUndoTagInbetweenerTrajectoryAdd::Apply( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    mInbetweenerTag->GetGrid()->AddTrajectory( mTrajectory );

    // update invalidated objects
    mScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                               | FOdysseyVectorEngine::SIGNAL_OBJECT_MODIFIED );
}

void
FOdysseyVectorUndoTagInbetweenerTrajectoryAdd::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    mInbetweenerTag->GetGrid()->RemoveTrajectory( mTrajectory );

    // update invalidated objects
    mScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                               | FOdysseyVectorEngine::SIGNAL_OBJECT_MODIFIED );
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoTagInbetweenerTrajectoryAdd::ToString() const
{
    return FString("FOdysseyVectorUndoTagInbetweenerTrajectoryAdd");
}
