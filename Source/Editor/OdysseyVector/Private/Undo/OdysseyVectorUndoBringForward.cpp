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
    : FOdysseyVectorUndo()
    , mScene( iScene )
    , mObject( iObject )
{
}

void
FOdysseyVectorUndoBringForward::Apply( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    mObject->MoveFront();

    // call callbacks if any (for refreshing GUI e.g)
    mRefreshDelegate.Broadcast( mScene );
}

void
FOdysseyVectorUndoBringForward::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    mObject->MoveBack();

    // call callbacks if any (for refreshing GUI e.g)
    mRefreshDelegate.Broadcast( mScene );
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoBringForward::ToString() const
{
    return FString("FOdysseyVectorUndoBringForward");
}
