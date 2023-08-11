#include "Undo/OdysseyVectorUndoSendBackward.h"

FOdysseyVectorUndoSendBackward::~FOdysseyVectorUndoSendBackward()
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

FOdysseyVectorUndoSendBackward::FOdysseyVectorUndoSendBackward( FOdysseyVectorScene* iScene
                                                              , FOdysseyVectorObject* iObject )
    : FOdysseyVectorUndo( iScene )
    , mObject( iObject )
{
}

void
FOdysseyVectorUndoSendBackward::Apply( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    mObject->SendBackward();

    // update invalidated objects
    mScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                               | FOdysseyVectorEngine::SIGNAL_SCENE_HIERARCHY );
}

void
FOdysseyVectorUndoSendBackward::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    mObject->BringForward();

    // update invalidated objects
    mScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );

    mScene->GetEngine()->ResetHUD();
    // call callbacks if any (for refreshing GUI e.g)
    mScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW
                               | FOdysseyVectorEngine::SIGNAL_SCENE_HIERARCHY );
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoSendBackward::ToString() const
{
    return FString("FOdysseyVectorUndoSendBackward");
}
