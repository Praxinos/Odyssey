#include "Undo/OdysseyVectorUndoBringForward.h"

FOdysseyVectorUndoBringForward::~FOdysseyVectorUndoBringForward()
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

FOdysseyVectorUndoBringForward::FOdysseyVectorUndoBringForward( FOdysseyVectorScene* iScene
                                                              , FOdysseyVectorObject* iObject )
    : FOdysseyVectorUndo( iScene )
    , mObject( iObject )
{
}

void
FOdysseyVectorUndoBringForward::Apply( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    mObject->MoveFront();

    // update invalidated objects
    mScene->Update(0);
    // call callbacks if any (for refreshing GUI e.g)
    mScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );
}

void
FOdysseyVectorUndoBringForward::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    mObject->MoveBack();

    // update invalidated objects
    mScene->Update(0);
    // call callbacks if any (for refreshing GUI e.g)
    mScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoBringForward::ToString() const
{
    return FString("FOdysseyVectorUndoBringForward");
}
