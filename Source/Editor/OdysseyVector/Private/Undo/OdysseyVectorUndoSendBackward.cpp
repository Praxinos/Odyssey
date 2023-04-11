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
    : FOdysseyVectorUndo()
    , mScene( iScene )
    , mObject( iObject )
{
}

void
FOdysseyVectorUndoSendBackward::Apply( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    mObject->MoveBack();

    // call callbacks if any (for refreshing GUI e.g)
    mRefreshDelegate.Broadcast( mScene );
}

void
FOdysseyVectorUndoSendBackward::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    mObject->MoveFront();

    // call callbacks if any (for refreshing GUI e.g)
    mRefreshDelegate.Broadcast( mScene );
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoSendBackward::ToString() const
{
    return FString("FOdysseyVectorUndoSendBackward");
}
