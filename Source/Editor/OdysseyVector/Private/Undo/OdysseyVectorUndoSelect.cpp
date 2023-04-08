#include "Undo/OdysseyVectorUndoSelect.h"

FOdysseyVectorUndoSelect::~FOdysseyVectorUndoSelect()
{
    mSelectedObjectList.clear();
}

FOdysseyVectorUndoSelect::FOdysseyVectorUndoSelect( FOdysseyVectorScene* iScene )
    : FOdysseyVectorUndo()
    , mScene( iScene )
{
    mSelectedObjectList = mScene->GetSelectedObjectList();
}

void
FOdysseyVectorUndoSelect::Apply( UObject* iIgnored )
{
    // save former selection
    std::list<FOdysseyVectorObject*> selectedObjectList = mScene->GetSelectedObjectList();
    FOdysseyVectorUndo::Apply( iIgnored );

    mScene->ClearSelection();

    for( std::list<FOdysseyVectorObject*>::iterator it = mSelectedObjectList.begin(); it != mSelectedObjectList.end(); ++it )
    {
        FOdysseyVectorObject* object = static_cast<FOdysseyVectorObject*>(*it);

        mScene->Select( object );
    }

    // prepare former selection for Revert()
    mSelectedObjectList = selectedObjectList;

    // call callbacks if any (for refreshing GUI e.g)
    mRefreshDelegate.Broadcast( mScene );
}

void
FOdysseyVectorUndoSelect::Revert( UObject* iIgnored )
{
    // save former selection
    std::list<FOdysseyVectorObject*> selectedObjectList = mScene->GetSelectedObjectList();
    FOdysseyVectorUndo::Revert( iIgnored );

    mScene->ClearSelection();

    for( std::list<FOdysseyVectorObject*>::iterator it = mSelectedObjectList.begin(); it != mSelectedObjectList.end(); ++it )
    {
        FOdysseyVectorObject* object = static_cast<FOdysseyVectorObject*>(*it);

        mScene->Select( object );
    }

    // prepare former selection for Apply()
    mSelectedObjectList = selectedObjectList;

    // call callbacks if any (for refreshing GUI e.g)
    mRefreshDelegate.Broadcast( mScene );
}

/** Describes this change (for debugging) */
FString
FOdysseyVectorUndoSelect::ToString() const
{
    return FString("FOdysseyVectorUndoSelect");
}
