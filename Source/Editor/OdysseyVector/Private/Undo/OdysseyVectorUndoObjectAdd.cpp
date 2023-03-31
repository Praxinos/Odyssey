#include "Undo/OdysseyVectorUndoObjectAdd.h"

FOdysseyVectorUndoObjectAdd::~FOdysseyVectorUndoObjectAdd()
{
    if( mApplied )
    {
        // nothing to do
    }
    else
    {
        delete mObject;
    }
}

FOdysseyVectorUndoObjectAdd::FOdysseyVectorUndoObjectAdd( FOdysseyVectorObject* iParent, FOdysseyVectorObject* iObject )
    : FOdysseyVectorUndo()
    , mParent( iParent )
    , mObject( iObject )
{
}

void
FOdysseyVectorUndoObjectAdd::Apply( UObject* iIgnored )
{
    FOdysseyVectorScene* scene = mParent->GetScene();
    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    mParent->AppendChild( mObject );

    scene->Select( mObject );

    // call callbacks if any (for refreshing GUI e.g)
    mRefreshDelegate.Broadcast( scene );
}

void
FOdysseyVectorUndoObjectAdd::Revert( UObject* iIgnored )
{
    FOdysseyVectorScene* scene = mParent->GetScene();
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    mParent->RemoveChild( mObject );

    scene->ClearSelection();

    // call callbacks if any (for refreshing GUI e.g)
    mRefreshDelegate.Broadcast( scene );
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoObjectAdd::ToString() const
{
    return FString("FOdysseyVectorUndoObjectAdd");
}
