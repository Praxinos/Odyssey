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

FOdysseyVectorUndoObjectAdd::FOdysseyVectorUndoObjectAdd( FOdysseyVectorScene* iScene
                                                        , FOdysseyVectorObject* iParent
                                                        , FOdysseyVectorObject* iObject )
    : FOdysseyVectorUndo( iScene )
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

    // update invalidated objects and call callbacks if any (for refreshing GUI e.g)
    mScene->Update(0);
}

void
FOdysseyVectorUndoObjectAdd::Revert( UObject* iIgnored )
{
    FOdysseyVectorScene* scene = mParent->GetScene();
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    mParent->RemoveChild( mObject );

    scene->ClearSelection();

    // update invalidated objects and call callbacks if any (for refreshing GUI e.g)
    mScene->Update(0);
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoObjectAdd::ToString() const
{
    return FString("FOdysseyVectorUndoObjectAdd");
}
