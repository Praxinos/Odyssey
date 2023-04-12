#include "Undo/OdysseyVectorUndoSceneRemoveSelection.h"

FOdysseyVectorUndoSceneRemoveSelection::~FOdysseyVectorUndoSceneRemoveSelection()
{
    // Removal confirmed
    if( mApplied )
    {
        for( std::list<FOdysseyVectorObject*>::iterator it = mRemovedObjectList.begin(); it != mRemovedObjectList.end(); ++it )
        {
            FOdysseyVectorObject* object = (*it);

            delete object;
        }
    }
    else
    {
        // nothing to do
    }
}

FOdysseyVectorUndoSceneRemoveSelection::FOdysseyVectorUndoSceneRemoveSelection( FOdysseyVectorScene* iScene )
    : FOdysseyVectorUndo()
    , mScene( iScene )
{
    mRemovedObjectList = mScene->GetSelectedObjectList();
}

void
FOdysseyVectorUndoSceneRemoveSelection::Apply( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Apply( iIgnored );

    for( std::list<FOdysseyVectorObject*>::iterator it = mRemovedObjectList.begin(); it != mRemovedObjectList.end(); ++it )
    {
        FOdysseyVectorObject* object = (*it);

        object->GetParent()->RemoveChild( object );
    }

    mScene->ClearSelection();
    mScene->Update( 0 );

    // call callbacks if any (for refreshing GUI e.g)
    mRefreshDelegate.Broadcast( mScene );
}

void
FOdysseyVectorUndoSceneRemoveSelection::Revert( UObject* iIgnored )
{
    // call method from base class
    FOdysseyVectorUndo::Revert( iIgnored );

    mScene->ClearSelection();

    for( std::list<FOdysseyVectorObject*>::iterator it = mRemovedObjectList.begin(); it != mRemovedObjectList.end(); ++it )
    {
        FOdysseyVectorObject* object = (*it);

        // Note: GetParent is still valid even though the object was removed from the children list.
        // This helps us to add the object to its parent anew without having to store the pointer to the parent object.
        object->GetParent()->AppendChild( object );

        mScene->Select( object );
    }

    mScene->Update( 0 );

    // call callbacks if any (for refreshing GUI e.g)
    mRefreshDelegate.Broadcast( mScene );
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoSceneRemoveSelection::ToString() const
{
    return FString("FOdysseyVectorUndoSceneRemoveSelection");
}
