#include "Undo/OdysseyVectorUndoUngroup.h"

FOdysseyVectorUndoUngroup::~FOdysseyVectorUndoUngroup()
{
    // if Grouping action confirmed
    if( mApplied )
    {
        delete mGroup;
    }
    else
    {
        // nothing to do
    }

    mUngroupedObjectList.clear();
}

FOdysseyVectorUndoUngroup::FOdysseyVectorUndoUngroup( FOdysseyVectorScene* iScene, FOdysseyVectorGroup* iGroup )
    : FOdysseyVectorUndo(iScene)
    , mGroup( iGroup )
{
    mUngroupedObjectList = iGroup->GetChildrenList();
}

void
FOdysseyVectorUndoUngroup::Apply( UObject* iIgnored )
{
    FOdysseyVectorUndo::Apply( iIgnored );

    mScene->ClearSelection();

    for( std::list<FOdysseyVectorObject*>::iterator it = mUngroupedObjectList.begin(); it != mUngroupedObjectList.end(); ++it )
    {
        FOdysseyVectorObject *child = (*it);

        // transfer the child to the group's parent object
        mGroup->GetParent()->TransferChild( child );
    }

    mGroup->GetParent()->RemoveChild( mGroup );

    // update invalidated objects and call callbacks if any (for refreshing GUI e.g)
    mScene->Update(0);
}

void
FOdysseyVectorUndoUngroup::Revert( UObject* iIgnored )
{
    FOdysseyVectorUndo::Revert( iIgnored );

    mScene->ClearSelection();

    // Note: GetParent() stills holds a valid pointer to the former parent.
    mGroup->GetParent()->AppendChild( mGroup );

    for( std::list<FOdysseyVectorObject*>::iterator it = mUngroupedObjectList.begin(); it != mUngroupedObjectList.end(); ++it )
    {
        FOdysseyVectorObject *child = (*it);

        mGroup->TransferChild( child );
    }

    // update invalidated objects and call callbacks if any (for refreshing GUI e.g)
    mScene->Update(0);
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoUngroup::ToString() const
{
    return FString("OdysseyVectorUndoUngroup");
}
