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

    mObject->MoveBack();

    // update invalidated objects
    mScene->Update(0);
    // call callbacks if any (for refreshing GUI e.g)
    mScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );
}

void
FOdysseyVectorUndoSendBackward::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    mObject->MoveFront();

    // update invalidated objects
    mScene->Update(0);
    // call callbacks if any (for refreshing GUI e.g)
    mScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoSendBackward::ToString() const
{
    return FString("FOdysseyVectorUndoSendBackward");
}
